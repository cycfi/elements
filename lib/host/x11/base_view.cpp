/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements/app.hpp>
#include <infra/assert.hpp>
#include <elements/base_view.hpp>
#include <elements/window.hpp>
#include <elements/support/resource_paths.hpp>

#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include <cairo.h>
#include <cairo-xlib.h>

#include <iostream>
#include <unordered_map>

namespace cycfi { namespace elements
{
   Display* get_display();

   struct host_view
   {
      host_view(base_view& view, Window parent);
      ~host_view();

      Window x_window;
      cairo_surface_t* surface = nullptr;
      cairo_t* context = nullptr;
      unsigned int last_cursor_shape = XC_X_cursor;
   };

   namespace {
      // set_cursor is not provided with a base_view context, so we must keep
      // track of the one the user is pointing to
      host_view *host_view_under_cursor = nullptr;
   }

   host_view::host_view(base_view& view, Window parent)
   {
      Display* display = get_display();
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
         0, // border_width
         XWhitePixel(display, screen),
         XWhitePixel(display, screen)
      );
      x_window = content_view;


      // TODO StructureNotify on parent


      extern std::unordered_map<Window, base_view*> gs_windows_map;
      gs_windows_map[content_view] = &view;

      /* Debug: paint window background magenta
      XColor bgcolor;
      Colormap cmap = DefaultColormap(display, screen);
      bgcolor.red = 65000;
      bgcolor.green = 0;
      bgcolor.blue = 65000;

      if (XAllocColor(display, cmap, &bgcolor))
         XSetWindowBackground(display, content_view, bgcolor.pixel);
      */

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

      surface = cairo_xlib_surface_create(
         display,
         content_view,
         attributes.visual,
         attributes.width,
         attributes.height
      );

      context = cairo_create(surface);
   }

   host_view::~host_view()
   {
      cairo_destroy(context);
      cairo_surface_destroy(surface);
      if (host_view_under_cursor == this)
         host_view_under_cursor = nullptr;
   }

   static void on_draw(base_view *view, rect area)
   {
      view->draw(view->host()->context, area);
      XFlush(get_display());
   }

   static void on_draw(base_view *view, const XExposeEvent& event)
   {
      on_draw(view, rect{float(event.x), float(event.y),
                         float(event.x + event.width), float(event.y + event.height)});
   }

   static void on_draw(base_view *view)
   {
      auto* host_view_h = view->host();
      Display* display = get_display();
      XWindowAttributes attributes;
      XGetWindowAttributes(display, host_view_h->x_window, &attributes);

      // TODO move to resize handler
      cairo_xlib_surface_set_size(host_view_h->surface, attributes.width, attributes.height);

      double left, top, right, bottom;
      cairo_clip_extents(host_view_h->context, &left, &top, &right, &bottom);

      /* Debug: paint background red
      cairo_set_source_rgb(host_view_h->context, 255, 0, 0);
      cairo_paint(host_view_h->context);
      */

      on_draw(view, rect{ float(left), float(top), float(right), float(bottom) });
   }

   template<typename XEvent>
   void get_mouse(const XEvent& event, mouse_button& btn)
   {
      btn.pos = { float(event.x), float(event.y) };
      btn.modifiers = 0; // TODO
   }

   static void on_button(base_view *view, const XButtonEvent& event)
   {
      mouse_button btn = {};
      get_mouse(event, btn);
      btn.state = (event.button == Button1) ? mouse_button::left
                : (event.button == Button2) ? mouse_button::middle
                :                             mouse_button::right;
      btn.num_clicks = 1; // TODO count double click like GTK
      btn.down = event.type == ButtonPress;

      view->click(btn);
   }

   static void on_pointer(base_view *view, const XPointerMovedEvent& event)
   {
      mouse_button btn = {};
      get_mouse(event, btn);
      btn.state = (event.state & Button1Mask) ? mouse_button::left
                : (event.state & Button2Mask) ? mouse_button::middle
                :                               mouse_button::right;
      btn.down = (event.state & (Button1Mask | Button2Mask | Button3Mask)) != 0;
      if(btn.down) {
         view->drag(btn);
      } else {
         view->cursor(btn.pos, cursor_tracking::hovering);
      }
   }

   static void on_crossing(base_view *view, const XCrossingEvent& event)
   {
      point pos { float(event.x), float(event.y) };
      if(event.type == EnterNotify) {
         host_view_under_cursor = view->host();
         view->cursor(pos, cursor_tracking::entering);
      } else {
         host_view_under_cursor = nullptr;
         view->cursor(pos, cursor_tracking::entering);
      }
   }

   static void on_key(base_view *view, const XKeyEvent& event)
   {
   }

   bool on_event(base_view *view, const XEvent& event)
   {
      switch(event.type)
      {
         case Expose:
            on_draw(view, event.xexpose);
            break;
         case MotionNotify:
            on_pointer(view, event.xmotion);
            break;
         case ButtonPress:
         case ButtonRelease:
            on_button(view, event.xbutton);
            break;
         case EnterNotify:
         case LeaveNotify:
            on_crossing(view, event.xcrossing);
            break;
         case FocusIn:
            view->begin_focus();
            break;
         case FocusOut:
            view->end_focus();
            break;
         case KeyPress:
         case KeyRelease:
            on_key(view, event.xkey);
            break;
         case ClientMessage:
         {
            Atom wm_delete_window = XInternAtom(get_display(), "WM_DELETE_WINDOW", True);
            if (static_cast<Atom>(event.xclient.data.l[0]) == wm_delete_window) {
               return false;
            }
            break;
         }
      }
      return true;
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
    : base_view(new host_view(*this, 0))
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
    : base_view(new host_view(*this, get_window(*h)))
   {
   }

   base_view::~base_view()
   {
      delete _view;
   }

   point base_view::cursor_pos() const
   {
      Window root, child;
      int root_x, root_y;
      int child_x, child_y;
      unsigned int mask;

      XQueryPointer(get_display(), _view->x_window,
                    &root, &child, &root_x, &root_y, &child_x, &child_y, &mask);
      return {float(child_x), float(child_y)};
   }

   elements::extent base_view::size() const
   {
       Window root;
       int x, y;
       unsigned int w = 0, h = 0, border_w, depth;
       XGetGeometry(get_display(), _view->x_window , &root, &x, &y, &w, &h, &border_w, &depth);
       return {float(w), float(h)};
   }

   void base_view::size(elements::extent p)
   {
       std::cerr << "base_view::size(extent)" << std::endl;
       (void)p; // TODO
   }

   float base_view::hdpi_scale() const
   {
      return 1.0f;
   }

   void base_view::refresh()
   {
      on_draw(this);
   }

   void base_view::refresh(rect area)
   {
      on_draw(this, area);
   }

   std::string clipboard()
   {
      std::cerr << "base_view::clipboard()" << std::endl;
      return std::string(); // TODO
   }

   void clipboard(std::string const& text)
   {
      std::cerr << "base_view::clipboard(std::string const&)" << std::endl;
      (void)text; // TODO
   }

   void set_cursor(cursor_type type)
   {
      unsigned int new_shape;
      switch (type)
      {
         case cursor_type::ibeam:
            new_shape = XC_xterm;
            break;
         case cursor_type::cross_hair:
            new_shape = XC_crosshair;
            break;
         case cursor_type::hand:
            new_shape = XC_hand2;
            break;
         case cursor_type::h_resize:
            new_shape = XC_sb_h_double_arrow;
            break;
         case cursor_type::v_resize:
            new_shape = XC_sb_v_double_arrow;
            break;
         default:
         case cursor_type::arrow:
            new_shape = XC_left_ptr;
            break;
      }
      if(host_view_under_cursor != nullptr && host_view_under_cursor->last_cursor_shape != new_shape) {
         Display* display = get_display();
         Cursor c = XCreateFontCursor(display, new_shape);
         XDefineCursor(display, host_view_under_cursor->x_window, c);
         host_view_under_cursor->last_cursor_shape = new_shape;
      }
   }

   point scroll_direction()
   {
      return { +1.0f, +1.0f };
   }
}}
