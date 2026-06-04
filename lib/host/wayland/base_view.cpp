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
#include "fractional-scale-v1-client-protocol.h"
#include "viewporter-client-protocol.h"

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
#include <cmath>
#include <map>
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
   struct host_view
   {
      wl_surface*       surface = nullptr;
      libdecor_frame*   frame = nullptr;
      double            scale = 1.0;          // fractional scale
      point             size;                 // logical size
      point             cursor_position;
      bool              configured = false;
      bool              buf_released = true;

      // wp_fractional_scale_v1 + wp_viewport (per-surface HiDPI)
      wp_fractional_scale_v1* frac_scale = nullptr;
      wp_viewport*            viewport = nullptr;

      // shm backing
      wl_shm_pool*      pool = nullptr;
      wl_buffer*        buffer = nullptr;
      void*             data = nullptr;
      int               fd = -1;
      size_t            data_size = 0;
#if defined(ARTIST_CAIRO)
      cairo_surface_t*  cairo = nullptr;
#endif
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
         wp_fractional_scale_manager_v1* frac_mgr = nullptr;
         wp_viewporter* viewporter = nullptr;
         libdecor*      decor = nullptr;

         std::map<wl_surface*, base_view*> views;

         platform()
         {
            display = wl_display_connect(nullptr);
            if (!display)
               throw std::runtime_error("Failed to connect to Wayland display");
            registry = wl_display_get_registry(display);
            wl_registry_add_listener(registry, &registry_listener, this);
            wl_display_roundtrip(display);   // receive globals
            if (!compositor || !shm)
               throw std::runtime_error("Missing required Wayland globals");
            decor = libdecor_new(display, &decor_iface);
         }
      };

      platform& plat()
      {
         static platform the_platform;
         return the_platform;
      }

      void registry_global(void* data, wl_registry* reg, uint32_t name,
                           const char* iface, uint32_t /*ver*/)
      {
         auto* p = static_cast<platform*>(data);
         if (!strcmp(iface, wl_compositor_interface.name))
            p->compositor = static_cast<wl_compositor*>(
               wl_registry_bind(reg, name, &wl_compositor_interface, 4));
         else if (!strcmp(iface, wl_shm_interface.name))
            p->shm = static_cast<wl_shm*>(wl_registry_bind(reg, name, &wl_shm_interface, 1));
         else if (!strcmp(iface, wp_fractional_scale_manager_v1_interface.name))
            p->frac_mgr = static_cast<wp_fractional_scale_manager_v1*>(
               wl_registry_bind(reg, name, &wp_fractional_scale_manager_v1_interface, 1));
         else if (!strcmp(iface, wp_viewporter_interface.name))
            p->viewporter = static_cast<wp_viewporter*>(
               wl_registry_bind(reg, name, &wp_viewporter_interface, 1));
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
      void buffer_release(void* data, wl_buffer*)
      {
         static_cast<host_view*>(data)->buf_released = true;
      }
      constexpr wl_buffer_listener buffer_listener = { buffer_release };

      void create_buffer(host_view* h)
      {
         int const w = int(std::ceil(h->size.x * h->scale));
         int const hgt = int(std::ceil(h->size.y * h->scale));
         int const stride = w * 4;
         size_t const sz = size_t(stride) * hgt;

         if (h->buffer) { wl_buffer_destroy(h->buffer); h->buffer = nullptr; }
         if (h->pool)   { wl_shm_pool_destroy(h->pool); h->pool = nullptr; }
#if defined(ARTIST_CAIRO)
         if (h->cairo)  { cairo_surface_destroy(h->cairo); h->cairo = nullptr; }
#endif
         if (h->data)   { munmap(h->data, h->data_size); h->data = nullptr; }
         if (h->fd >= 0) { close(h->fd); h->fd = -1; }

         h->fd = memfd_create("elements-wl", MFD_CLOEXEC);
         if (h->fd < 0 || ftruncate(h->fd, off_t(sz)) < 0)
            throw std::runtime_error("shm alloc failed");
         h->data_size = sz;
         h->data = mmap(nullptr, sz, PROT_READ | PROT_WRITE, MAP_SHARED, h->fd, 0);
         h->pool = wl_shm_create_pool(plat().shm, h->fd, int32_t(sz));
         h->buffer = wl_shm_pool_create_buffer(h->pool, 0, w, hgt, stride, WL_SHM_FORMAT_ARGB8888);
         wl_buffer_add_listener(h->buffer, &buffer_listener, h);
         h->buf_released = true;
#if defined(ARTIST_CAIRO)
         h->cairo = cairo_image_surface_create_for_data(
            static_cast<unsigned char*>(h->data), CAIRO_FORMAT_ARGB32, w, hgt, stride);
         cairo_surface_set_device_scale(h->cairo, h->scale, h->scale);
#endif
      }

      void render(base_view& view)
      {
         auto* h = platform_access::get_host_view(view);
         if (!h->configured || !h->buffer)
            return;
#if defined(ARTIST_CAIRO)
         auto* cr = cairo_create(h->cairo);
         auto cnv = canvas{cr};
         view.draw(cnv);
         cairo_destroy(cr);
         cairo_surface_flush(h->cairo);
#endif
         int const w = int(std::ceil(h->size.x * h->scale));
         int const hgt = int(std::ceil(h->size.y * h->scale));
         if (h->viewport)
            wp_viewport_set_destination(h->viewport, int(h->size.x), int(h->size.y));
         h->buf_released = false;
         wl_surface_attach(h->surface, h->buffer, 0, 0);
         wl_surface_damage_buffer(h->surface, 0, 0, w, hgt);
         wl_surface_commit(h->surface);
      }

      // wp_fractional_scale_v1: preferred_scale in 1/120ths.
      void frac_preferred(void* data, wp_fractional_scale_v1*, uint32_t scale_120)
      {
         auto* view = static_cast<base_view*>(data);
         auto* h = platform_access::get_host_view(*view);
         double s = double(scale_120) / 120.0;
         if (s != h->scale && h->configured)
         {
            h->scale = s;
            create_buffer(h);
            render(*view);
         }
         else
         {
            h->scale = s;
         }
      }
      constexpr wp_fractional_scale_v1_listener frac_listener = { frac_preferred };
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
      _view->surface = get_surface(*h);
      _view->frame = get_frame(*h);

      // Per-surface fractional scale + viewport (HiDPI).
      if (plat().frac_mgr && plat().viewporter)
      {
         _view->frac_scale = wp_fractional_scale_manager_v1_get_fractional_scale(
            plat().frac_mgr, _view->surface);
         wp_fractional_scale_v1_add_listener(_view->frac_scale, &frac_listener, this);
         _view->viewport = wp_viewporter_get_viewport(plat().viewporter, _view->surface);
      }

      bind_view(*h, this);
   }

   base_view::~base_view()
   {
      plat().views.erase(_view->surface);
#if defined(ARTIST_CAIRO)
      if (_view->cairo)  cairo_surface_destroy(_view->cairo);
#endif
      if (_view->buffer) wl_buffer_destroy(_view->buffer);
      if (_view->pool)   wl_shm_pool_destroy(_view->pool);
      if (_view->data)   munmap(_view->data, _view->data_size);
      if (_view->fd >= 0) close(_view->fd);
      if (_view->viewport)   wp_viewport_destroy(_view->viewport);
      if (_view->frac_scale) wp_fractional_scale_v1_destroy(_view->frac_scale);
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
