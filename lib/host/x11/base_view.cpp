/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements/base_view.hpp>
#include <elements/window.hpp>
#include <infra/assert.hpp>
#include <artist/resources.hpp>
#include <artist/canvas.hpp>
#include "x11_host.hpp"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#include <X11/Xatom.h>
#include <X11/Xcursor/Xcursor.h>
#include <X11/extensions/Xrandr.h>

#if defined(ARTIST_SKIA)
# include <EGL/egl.h>
# include <GL/gl.h>
# include <SkImage.h>
# include <SkColorSpace.h>
# include <SkCanvas.h>
# include <SkSurface.h>
# include <ganesh/GrDirectContext.h>
# include <ganesh/GrBackendSurface.h>
# include <ganesh/SkSurfaceGanesh.h>
# include <ganesh/gl/GrGLInterface.h>
# include <ganesh/gl/GrGLDirectContext.h>
# include <ganesh/gl/GrGLBackendSurface.h>
# include <ganesh/gl/GrGLTypes.h>
# include <ganesh/gl/egl/GrGLMakeEGLInterface.h>
# include <vector>
#elif defined(ARTIST_CAIRO)
# include <cairo.h>
# include <cairo-xlib.h>
#endif

#include <limits.h>
#include <unistd.h>
#include <locale.h>
#include <memory>
#include <string>
#include <string_view>
#include <map>
#include <array>
#include <algorithm>
#include <cstdlib>
#include <cstdint>

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
         fs::path const app_path = exe_path();
         fs::path const app_dir = app_path.parent_path();

         if (app_dir.filename() == "bin")
         {
            fs::path const path = app_dir.parent_path() / "share" / app_path.filename() / "resources";
            if (fs::is_directory(path))
               return path;
         }
         fs::path const app_resources_dir = app_dir / "resources";
         if (fs::is_directory(app_resources_dir))
            return app_resources_dir;
         return fs::current_path() / "resources";
      }
   }

   void init_paths()
   {
      add_search_path(find_resources());
   }

   fs::path get_user_fonts_directory()
   {
      return find_resources();
   }
}

namespace cycfi::elements
{
   // Defined in key.cpp
   key_code translate_key(unsigned keysym);

   ////////////////////////////////////////////////////////////////////////////
   // host_view
   ////////////////////////////////////////////////////////////////////////////
   struct host_view
   {
      using key_map = std::map<key_code, key_action>;

      ::Window          window = 0;       // the X11 window we render into
      double            scale  = 1.0;     // HiDPI scale
      point             size;             // logical size
      point             cursor_position;
      bool              opened = false;
      ::Time            click_time = 0;   // last button-press time (double-click)
      int               click_count = 0;
      ::Time            scroll_time = 0;  // last scroll event (acceleration)
      ::Pixmap          pixmap = 0;       // offscreen back buffer (no flicker)
      GC                gc = nullptr;
      int               pix_w = 0, pix_h = 0;
      XIC               xic = nullptr;    // input context (UTF-8 text entry)
      key_map           keys;
      int               modifiers = 0;
#if defined(ARTIST_CAIRO)
      cairo_surface_t*  surface = nullptr;  // Cairo surface on the pixmap
#elif defined(ARTIST_SKIA)
      EGLDisplay        egl_display = EGL_NO_DISPLAY;
      EGLContext        egl_context = EGL_NO_CONTEXT;
      EGLSurface        egl_surface = EGL_NO_SURFACE;
      sk_sp<const GrGLInterface> xface;
      sk_sp<GrDirectContext>     ctx;
      sk_sp<SkSurface>           skia_surface;
#endif
      std::unique_ptr<drop_info> _drop_info;
   };

   struct platform_access
   {
      inline static host_view* get_host_view(base_view& view)
      {
         return view.host();
      }
   };

   ////////////////////////////////////////////////////////////////////////////
   // Platform layer — shared X11 state, lazily initialised on first use so it
   // works with or without app/window (windowless plugin case).
   ////////////////////////////////////////////////////////////////////////////
   namespace
   {
      struct platform
      {
         Display*  display = nullptr;
         XIM       xim = nullptr;
         Atom      wm_delete = 0;
         Atom      a_clipboard = 0, a_utf8 = 0, a_targets = 0, a_clip_prop = 0;
         ::Window  clip_window = 0;        // hidden selection-owner window
         std::string clip_text;            // clipboard text we own

         std::map<::Window, base_view*>            views;
         std::map<::Window, std::function<void()>> closers;

         std::map<cursor_type, Cursor> cursors;
         ::Window  cursor_window = 0;       // window currently under the cursor
         cursor_type cursor = cursor_type::arrow;

         platform()
         {
            setlocale(LC_ALL, "");
            XSetLocaleModifiers("");

            display = XOpenDisplay(nullptr);
            if (!display)
               throw std::runtime_error("Failed to open X display");

            wm_delete   = XInternAtom(display, "WM_DELETE_WINDOW", False);
            a_clipboard = XInternAtom(display, "CLIPBOARD", False);
            a_utf8      = XInternAtom(display, "UTF8_STRING", False);
            a_targets   = XInternAtom(display, "TARGETS", False);
            a_clip_prop = XInternAtom(display, "ELEMENTS_CLIPBOARD", False);

            xim = XOpenIM(display, nullptr, nullptr, nullptr);

            clip_window = XCreateSimpleWindow(
               display, RootWindow(display, DefaultScreen(display)),
               -10, -10, 1, 1, 0, 0, 0);
         }
      };

      platform& plat()
      {
         static platform the_platform;   // lazy: no app required
         return the_platform;
      }
   }

   Display* get_display()        { return plat().display; }
   Atom     get_wm_delete_atom() { return plat().wm_delete; }
   XIM      get_xim()            { return plat().xim; }

   double display_scale()
   {
      double scale = 1.0;
      if (char* rms = XResourceManagerString(plat().display))
      {
         if (XrmDatabase db = XrmGetStringDatabase(rms))
         {
            XrmValue value;
            char* type = nullptr;
            if (XrmGetResource(db, "Xft.dpi", "Xft.Dpi", &type, &value) && value.addr)
               if (double dpi = std::atof(value.addr); dpi > 0)
                  scale = dpi / 96.0;
            XrmDestroyDatabase(db);
         }
      }
      return scale;
   }

   // Per-monitor scale via XRandR: find the CRTC containing the point, derive
   // its DPI from physical size, round to an integer scale. Falls back to the
   // global Xft.dpi scale if no monitor matches. This gives correct sizing on
   // mixed-DPI multi-monitor setups (works on pure X11 and under XWayland,
   // which presents X11 windows 1:1 in the shared X coordinate space).
   double monitor_scale(int px, int py)
   {
      Display* d = plat().display;
      ::Window root = DefaultRootWindow(d);
      double scale = 0.0;

      if (XRRScreenResources* res = XRRGetScreenResourcesCurrent(d, root))
      {
         for (int i = 0; i < res->ncrtc && scale == 0.0; ++i)
         {
            XRRCrtcInfo* c = XRRGetCrtcInfo(d, res, res->crtcs[i]);
            if (!c)
               continue;
            if (c->width > 0 && c->noutput > 0 &&
                px >= c->x && px < c->x + int(c->width) &&
                py >= c->y && py < c->y + int(c->height))
            {
               if (XRROutputInfo* o = XRRGetOutputInfo(d, res, c->outputs[0]))
               {
                  if (o->mm_width > 0)
                  {
                     double dpi = c->width / (o->mm_width / 25.4);
                     scale = std::max(1.0, std::round(dpi / 96.0));
                  }
                  XRRFreeOutputInfo(o);
               }
            }
            XRRFreeCrtcInfo(c);
         }
         XRRFreeScreenResources(res);
      }
      return scale > 0.0 ? scale : std::max(1.0, display_scale());
   }

   // Scale of the monitor under a window's center.
   double window_scale(::Window w)
   {
      Display* d = plat().display;
      ::Window root = DefaultRootWindow(d), child;
      int rx = 0, ry = 0;
      XTranslateCoordinates(d, w, root, 0, 0, &rx, &ry, &child);
      XWindowAttributes attr;
      XGetWindowAttributes(d, w, &attr);
      return monitor_scale(rx + attr.width / 2, ry + attr.height / 2);
   }

   void register_close(::Window w, std::function<void()> on_close)
   {
      plat().closers[w] = std::move(on_close);
   }
   void unregister_close(::Window w) { plat().closers.erase(w); }

   namespace
   {
      void register_view(::Window w, base_view* view) { plat().views[w] = view; }
      void unregister_view(::Window w)                { plat().views.erase(w); }
      base_view* find_view(::Window w)
      {
         auto i = plat().views.find(w);
         return i == plat().views.end() ? nullptr : i->second;
      }

      bool fire_close(::Window w)
      {
         auto i = plat().closers.find(w);
         if (i == plat().closers.end())
            return false;
         if (i->second)
            i->second();
         return true;
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   // Cursor (themed, HiDPI via libXcursor)
   ////////////////////////////////////////////////////////////////////////////
   namespace
   {
      char const* cursor_name(cursor_type type)
      {
         switch (type)
         {
            case cursor_type::arrow:      return "left_ptr";
            case cursor_type::ibeam:      return "xterm";
            case cursor_type::cross_hair: return "crosshair";
            case cursor_type::hand:       return "hand2";
            case cursor_type::h_resize:   return "sb_h_double_arrow";
            case cursor_type::v_resize:   return "sb_v_double_arrow";
         }
         return "left_ptr";
      }

      // Load (and cache) a themed cursor; XcursorLibraryLoadCursor respects
      // Xcursor.theme / Xcursor.size, so it comes out at the right HiDPI size.
      Cursor load_cursor(cursor_type type)
      {
         auto& cache = plat().cursors;
         if (auto i = cache.find(type); i != cache.end())
            return i->second;
         Cursor c = XcursorLibraryLoadCursor(plat().display, cursor_name(type));
         cache[type] = c;
         return c;
      }

      void apply_cursor(::Window w, cursor_type type)
      {
         if (w)
            if (Cursor c = load_cursor(type))
               XDefineCursor(plat().display, w, c);
      }
   }

   void set_cursor(cursor_type type)
   {
      plat().cursor = type;
      apply_cursor(plat().cursor_window, type);
   }

   ////////////////////////////////////////////////////////////////////////////
   // Clipboard (CLIPBOARD selection)
   ////////////////////////////////////////////////////////////////////////////
   namespace
   {
      void handle_selection_request(XSelectionRequestEvent& req)
      {
         auto& p = plat();
         XSelectionEvent resp = {};
         resp.type      = SelectionNotify;
         resp.display   = req.display;
         resp.requestor = req.requestor;
         resp.selection = req.selection;
         resp.target    = req.target;
         resp.time      = req.time;
         resp.property  = req.property;

         if (req.target == p.a_targets)
         {
            Atom targets[] = {p.a_targets, p.a_utf8, XA_STRING};
            XChangeProperty(p.display, req.requestor, req.property, XA_ATOM, 32,
               PropModeReplace, (unsigned char*)targets,
               int(sizeof(targets) / sizeof(targets[0])));
         }
         else if (req.target == p.a_utf8 || req.target == XA_STRING)
         {
            XChangeProperty(p.display, req.requestor, req.property, req.target, 8,
               PropModeReplace, (unsigned char*)p.clip_text.data(),
               int(p.clip_text.size()));
         }
         else
         {
            resp.property = None;
         }

         XSendEvent(p.display, req.requestor, False, 0, (XEvent*)&resp);
         XFlush(p.display);
      }
   }

   std::string clipboard()
   {
      auto& p = plat();
      if (XGetSelectionOwner(p.display, p.a_clipboard) == p.clip_window)
         return p.clip_text;   // we own it

      XConvertSelection(p.display, p.a_clipboard, p.a_utf8, p.a_clip_prop,
         p.clip_window, CurrentTime);
      XFlush(p.display);

      // Wait (bounded) for the SelectionNotify reply.
      XEvent ev;
      bool got = false;
      for (int i = 0; i < 200 && !got; ++i)
      {
         if (XCheckTypedWindowEvent(p.display, p.clip_window, SelectionNotify, &ev))
            got = true;
         else
            usleep(1000);
      }
      if (!got || ev.xselection.property == None)
         return {};

      Atom type; int fmt; unsigned long nitems, after; unsigned char* data = nullptr;
      XGetWindowProperty(p.display, p.clip_window, p.a_clip_prop, 0, ~0L, True,
         AnyPropertyType, &type, &fmt, &nitems, &after, &data);
      std::string result;
      if (data)
      {
         result.assign((char*)data, nitems);
         XFree(data);
      }
      return result;
   }

   void clipboard(std::string_view text)
   {
      auto& p = plat();
      p.clip_text.assign(text);
      XSetSelectionOwner(p.display, p.a_clipboard, p.clip_window, CurrentTime);
      XFlush(p.display);
   }

   point scroll_direction()
   {
      return {-1.0f, -1.0f};   // traditional; TODO read system setting
   }

   ////////////////////////////////////////////////////////////////////////////
   // Rendering
   ////////////////////////////////////////////////////////////////////////////
   namespace
   {
      void window_pixel_size(::Window w, int& width, int& height)
      {
         XWindowAttributes attr;
         XGetWindowAttributes(plat().display, w, &attr);
         width = attr.width;
         height = attr.height;
      }

#if defined(ARTIST_SKIA)
      // The window was already created (by window.cpp) with the default visual.
      // For EGL we must pick a config whose native visual matches the window's,
      // else eglCreateWindowSurface fails with BadMatch.
      EGLConfig choose_egl_config(EGLDisplay dpy, VisualID vid)
      {
         EGLint attribs[] = {
            EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
            EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8,
            EGL_NONE
         };
         EGLint n = 0;
         eglChooseConfig(dpy, attribs, nullptr, 0, &n);
         if (n <= 0)
            return nullptr;
         std::vector<EGLConfig> cfgs(n);
         eglChooseConfig(dpy, attribs, cfgs.data(), n, &n);
         for (auto c : cfgs)
         {
            EGLint cvid = 0;
            eglGetConfigAttrib(dpy, c, EGL_NATIVE_VISUAL_ID, &cvid);
            if (VisualID(cvid) == vid)
               return c;
         }
         return cfgs[0];   // fallback
      }

      // Initialise EGL on the existing window + the Skia GL context.
      void init_egl_skia(host_view* h)
      {
         Display* d = plat().display;
         h->egl_display = eglGetDisplay((EGLNativeDisplayType)d);
         if (h->egl_display == EGL_NO_DISPLAY || !eglInitialize(h->egl_display, nullptr, nullptr))
            throw std::runtime_error("EGL init failed");
         eglBindAPI(EGL_OPENGL_API);

         XWindowAttributes attr;
         XGetWindowAttributes(d, h->window, &attr);
         EGLConfig cfg = choose_egl_config(h->egl_display, XVisualIDFromVisual(attr.visual));
         if (!cfg)
            throw std::runtime_error("No matching EGL config for window visual");

         EGLint ctx_attribs[] = { EGL_NONE };
         h->egl_context = eglCreateContext(h->egl_display, cfg, EGL_NO_CONTEXT, ctx_attribs);
         h->egl_surface = eglCreateWindowSurface(
            h->egl_display, cfg, (EGLNativeWindowType)h->window, nullptr);
         if (h->egl_context == EGL_NO_CONTEXT || h->egl_surface == EGL_NO_SURFACE)
            throw std::runtime_error("EGL context/surface creation failed");
         if (!eglMakeCurrent(h->egl_display, h->egl_surface, h->egl_surface, h->egl_context))
            throw std::runtime_error("eglMakeCurrent failed");

         h->xface = GrGLMakeNativeInterface();
         if (!h->xface)
            h->xface = GrGLInterfaces::MakeEGL();
         if (!h->xface)
            throw std::runtime_error("GrGLMakeNativeInterface / MakeEGL failed");
         h->ctx = GrDirectContexts::MakeGL(h->xface);
         if (!h->ctx)
            throw std::runtime_error("GrDirectContexts::MakeGL failed");
      }
#endif

      void create_backing(host_view* h, int w, int hgt)
      {
         Display* d = plat().display;
         h->pix_w = w;
         h->pix_h = hgt;

#if defined(ARTIST_CAIRO)
         int screen = DefaultScreen(d);
         if (h->surface) { cairo_surface_destroy(h->surface); h->surface = nullptr; }
         if (h->pixmap)  { XFreePixmap(d, h->pixmap); h->pixmap = 0; }
         h->pixmap = XCreatePixmap(d, h->window, w, hgt, DefaultDepth(d, screen));
         h->surface = cairo_xlib_surface_create(
            d, h->pixmap, DefaultVisual(d, screen), w, hgt);
         cairo_surface_set_device_scale(h->surface, h->scale, h->scale);
#elif defined(ARTIST_SKIA)
         // EGL renders directly into the window (double-buffered via swap), so
         // no pixmap. Recreate the Skia render target wrapping the framebuffer.
         (void)d;
         h->skia_surface.reset();
         eglMakeCurrent(h->egl_display, h->egl_surface, h->egl_surface, h->egl_context);
         GrGLFramebufferInfo info;
         info.fFBOID  = 0;
         info.fFormat = GL_RGBA8;
         auto target = GrBackendRenderTargets::MakeGL(w, hgt, 0, 8, info);
         h->skia_surface = SkSurfaces::WrapBackendRenderTarget(
            h->ctx.get(), target, kBottomLeft_GrSurfaceOrigin,
            kRGBA_8888_SkColorType, nullptr, nullptr);
#endif
      }

      void do_render(base_view& view)
      {
         auto* h = platform_access::get_host_view(view);
         Display* d = plat().display;
#if defined(ARTIST_CAIRO)
         if (!h->surface)
            return;
         auto* cr = cairo_create(h->surface);
         auto cnv = canvas{cr};
         view.draw(cnv);
         cairo_destroy(cr);
         cairo_surface_flush(h->surface);
         if (h->pixmap && h->gc)
            XCopyArea(d, h->pixmap, h->window, h->gc, 0, 0, h->pix_w, h->pix_h, 0, 0);
         XFlush(d);
#elif defined(ARTIST_SKIA)
         (void)d;
         if (!h->skia_surface)
            return;
         SkCanvas* gpu = h->skia_surface->getCanvas();
         gpu->save();
         gpu->scale(h->scale, h->scale);   // logical → physical
         auto cnv = canvas{gpu};
         view.draw(cnv);
         gpu->restore();
         h->ctx->flushAndSubmit(h->skia_surface.get());
         eglSwapBuffers(h->egl_display, h->egl_surface);
#endif
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   // Input helpers
   ////////////////////////////////////////////////////////////////////////////
   namespace
   {
      constexpr ::Time double_click_ms = 250;

      int get_mods(unsigned state)
      {
         int mods = 0;
         if (state & ShiftMask)   mods |= mod_shift;
         if (state & ControlMask) mods |= mod_control | mod_action;
         if (state & Mod1Mask)    mods |= mod_alt;
         if (state & Mod4Mask)    mods |= mod_super;
         return mods;
      }

      point logical_pos(host_view* h, int x, int y)
      {
         return {float(x / h->scale), float(y / h->scale)};
      }

      bool get_button(XButtonEvent& e, mouse_button& btn, host_view* h)
      {
         if (e.button < 1 || e.button > 3)
            return false;

         btn.modifiers = get_mods(e.state);
         if (e.type == ButtonPress)
         {
            btn.down = true;
            if ((e.time - h->click_time) < double_click_ms)
               ++h->click_count;
            else
               h->click_count = 1;
            h->click_time = e.time;
         }
         else
         {
            btn.down = false;
         }

         switch (e.button)
         {
            case 1: btn.state = mouse_button::left;   break;
            case 2: btn.state = mouse_button::middle; break;
            case 3: btn.state = mouse_button::right;  break;
            default: return false;
         }

         btn.num_clicks = h->click_count;
         btn.pos = logical_pos(h, e.x, e.y);
         return true;
      }

      void handle_key(base_view& view, host_view::key_map& keys, key_info k)
      {
         bool repeated = false;
         if (k.action == key_action::release)
         {
            keys.erase(k.key);
            view.key(k);
            return;
         }
         if (k.action == key_action::press && keys[k.key] == key_action::press)
            repeated = true;
         keys[k.key] = k.action;
         if (repeated)
            k.action = key_action::repeat;
         view.key(k);
      }

      int next_codepoint(char const* s, int len, std::uint32_t& cp)
      {
         if (len <= 0)
            return 0;
         unsigned char c = (unsigned char)s[0];
         if (c < 0x80)            { cp = c; return 1; }
         else if ((c >> 5) == 0x6 && len >= 2)
            { cp = ((c & 0x1F) << 6) | (s[1] & 0x3F); return 2; }
         else if ((c >> 4) == 0xE && len >= 3)
            { cp = ((c & 0x0F) << 12) | ((s[1] & 0x3F) << 6) | (s[2] & 0x3F); return 3; }
         else if ((c >> 3) == 0x1E && len >= 4)
            { cp = ((c & 0x07) << 18) | ((s[1] & 0x3F) << 12) |
                   ((s[2] & 0x3F) << 6) | (s[3] & 0x3F); return 4; }
         return 0;
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   // Per-view event handling
   ////////////////////////////////////////////////////////////////////////////
   namespace
   {
      void handle_view_event(base_view& view, XEvent& ev)
      {
         auto* h = platform_access::get_host_view(view);

         switch (ev.type)
         {
            case Expose:
               if (ev.xexpose.count != 0)
                  break;
               if (!h->opened)
               {
                  int w = 0, hgt = 0;
                  window_pixel_size(h->window, w, hgt);
                  h->size = {float(w / h->scale), float(hgt / h->scale)};
                  h->opened = true;
               }
               do_render(view);
               break;

            case ConfigureNotify:
            {
               // Per-monitor HiDPI: if the window moved to a monitor with a
               // different scale, keep the logical size and resize the X window
               // to logical×scale (so it stays the right physical size). The
               // resize triggers another ConfigureNotify at the same scale,
               // which falls into the else-branch — no loop.
               double new_scale = window_scale(h->window);
               if (new_scale != h->scale)
               {
                  h->scale = new_scale;
                  int pw = int(std::lround(h->size.x * new_scale));
                  int ph = int(std::lround(h->size.y * new_scale));
                  XResizeWindow(plat().display, h->window, pw, ph);
                  create_backing(h, pw, ph);   // uses h->scale for device scale
                  do_render(view);
               }
               else
               {
                  int w = ev.xconfigure.width;
                  int hgt = ev.xconfigure.height;
                  if (w != h->pix_w || hgt != h->pix_h)
                  {
                     create_backing(h, w, hgt);
                     h->size = {float(w / h->scale), float(hgt / h->scale)};
                  }
               }
               break;
            }

            case ButtonPress:
            case ButtonRelease:
            {
               unsigned b = ev.xbutton.button;
               if (b >= 4 && b <= 7)
               {
                  if (ev.type != ButtonPress)
                     break;
                  // Time-based acceleration (mirrors the GTK host): faster
                  // gestures send larger steps. elapsed clamped so step ≤ 10.
                  auto elapsed = std::max<float>(10.0f, ev.xbutton.time - h->scroll_time);
                  h->scroll_time = ev.xbutton.time;
                  float step = 100.0f / elapsed;
                  float dx = 0, dy = 0;
                  switch (b)
                  {
                     case 4: dy = +step; break;
                     case 5: dy = -step; break;
                     case 6: dx = +step; break;
                     case 7: dx = -step; break;
                  }
                  view.scroll({dx, dy}, logical_pos(h, ev.xbutton.x, ev.xbutton.y));
                  break;
               }

               mouse_button btn;
               if (get_button(ev.xbutton, btn, h))
               {
                  h->cursor_position = btn.pos;
                  view.click(btn);
               }
               break;
            }

            case MotionNotify:
            {
               point pos = logical_pos(h, ev.xmotion.x, ev.xmotion.y);
               h->cursor_position = pos;

               mouse_button btn;
               btn.modifiers = get_mods(ev.xmotion.state);
               btn.num_clicks = h->click_count;
               btn.pos = pos;

               if (ev.xmotion.state & Button1Mask)
                  { btn.down = true; btn.state = mouse_button::left;   view.drag(btn); }
               else if (ev.xmotion.state & Button2Mask)
                  { btn.down = true; btn.state = mouse_button::middle; view.drag(btn); }
               else if (ev.xmotion.state & Button3Mask)
                  { btn.down = true; btn.state = mouse_button::right;  view.drag(btn); }
               else
                  view.cursor(pos, cursor_tracking::hovering);
               break;
            }

            case EnterNotify:
               h->cursor_position = logical_pos(h, ev.xcrossing.x, ev.xcrossing.y);
               plat().cursor_window = h->window;
               apply_cursor(h->window, plat().cursor);
               view.cursor(h->cursor_position, cursor_tracking::entering);
               break;

            case LeaveNotify:
               h->cursor_position = logical_pos(h, ev.xcrossing.x, ev.xcrossing.y);
               if (plat().cursor_window == h->window)
                  plat().cursor_window = 0;
               view.cursor(h->cursor_position, cursor_tracking::leaving);
               break;

            case FocusIn:
               if (h->xic)
                  XSetICFocus(h->xic);
               view.begin_focus();
               break;

            case FocusOut:
               if (h->xic)
                  XUnsetICFocus(h->xic);
               view.end_focus();
               break;

            case KeyPress:
            {
               h->modifiers = get_mods(ev.xkey.state);

               char buf[64];
               KeySym keysym = NoSymbol;
               int len = 0;
               Status status = XLookupNone;
               if (h->xic)
                  len = Xutf8LookupString(h->xic, &ev.xkey, buf, sizeof(buf), &keysym, &status);
               else
                  len = XLookupString(&ev.xkey, buf, sizeof(buf), &keysym, nullptr);

               if (auto key = translate_key(keysym); key != key_code::unknown)
                  handle_key(view, h->keys, {key, key_action::press, h->modifiers});

               if (len > 0 && (status == XLookupChars || status == XLookupBoth || !h->xic))
               {
                  int i = 0;
                  while (i < len)
                  {
                     std::uint32_t cp = 0;
                     int n = next_codepoint(buf + i, len - i, cp);
                     if (n == 0)
                        break;
                     i += n;
                     if (cp >= 0x20 && cp != 0x7F)
                        view.text({cp, h->modifiers});
                  }
               }
               break;
            }

            case KeyRelease:
            {
               // Filter X11 auto-repeat (release immediately followed by a
               // press of the same key is synthetic).
               if (XPending(plat().display))
               {
                  XEvent next;
                  XPeekEvent(plat().display, &next);
                  if (next.type == KeyPress &&
                      next.xkey.window == ev.xkey.window &&
                      next.xkey.keycode == ev.xkey.keycode &&
                      (next.xkey.time - ev.xkey.time) < 2)
                     break;
               }
               h->modifiers = get_mods(ev.xkey.state);
               KeySym keysym = XLookupKeysym(&ev.xkey, 0);
               if (auto key = translate_key(keysym); key != key_code::unknown)
                  handle_key(view, h->keys, {key, key_action::release, h->modifiers});
               break;
            }

            default:
               break;
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   // Event-pump entry points (used by app::run() or an external host loop)
   ////////////////////////////////////////////////////////////////////////////
   void dispatch_event(XEvent& ev)
   {
      // Clipboard selection traffic (targets the hidden clip window).
      if (ev.type == SelectionRequest)
      {
         handle_selection_request(ev.xselectionrequest);
         return;
      }
      if (ev.type == SelectionClear)
         return;

      // Window-close protocol.
      if (ev.type == ClientMessage &&
          Atom(ev.xclient.data.l[0]) == plat().wm_delete)
      {
         fire_close(ev.xclient.window);
         return;
      }

      if (base_view* view = find_view(ev.xany.window))
         handle_view_event(*view, ev);
   }

   void poll_views()
   {
      for (auto& [w, view] : plat().views)
         view->poll();
   }

   ////////////////////////////////////////////////////////////////////////////
   // base_view
   ////////////////////////////////////////////////////////////////////////////
   base_view::base_view(extent /*size_*/)
    : _view(new host_view)
   {
      CYCFI_ASSERT(false, "Offscreen base_view unimplemented on X11");
   }

   // Adopt an existing host_view (the embedding / custom-host entry point).
   base_view::base_view(host_view_handle h)
    : _view(h)
   {
   }

   base_view::base_view(host_window_handle h)
    : _view(new host_view)
   {
      Display* d = get_display();
      _view->window = get_window(*h);
      _view->scale = window_scale(_view->window);

      if (XIM xim = get_xim())
      {
         _view->xic = XCreateIC(xim,
            XNInputStyle, XIMPreeditNothing | XIMStatusNothing,
            XNClientWindow, _view->window,
            XNFocusWindow, _view->window,
            nullptr);
      }

      XSelectInput(d, _view->window,
         ExposureMask | StructureNotifyMask |
         ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
         KeyPressMask | KeyReleaseMask |
         EnterWindowMask | LeaveWindowMask | FocusChangeMask);

      // Default themed arrow cursor (avoids the tiny core X cursor on enter).
      apply_cursor(_view->window, cursor_type::arrow);

#if defined(ARTIST_CAIRO)
      _view->gc = XCreateGC(d, _view->window, 0, nullptr);
#elif defined(ARTIST_SKIA)
      init_egl_skia(_view);
#endif
      int w = 0, hgt = 0;
      window_pixel_size(_view->window, w, hgt);
      _view->size = {float(w / _view->scale), float(hgt / _view->scale)};
      create_backing(_view, w, hgt);

      register_view(_view->window, this);

      XClearArea(d, _view->window, 0, 0, 0, 0, True);
      XFlush(d);
   }

   base_view::~base_view()
   {
      Display* d = get_display();
      unregister_view(_view->window);
      if (_view->xic)
         XDestroyIC(_view->xic);
#if defined(ARTIST_CAIRO)
      if (_view->surface)
         cairo_surface_destroy(_view->surface);
      if (_view->pixmap)
         XFreePixmap(d, _view->pixmap);
      if (_view->gc)
         XFreeGC(d, _view->gc);
#elif defined(ARTIST_SKIA)
      _view->skia_surface.reset();
      _view->ctx.reset();
      _view->xface.reset();
      if (_view->egl_surface != EGL_NO_SURFACE)
         eglDestroySurface(_view->egl_display, _view->egl_surface);
      if (_view->egl_context != EGL_NO_CONTEXT)
         eglDestroyContext(_view->egl_display, _view->egl_context);
      if (_view->egl_display != EGL_NO_DISPLAY)
         eglTerminate(_view->egl_display);
#endif
      delete _view;
      _view = nullptr;
   }

   point base_view::cursor_pos() const
   {
      return _view->cursor_position;
   }

   elements::extent base_view::size() const
   {
      return {_view->size.x, _view->size.y};
   }

   void base_view::size(elements::extent p)
   {
      XResizeWindow(get_display(), _view->window,
         (unsigned)(p.x * _view->scale), (unsigned)(p.y * _view->scale));
   }

   void base_view::refresh()
   {
      // Render into the back buffer and blit — no window clear, so no flicker.
      do_render(*this);
   }

   void base_view::refresh(rect /*area*/)
   {
      do_render(*this);
   }
}
