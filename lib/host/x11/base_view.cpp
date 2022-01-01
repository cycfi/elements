/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements/app.hpp>
#include <cairo.h>
#include <infra/assert.hpp>
#include <elements/base_view.hpp>
#include <elements/window.hpp>
#include <elements/support/resource_paths.hpp>

#include <X11/Xlib.h>
#include <cairo-xlib.h>

#include <unordered_map>

namespace cycfi { namespace elements
{
   struct host_view
   {
      host_view();
      ~host_view();

      Window native;
      cairo_surface_t* surface = nullptr;
   };

   host_view::host_view()
   {
   }

   host_view::~host_view()
   {
      if (surface)
         cairo_surface_destroy(surface);

      surface = nullptr;
   }

   Window make_view(base_view& view, Window parent)
   {
      Display* display = XOpenDisplay(nullptr);
      int screen = DefaultScreen(display);
      XWindowAttributes attributes;

      XGetWindowAttributes(display, parent, &attributes);

      Window content_view = XCreateSimpleWindow
      (
         display,
         parent,
         0,
         0,
         attributes.width,
         attributes.height,
         attributes.border_width,
         XWhitePixel(display, screen),
         XWhitePixel(display, screen)
      );

      extern std::unordered_map<Window, base_view*> gs_windows_map;
      gs_windows_map[content_view] = &view;

      // TODO: to remove, just for testing
      XColor bgcolor;
      Colormap cmap = DefaultColormap(display, screen);
      bgcolor.red = 65000;
      bgcolor.green = 0;
      bgcolor.blue = 65000;

      if (XAllocColor(display, cmap, &bgcolor))
         XSetWindowBackground(display, content_view, bgcolor.pixel);

      XSelectInput
      (
         display,
         content_view,
         ExposureMask
         | ButtonPressMask
         | ButtonReleaseMask
         | EnterWindowMask
         | LeaveWindowMask
         | PointerMotionMask
         | FocusChangeMask
         | KeyPressMask
         | KeyReleaseMask
         | SubstructureNotifyMask
      );

      XMapWindow(display, content_view);

      XFlush(display);

      return content_view;
   }

   struct platform_access
   {
      inline static host_view* get_host_view(base_view& view)
      {
         return view.host();
      }
   };

   void on_draw(base_view *view)
   {
      auto* host_view_h = view->host();
      Display* display = XOpenDisplay(nullptr);
      XWindowAttributes attributes;
      XGetWindowAttributes(display, host_view_h->native, &attributes);

      host_view_h->surface = cairo_xlib_surface_create(
         display,
         host_view_h->native,
         attributes.visual,
         attributes.width,
         attributes.height
      );

      cairo_xlib_surface_set_size(host_view_h->surface, attributes.width, attributes.height);
      cairo_t* context = cairo_create(host_view_h->surface);

      double left, top, right, bottom;
      cairo_clip_extents(context, &left, &top, &right, &bottom);
      view->draw(
         context,
         rect{ float(left), float(top), float(right), float(bottom) }
      );
      cairo_destroy(context);
      cairo_surface_destroy(host_view_h->surface);
   }

   // Defined in window.cpp
   Window get_window(host_window& h);

   struct init_view_class
   {
      init_view_class()
      {
         auto pwd = fs::current_path();
         auto resource_path = pwd / "resources";
         add_search_path(resource_path);
      }
   };

    base_view::base_view(extent /* size_ */)
    : base_view(new host_view)
    {
        // $$$ FIXME: Implement Me $$$
        CYCFI_ASSERT(false, "Unimplemented");
    }

   base_view::base_view(host_view_handle h)
    : _view(h)
   {
      static init_view_class init;
   }

   base_view::base_view(host_window_handle h)
    : base_view(new host_view)
   {
       _view->native = elements::make_view(*this, get_window(*h));
   }

   base_view::~base_view()
   {
      delete _view;
   }

   point base_view::cursor_pos() const
   {
      return point(); // TODO
   }

   elements::extent base_view::size() const
   {
       return {0.0, 0.0}; // TODO
   }

   void base_view::size(elements::extent p)
   {
      (void)p; // TODO
   }

   float base_view::hdpi_scale() const
   {
      return 1.0f;
   }

   void base_view::refresh()
   {
      // TODO
   }

   void base_view::refresh(rect area)
   {
      (void)area; // TODO
   }

   std::string clipboard()
   {
      return std::string(); // TODO
   }

   void clipboard(std::string const& text)
   {
      (void)text; // TODO
   }

   void set_cursor(cursor_type type)
   {
      (void)type; // TODO
   }

   point scroll_direction()
   {
      return { +1.0f, +1.0f };
   }
}}
