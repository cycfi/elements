/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements/base_view.hpp>
#include <elements/window.hpp>
#include <infra/assert.hpp>
#include <artist/resources.hpp>
#include <artist/canvas.hpp>
#include "wayland_host.hpp"

#include <wayland-client.h>
#include <libdecor.h>
#include <xkbcommon/xkbcommon.h>
#include <linux/input-event-codes.h>

#if defined(ARTIST_SKIA)
# error "Wayland Skia host not yet implemented — build with ELEMENTS_CAIRO=ON for now"
#elif defined(ARTIST_CAIRO)
# include <cairo.h>
#endif

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <map>
#include <set>
#include <algorithm>
#include <memory>
#include <string>
#include <string_view>
#include <stdexcept>

namespace cycfi::artist
{
   namespace
   {
      fs::path exe_path()
      {
         char result[PATH_MAX];
         ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
         return std::string(result, (count > 0)? count : 0);
      }

      fs::path find_resources()
      {
         fs::path const app_dir = exe_path().parent_path();
         if (app_dir.filename() == "bin")
         {
            fs::path const p = app_dir.parent_path() / "share" / exe_path().filename() / "resources";
            if (fs::is_directory(p)) return p;
         }
         fs::path const r = app_dir / "resources";
         if (fs::is_directory(r)) return r;
         return fs::current_path() / "resources";
      }
   }

   void init_paths() { add_search_path(find_resources()); }
   fs::path get_user_fonts_directory() { return find_resources(); }
}

namespace cycfi::elements
{
   // Defined in window.cpp
   wl_surface*     get_surface(host_window& h);
   libdecor_frame* get_frame(host_window& h);

   // Defined in key.cpp
   key_code translate_key(unsigned keysym);

   ////////////////////////////////////////////////////////////////////////////
   struct host_view;

   // One double-buffered shm buffer.
   struct shm_buffer
   {
      wl_buffer*        buffer = nullptr;
      wl_shm_pool*      pool = nullptr;
      void*             data = nullptr;
      int               fd = -1;
      size_t            size = 0;
#if defined(ARTIST_CAIRO)
      cairo_surface_t*  cairo = nullptr;
#endif
      host_view*        owner = nullptr;
      bool              busy = false;          // attached, compositor may be using it
   };

   struct host_view
   {
      wl_surface*       surface = nullptr;
      libdecor_frame*   frame = nullptr;
      double            scale = 1.0;          // fractional scale
      point             size;                 // logical size
      point             cursor_position;
      base_view*        self = nullptr;       // back-pointer (for deferred render)
      bool              configured = false;
      bool              pending = false;       // a render was deferred (both busy)

      // Integer per-monitor HiDPI: the outputs the surface is on; scale = the
      // max of their wl_output scales (set via wl_surface_set_buffer_scale).
      std::set<wl_output*> outputs;

      shm_buffer        bufs[2];               // double-buffered shm
      std::unique_ptr<drop_info> _drop_info;
   };

   struct platform_access
   {
      inline static host_view* get_host_view(base_view& v) { return v.host(); }
   };

   ////////////////////////////////////////////////////////////////////////////
   // Platform (lazy singleton)
   ////////////////////////////////////////////////////////////////////////////
   namespace
   {
      void registry_global(void*, wl_registry*, uint32_t, const char*, uint32_t);
      void registry_remove(void*, wl_registry*, uint32_t) {}
      constexpr wl_registry_listener registry_listener = { registry_global, registry_remove };

      void libdecor_error(libdecor*, libdecor_error, const char* msg)
      {
         throw std::runtime_error(std::string("libdecor: ") + msg);
      }
      libdecor_interface decor_iface = { libdecor_error };

      struct platform
      {
         wl_display*    display = nullptr;
         wl_registry*   registry = nullptr;
         wl_compositor* compositor = nullptr;
         wl_shm*        shm = nullptr;
         libdecor*      decor = nullptr;
         std::map<wl_output*, int32_t> output_scale;

         // Input (wl_seat + xkbcommon)
         wl_seat*       seat = nullptr;
         wl_pointer*    pointer = nullptr;
         wl_keyboard*   keyboard = nullptr;
         xkb_context*   xkb_ctx = nullptr;
         xkb_keymap*    xkb_map = nullptr;
         xkb_state*     xkb_st = nullptr;
         base_view*     pointer_focus = nullptr;
         base_view*     keyboard_focus = nullptr;
         point          pointer_pos;
         uint32_t       click_time = 0;
         int            click_count = 0;
         uint32_t       last_button = 0;
         int            down_button = 0;   // 0=none,1=left,2=middle,3=right

         std::map<wl_surface*, base_view*> views;

         // Ctor does NO roundtrips — just connect + registry + xkb context, so
         // no Wayland listener fires before the singleton pointer is set.
         platform()
         {
            display = wl_display_connect(nullptr);
            if (!display)
               throw std::runtime_error("Failed to connect to Wayland display");
            xkb_ctx = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
            registry = wl_display_get_registry(display);
            wl_registry_add_listener(registry, &registry_listener, this);
         }

         // Roundtrips happen here, AFTER the singleton pointer is assigned, so
         // listeners (seat_caps, kb_keymap, ...) calling plat() see a valid
         // pointer instead of re-entering a still-initializing function-local
         // static (which would throw recursive_init_error).
         void init()
         {
            wl_display_roundtrip(display);   // receive globals
            if (!compositor || !shm)
               throw std::runtime_error("Missing required Wayland globals");
            wl_display_roundtrip(display);   // receive seat capabilities + keymap
            decor = libdecor_new(display, &decor_iface);
         }
      };

      platform* g_platform = nullptr;
      platform& plat()
      {
         if (!g_platform)
         {
            g_platform = new platform();   // pointer set BEFORE init() roundtrips
            g_platform->init();
         }
         return *g_platform;
      }

      ////////////////////////////////////////////////////////////////////////
      // Input (wl_seat: pointer + keyboard via xkbcommon)
      ////////////////////////////////////////////////////////////////////////
      int get_mods()
      {
         auto& p = plat();
         if (!p.xkb_st)
            return 0;
         int m = 0;
         auto on = [&](const char* n)
            { return xkb_state_mod_name_is_active(p.xkb_st, n, XKB_STATE_MODS_EFFECTIVE) > 0; };
         if (on(XKB_MOD_NAME_SHIFT)) m |= mod_shift;
         if (on(XKB_MOD_NAME_CTRL))  m |= mod_control | mod_action;
         if (on(XKB_MOD_NAME_ALT))   m |= mod_alt;
         if (on(XKB_MOD_NAME_LOGO))  m |= mod_super;
         return m;
      }

      // --- pointer ---
      void ptr_enter(void*, wl_pointer*, uint32_t, wl_surface* surf, wl_fixed_t x, wl_fixed_t y)
      {
         auto& p = plat();
         auto i = p.views.find(surf);
         p.pointer_focus = (i != p.views.end()) ? i->second : nullptr;
         if (p.pointer_focus)
         {
            p.pointer_pos = {float(wl_fixed_to_double(x)), float(wl_fixed_to_double(y))};
            platform_access::get_host_view(*p.pointer_focus)->cursor_position = p.pointer_pos;
            p.pointer_focus->cursor(p.pointer_pos, cursor_tracking::entering);
         }
      }
      void ptr_leave(void*, wl_pointer*, uint32_t, wl_surface*)
      {
         auto& p = plat();
         if (p.pointer_focus)
            p.pointer_focus->cursor(p.pointer_pos, cursor_tracking::leaving);
         p.pointer_focus = nullptr;
         p.down_button = 0;
      }
      void ptr_motion(void*, wl_pointer*, uint32_t, wl_fixed_t x, wl_fixed_t y)
      {
         auto& p = plat();
         if (!p.pointer_focus)
            return;
         p.pointer_pos = {float(wl_fixed_to_double(x)), float(wl_fixed_to_double(y))};
         platform_access::get_host_view(*p.pointer_focus)->cursor_position = p.pointer_pos;
         if (p.down_button)
         {
            mouse_button btn;
            btn.down = true;
            btn.modifiers = get_mods();
            btn.num_clicks = p.click_count;
            btn.pos = p.pointer_pos;
            btn.state = mouse_button::what(p.down_button - 1);  // left=0,middle=1,right=2
            p.pointer_focus->drag(btn);
         }
         else
         {
            p.pointer_focus->cursor(p.pointer_pos, cursor_tracking::hovering);
         }
      }
      void ptr_button(void*, wl_pointer*, uint32_t, uint32_t time, uint32_t button, uint32_t state)
      {
         auto& p = plat();
         if (!p.pointer_focus)
            return;
         mouse_button::what w;
         int idx;
         switch (button)
         {
            case BTN_LEFT:   w = mouse_button::left;   idx = 1; break;
            case BTN_MIDDLE: w = mouse_button::middle; idx = 2; break;
            case BTN_RIGHT:  w = mouse_button::right;  idx = 3; break;
            default: return;
         }
         mouse_button btn;
         btn.modifiers = get_mods();
         btn.pos = p.pointer_pos;
         btn.state = w;
         if (state == WL_POINTER_BUTTON_STATE_PRESSED)
         {
            btn.down = true;
            if ((time - p.click_time) < 250 && button == p.last_button)
               ++p.click_count;
            else
               p.click_count = 1;
            p.click_time = time;
            p.last_button = button;
            p.down_button = idx;
         }
         else
         {
            btn.down = false;
            p.down_button = 0;
         }
         btn.num_clicks = p.click_count;
         p.pointer_focus->click(btn);
      }
      void ptr_axis(void*, wl_pointer*, uint32_t, uint32_t axis, wl_fixed_t value)
      {
         auto& p = plat();
         if (!p.pointer_focus)
            return;
         float v = float(wl_fixed_to_double(value));
         float dx = 0, dy = 0;
         if (axis == WL_POINTER_AXIS_VERTICAL_SCROLL)
            dy = -v * 0.5f;
         else
            dx = -v * 0.5f;
         p.pointer_focus->scroll({dx, dy}, p.pointer_pos);
      }
      void ptr_frame(void*, wl_pointer*) {}
      void ptr_axis_source(void*, wl_pointer*, uint32_t) {}
      void ptr_axis_stop(void*, wl_pointer*, uint32_t, uint32_t) {}
      void ptr_axis_discrete(void*, wl_pointer*, uint32_t, int32_t) {}
      void ptr_axis_value120(void*, wl_pointer*, uint32_t, int32_t) {}
      void ptr_axis_dir(void*, wl_pointer*, uint32_t, uint32_t) {}
      constexpr wl_pointer_listener pointer_listener = {
         ptr_enter, ptr_leave, ptr_motion, ptr_button, ptr_axis, ptr_frame,
         ptr_axis_source, ptr_axis_stop, ptr_axis_discrete, ptr_axis_value120,
         ptr_axis_dir
      };

      // --- keyboard ---
      void kb_keymap(void*, wl_keyboard*, uint32_t format, int32_t fd, uint32_t size)
      {
         auto& p = plat();
         if (format != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1) { close(fd); return; }
         char* str = static_cast<char*>(mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0));
         if (str != MAP_FAILED)
         {
            if (p.xkb_map) xkb_keymap_unref(p.xkb_map);
            p.xkb_map = xkb_keymap_new_from_string(
               p.xkb_ctx, str, XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS);
            munmap(str, size);
            if (p.xkb_st) xkb_state_unref(p.xkb_st);
            p.xkb_st = p.xkb_map ? xkb_state_new(p.xkb_map) : nullptr;
         }
         close(fd);
      }
      void kb_enter(void*, wl_keyboard*, uint32_t, wl_surface* surf, wl_array*)
      {
         auto& p = plat();
         auto i = p.views.find(surf);
         p.keyboard_focus = (i != p.views.end()) ? i->second : nullptr;
         if (p.keyboard_focus)
            p.keyboard_focus->begin_focus();
      }
      void kb_leave(void*, wl_keyboard*, uint32_t, wl_surface*)
      {
         auto& p = plat();
         if (p.keyboard_focus)
            p.keyboard_focus->end_focus();
         p.keyboard_focus = nullptr;
      }
      void kb_key(void*, wl_keyboard*, uint32_t, uint32_t, uint32_t key, uint32_t state)
      {
         auto& p = plat();
         if (!p.keyboard_focus || !p.xkb_st)
            return;
         uint32_t keycode = key + 8;   // evdev → xkb
         xkb_keysym_t sym = xkb_state_key_get_one_sym(p.xkb_st, keycode);
         bool pressed = (state == WL_KEYBOARD_KEY_STATE_PRESSED);
         int mods = get_mods();

         if (auto kc = translate_key(sym); kc != key_code::unknown)
            p.keyboard_focus->key({kc, pressed ? key_action::press : key_action::release, mods});

         if (pressed)
         {
            uint32_t cp = xkb_state_key_get_utf32(p.xkb_st, keycode);
            if (cp >= 0x20 && cp != 0x7F)
               p.keyboard_focus->text({cp, mods});
         }
      }
      void kb_mods(void*, wl_keyboard*, uint32_t, uint32_t dep, uint32_t lat, uint32_t lock, uint32_t group)
      {
         auto& p = plat();
         if (p.xkb_st)
            xkb_state_update_mask(p.xkb_st, dep, lat, lock, 0, 0, group);
      }
      void kb_repeat(void*, wl_keyboard*, int32_t, int32_t) {}
      constexpr wl_keyboard_listener keyboard_listener = {
         kb_keymap, kb_enter, kb_leave, kb_key, kb_mods, kb_repeat
      };

      // --- output (per-monitor integer scale) ---
      void out_geometry(void*, wl_output*, int32_t,int32_t,int32_t,int32_t,int32_t,const char*,const char*,int32_t) {}
      void out_mode(void*, wl_output*, uint32_t,int32_t,int32_t,int32_t) {}
      void out_done(void*, wl_output*) {}
      void out_scale(void*, wl_output* o, int32_t factor) { plat().output_scale[o] = factor; }
      void out_name(void*, wl_output*, const char*) {}
      void out_description(void*, wl_output*, const char*) {}
      constexpr wl_output_listener output_listener = {
         out_geometry, out_mode, out_done, out_scale, out_name, out_description
      };

      // --- seat ---
      // Uses the passed platform* (not plat()) — this can fire during the
      // platform's own construction (registry roundtrip), and calling plat()
      // then would re-enter the still-initializing function-local static.
      void seat_caps(void* data, wl_seat* seat, uint32_t caps)
      {
         auto& p = *static_cast<platform*>(data);
         if ((caps & WL_SEAT_CAPABILITY_POINTER) && !p.pointer)
         {
            p.pointer = wl_seat_get_pointer(seat);
            wl_pointer_add_listener(p.pointer, &pointer_listener, nullptr);
         }
         if ((caps & WL_SEAT_CAPABILITY_KEYBOARD) && !p.keyboard)
         {
            p.keyboard = wl_seat_get_keyboard(seat);
            wl_keyboard_add_listener(p.keyboard, &keyboard_listener, nullptr);
         }
      }
      void seat_name(void*, wl_seat*, const char*) {}
      constexpr wl_seat_listener seat_listener = { seat_caps, seat_name };

      void registry_global(void* data, wl_registry* reg, uint32_t name,
                           const char* iface, uint32_t /*ver*/)
      {
         auto* p = static_cast<platform*>(data);
         if (!strcmp(iface, wl_compositor_interface.name))
            p->compositor = static_cast<wl_compositor*>(
               wl_registry_bind(reg, name, &wl_compositor_interface, 4));
         else if (!strcmp(iface, wl_shm_interface.name))
            p->shm = static_cast<wl_shm*>(wl_registry_bind(reg, name, &wl_shm_interface, 1));
         else if (!strcmp(iface, wl_output_interface.name))
         {
            auto* o = static_cast<wl_output*>(wl_registry_bind(reg, name, &wl_output_interface, 2));
            p->output_scale[o] = 1;
            wl_output_add_listener(o, &output_listener, nullptr);
         }
         else if (!strcmp(iface, wl_seat_interface.name))
         {
            p->seat = static_cast<wl_seat*>(wl_registry_bind(reg, name, &wl_seat_interface, 5));
            wl_seat_add_listener(p->seat, &seat_listener, p);   // pass platform*, not plat()
         }
      }
   }

   wl_display*    get_display()    { return plat().display; }
   libdecor*      get_libdecor()   { return plat().decor; }
   wl_compositor* get_compositor() { return plat().compositor; }
   wl_shm*        get_shm()        { return plat().shm; }

   ////////////////////////////////////////////////////////////////////////////
   // Rendering (wl_shm + Cairo)
   ////////////////////////////////////////////////////////////////////////////
   namespace
   {
      void render(base_view& view);   // fwd

      void destroy_buffer(shm_buffer& b)
      {
#if defined(ARTIST_CAIRO)
         if (b.cairo)  { cairo_surface_destroy(b.cairo); b.cairo = nullptr; }
#endif
         if (b.buffer) { wl_buffer_destroy(b.buffer); b.buffer = nullptr; }
         if (b.pool)   { wl_shm_pool_destroy(b.pool); b.pool = nullptr; }
         if (b.data)   { munmap(b.data, b.size); b.data = nullptr; }
         if (b.fd >= 0) { close(b.fd); b.fd = -1; }
         b.busy = false;
      }

      void buffer_release(void* data, wl_buffer*)
      {
         auto* b = static_cast<shm_buffer*>(data);
         b->busy = false;
         if (b->owner && b->owner->pending && b->owner->self)
            render(*b->owner->self);   // a deferred render can now proceed
      }
      constexpr wl_buffer_listener buffer_listener = { buffer_release };

      void make_buffer(shm_buffer& b, host_view* h, int w, int hgt)
      {
         destroy_buffer(b);
         b.owner = h;
         int const stride = w * 4;
         size_t const sz = size_t(stride) * hgt;
         b.fd = memfd_create("elements-wl", MFD_CLOEXEC);
         if (b.fd < 0 || ftruncate(b.fd, off_t(sz)) < 0)
            throw std::runtime_error("shm alloc failed");
         b.size = sz;
         b.data = mmap(nullptr, sz, PROT_READ | PROT_WRITE, MAP_SHARED, b.fd, 0);
         b.pool = wl_shm_create_pool(plat().shm, b.fd, int32_t(sz));
         b.buffer = wl_shm_pool_create_buffer(b.pool, 0, w, hgt, stride, WL_SHM_FORMAT_ARGB8888);
         wl_buffer_add_listener(b.buffer, &buffer_listener, &b);
#if defined(ARTIST_CAIRO)
         b.cairo = cairo_image_surface_create_for_data(
            static_cast<unsigned char*>(b.data), CAIRO_FORMAT_ARGB32, w, hgt, stride);
         cairo_surface_set_device_scale(b.cairo, h->scale, h->scale);
#endif
      }

      void create_buffer(host_view* h)
      {
         int const w = int(std::ceil(h->size.x * h->scale));
         int const hgt = int(std::ceil(h->size.y * h->scale));
         make_buffer(h->bufs[0], h, w, hgt);
         make_buffer(h->bufs[1], h, w, hgt);
      }

      void render(base_view& view)
      {
         auto* h = platform_access::get_host_view(view);
         if (!h->configured)
            return;
         // Pick a free buffer; if both are still held by the compositor, defer
         // and re-render on the next release (no tearing, no stall).
         shm_buffer* b = !h->bufs[0].busy ? &h->bufs[0]
                       : !h->bufs[1].busy ? &h->bufs[1] : nullptr;
         if (!b || !b->buffer)
         {
            h->pending = true;
            return;
         }
         h->pending = false;
#if defined(ARTIST_CAIRO)
         auto* cr = cairo_create(b->cairo);
         auto cnv = canvas{cr};
         view.draw(cnv);
         cairo_destroy(cr);
         cairo_surface_flush(b->cairo);
#endif
         int const w = int(std::ceil(h->size.x * h->scale));
         int const hgt = int(std::ceil(h->size.y * h->scale));
         b->busy = true;
         wl_surface_attach(h->surface, b->buffer, 0, 0);
         wl_surface_damage_buffer(h->surface, 0, 0, w, hgt);
         wl_surface_commit(h->surface);
      }

      void update_view_scale(base_view* view)
      {
         auto* h = platform_access::get_host_view(*view);
         int sc = 1;
         for (auto* o : h->outputs)
            if (auto it = plat().output_scale.find(o); it != plat().output_scale.end())
               sc = std::max(sc, it->second);
         if (double(sc) != h->scale)
         {
            h->scale = sc;
            wl_surface_set_buffer_scale(h->surface, sc);
            if (h->configured)
            {
               // Preserve the LOGICAL size across the scale change. Otherwise
               // libdecor keeps the buffer size and halves the logical size
               // (e.g. 640 logical → 320 on a 1×→2× move). A client-initiated
               // state commit (null configuration) tells libdecor the content
               // is still h->size logical, so it doubles the buffer instead.
               auto* st = libdecor_state_new(
                  int(std::lround(h->size.x)), int(std::lround(h->size.y)));
               libdecor_frame_commit(h->frame, st, nullptr);
               libdecor_state_free(st);
               create_buffer(h);
               render(*view);
            }
         }
      }
      void surface_enter(void* data, wl_surface*, wl_output* output)
      {
         auto* view = static_cast<base_view*>(data);
         platform_access::get_host_view(*view)->outputs.insert(output);
         update_view_scale(view);
      }
      void surface_leave(void* data, wl_surface*, wl_output* output)
      {
         auto* view = static_cast<base_view*>(data);
         platform_access::get_host_view(*view)->outputs.erase(output);
         update_view_scale(view);
      }
      void surface_pref_scale(void*, wl_surface*, int32_t) {}
      void surface_pref_transform(void*, wl_surface*, uint32_t) {}
      constexpr wl_surface_listener surface_listener = {
         surface_enter, surface_leave, surface_pref_scale, surface_pref_transform
      };
   }

   ////////////////////////////////////////////////////////////////////////////
   // Hooks called from window.cpp's libdecor frame callbacks
   ////////////////////////////////////////////////////////////////////////////
   void bind_view(host_window& w, base_view* view)
   {
      plat().views[get_surface(w)] = view;
   }

   void view_configure(host_window& w, int content_w, int content_h)
   {
      auto i = plat().views.find(get_surface(w));
      if (i == plat().views.end())
         return;
      auto* h = platform_access::get_host_view(*i->second);
      h->size = {float(content_w), float(content_h)};
      h->configured = true;
      if (std::getenv("ELEMENTS_WL_DEBUG"))
         std::fprintf(stderr, "[wl] configure content=%dx%d scale=%g buffer=%dx%d\n",
            content_w, content_h, h->scale,
            int(std::ceil(content_w * h->scale)), int(std::ceil(content_h * h->scale)));
      create_buffer(h);
   }

   void view_render(host_window& w)
   {
      auto i = plat().views.find(get_surface(w));
      if (i != plat().views.end())
         render(*i->second);
   }

   ////////////////////////////////////////////////////////////////////////////
   void dispatch_events()
   {
      wl_display_dispatch_pending(plat().display);
      wl_display_flush(plat().display);
   }

   void poll_views()
   {
      for (auto& [s, view] : plat().views)
         view->poll();
   }

   ////////////////////////////////////////////////////////////////////////////
   // base_view
   ////////////////////////////////////////////////////////////////////////////
   base_view::base_view(extent /*size_*/)
    : _view(new host_view)
   {
      CYCFI_ASSERT(false, "Offscreen base_view unimplemented on Wayland");
   }

   base_view::base_view(host_view_handle h)
    : _view(h)
   {
   }

   base_view::base_view(host_window_handle h)
    : _view(new host_view)
   {
      _view->self = this;
      _view->surface = get_surface(*h);
      _view->frame = get_frame(*h);

      wl_surface_add_listener(_view->surface, &surface_listener, this);

      bind_view(*h, this);
   }

   base_view::~base_view()
   {
      plat().views.erase(_view->surface);
      destroy_buffer(_view->bufs[0]);
      destroy_buffer(_view->bufs[1]);
      delete _view;
      _view = nullptr;
   }

   point base_view::cursor_pos() const { return _view->cursor_position; }

   elements::extent base_view::size() const { return {_view->size.x, _view->size.y}; }

   void base_view::size(elements::extent /*p*/) { /* window-driven; no-op */ }

   void base_view::refresh()
   {
      render(*this);
   }

   void base_view::refresh(rect /*area*/)
   {
      render(*this);
   }

   ////////////////////////////////////////////////////////////////////////////
   // Clipboard / cursor / scroll — stubs for Phase W3
   std::string clipboard() { return {}; }
   void clipboard(std::string_view) {}
   void set_cursor(cursor_type) {}
   point scroll_direction() { return {-1.0f, -1.0f}; }
}
