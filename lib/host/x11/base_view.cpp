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

#if defined(ARTIST_SKIA)
# error "X11 Skia host not yet implemented — build Elements with ELEMENTS_CAIRO=ON for now"
#elif defined(ARTIST_CAIRO)
# include <cairo.h>
# include <cairo-xlib.h>
#endif

#include <limits.h>
#include <unistd.h>
#include <memory>
#include <string>

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
   // Defined in window.cpp
   ::Window get_window(host_window& h);

   struct host_view
   {
      ::Window          window = 0;       // the X11 window we render into
      double            scale  = 1.0;     // HiDPI scale
      point             size;             // logical size
      point             cursor_position;
      bool              opened = false;   // has on_open fired?
#if defined(ARTIST_CAIRO)
      cairo_surface_t*  surface = nullptr;
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

   namespace
   {
      // Current window pixel size
      void window_pixel_size(::Window w, int& width, int& height)
      {
         XWindowAttributes attr;
         XGetWindowAttributes(get_display(), w, &attr);
         width = attr.width;
         height = attr.height;
      }

      void do_render(base_view& view)
      {
         auto* h = platform_access::get_host_view(view);
#if defined(ARTIST_CAIRO)
         if (!h->surface)
            return;
         auto* cr = cairo_create(h->surface);
         auto cnv = canvas{cr};
         view.draw(cnv);
         cairo_destroy(cr);
         cairo_surface_flush(h->surface);
         XFlush(get_display());
#endif
      }
   }

   // Called from the app event loop (declared in x11_host.hpp)
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
            int w = ev.xconfigure.width;
            int hgt = ev.xconfigure.height;
#if defined(ARTIST_CAIRO)
            if (h->surface)
               cairo_xlib_surface_set_size(h->surface, w, hgt);
#endif
            h->size = {float(w / h->scale), float(hgt / h->scale)};
            break;
         }

         // Input events handled in a later phase.
         default:
            break;
      }
   }

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
      _view->scale = display_scale();

      XSelectInput(d, _view->window,
         ExposureMask | StructureNotifyMask |
         ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
         KeyPressMask | KeyReleaseMask |
         EnterWindowMask | LeaveWindowMask);

#if defined(ARTIST_CAIRO)
      int screen = DefaultScreen(d);
      int w = 0, hgt = 0;
      window_pixel_size(_view->window, w, hgt);
      _view->surface = cairo_xlib_surface_create(
         d, _view->window, DefaultVisual(d, screen), w, hgt);
      // Device scale lets draw() use logical coordinates while rendering at
      // full physical resolution on HiDPI displays.
      cairo_surface_set_device_scale(_view->surface, _view->scale, _view->scale);
#endif

      register_view(_view->window, this);

      // Trigger an initial Expose now that input is selected.
      XClearArea(d, _view->window, 0, 0, 0, 0, True);
      XFlush(d);
   }

   base_view::~base_view()
   {
      unregister_view(_view->window);
#if defined(ARTIST_CAIRO)
      if (_view->surface)
         cairo_surface_destroy(_view->surface);
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
      Display* d = get_display();
      XResizeWindow(d, _view->window,
         (unsigned)(p.x * _view->scale), (unsigned)(p.y * _view->scale));
   }

   void base_view::refresh()
   {
      Display* d = get_display();
      XClearArea(d, _view->window, 0, 0, 0, 0, True);
      XFlush(d);
   }

   void base_view::refresh(rect area)
   {
      Display* d = get_display();
      XClearArea(d, _view->window,
         int(area.left * _view->scale), int(area.top * _view->scale),
         (unsigned)std::max<float>(area.width() * _view->scale, 1),
         (unsigned)std::max<float>(area.height() * _view->scale, 1),
         True);
      XFlush(d);
   }

   // -------------------------------------------------------------------------
   // Clipboard / cursor / scroll — minimal stubs for Phase 1
   std::string clipboard()
   {
      return {};   // TODO: X11 selection (PRIMARY/CLIPBOARD)
   }

   void clipboard(std::string_view /*text*/)
   {
      // TODO: X11 selection ownership
   }

   void set_cursor(cursor_type /*type*/)
   {
      // TODO (Phase 2): load themed cursors via libXcursor
      // (XcursorLibraryLoadCursor) so they respect Xcursor.theme / Xcursor.size
      // and come out at the correct HiDPI scale, then XDefineCursor on the view
      // window. Without this, XWayland gives the window the tiny 1x core X
      // cursor on enter instead of the scaled themed desktop cursor.
      // Map cursor_type::arrow/ibeam/cross_hair/hand/h_resize/v_resize to the
      // corresponding theme cursor names (left_ptr, xterm, crosshair, hand2,
      // sb_h_double_arrow, sb_v_double_arrow).
   }

   point scroll_direction()
   {
      return {-1.0f, -1.0f};   // traditional; TODO read system setting
   }
}
