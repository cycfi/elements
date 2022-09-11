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
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <cairo.h>
#include <cairo-xlib.h>

#include <iostream>
#include <unordered_map>
#include <string>

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

      Time click_time = 0;
      uint32_t click_count = 0;

      using key_map = std::unordered_map<key_code, key_action>;
      key_map keys;
      XIC x_ic;
   };

   namespace {
      // set_cursor is not provided with a base_view context, so we must keep
      // track of the one the user is pointing to
      host_view *host_view_under_cursor = nullptr;

      // X11 does not detect double clicks; it lets the application (framework)
      // do it.  Both GTK and Qt default to 400ms for double click time, so
      // copy that behavior.
      static constexpr Time dbl_click_time = 400;
   }

   extern std::unordered_map<Window, base_view*> gs_windows_map;

   host_view::host_view(base_view& view, Window parent)
   {
      Display* display = get_display();
      XWindowAttributes attributes;

      XGetWindowAttributes(display, parent, &attributes);

      // directly reuse parent's XWindow since there's a 1:1 correspondence
      // between window and base_view
      x_window = parent;

      gs_windows_map[x_window] = &view;

      /* Debug: paint window background magenta
      XColor bgcolor;
      Colormap cmap = DefaultColormap(display, screen);
      bgcolor.red = 65000;
      bgcolor.green = 0;
      bgcolor.blue = 65000;

      if (XAllocColor(display, cmap, &bgcolor))
         XSetWindowBackground(display, x_window, bgcolor.pixel);
      */

      XSelectInput
      (
         display,
         x_window,
         ExposureMask
         | ButtonPressMask
         | ButtonReleaseMask
         | EnterWindowMask
         | LeaveWindowMask
         | PointerMotionMask
         | FocusChangeMask
         | KeyPressMask
         | KeyReleaseMask
      );

      XFlush(display);

      surface = cairo_xlib_surface_create(
         display,
         x_window,
         attributes.visual,
         attributes.width,
         attributes.height
      );

      context = cairo_create(surface);

      // Create an input context so X delivers user's typed text to us
      // (not just individual keystrokes)
      // ref https://tedyin.com/posts/a-brief-intro-to-linux-input-method-framework/
      XIM xim = XOpenIM(display, nullptr, nullptr, nullptr);
      x_ic = XCreateIC(xim,
                       XNInputStyle, XIMPreeditNothing | XIMStatusNothing,
                       XNClientWindow, x_window, NULL);
      XSetICFocus(x_ic);
   }

   host_view::~host_view()
   {
      cairo_destroy(context);
      cairo_surface_destroy(surface);
      if (host_view_under_cursor == this)
         host_view_under_cursor = nullptr;
      gs_windows_map.erase(x_window);
      XIM xim = XIMOfIC(x_ic);
      XDestroyIC(x_ic);
      XCloseIM(xim);
   }

   static void on_draw(base_view *view, rect area)
   {
      auto* host_view_h = view->host();
      Display* display = get_display();
      XWindowAttributes attributes;
      XGetWindowAttributes(display, host_view_h->x_window, &attributes);

      cairo_xlib_surface_set_size(host_view_h->surface, attributes.width, attributes.height);

      cairo_push_group_with_content(view->host()->context, CAIRO_CONTENT_COLOR);
      view->draw(view->host()->context, area);
      cairo_pop_group_to_source(view->host()->context);
      cairo_paint(view->host()->context);
      XFlush(display);
   }

   static void on_draw(base_view *view, const XExposeEvent& event)
   {
      on_draw(view, rect{float(event.x), float(event.y),
                         float(event.x + event.width), float(event.y + event.height)});
   }

   static void on_draw(base_view *view)
   {
      auto* host_view_h = view->host();

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

      auto& click_count = view->host()->click_count;
      if ((event.time - view->host()->click_time) < dbl_click_time)
         ++click_count;
      else
         click_count = 1;
      view->host()->click_time = event.time;

      btn.state = (event.button == Button1) ? mouse_button::left
                : (event.button == Button2) ? mouse_button::middle
                :                             mouse_button::right;
      btn.num_clicks = view->host()->click_count;
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

   static void handle_key(base_view* _view, key_info k)
   {
      bool repeated = false;
      auto& keys = _view->host()->keys;

      if (k.action == key_action::release
         && keys[k.key] == key_action::release)
         return;

      if (k.action == key_action::press
         && keys[k.key] == key_action::press)
         repeated = true;

      keys[k.key] = k.action;

      if (repeated)
         k.action = key_action::repeat;

      _view->key(k);
   }

   // Defined in key.cpp
   key_code translate_key(KeySym key);

   static void on_key(base_view *view, const XKeyEvent& event)
   {
      auto& state = event.state;
      int mods = 0;
      if (state & ShiftMask)
         mods |= mod_shift;
      if (state & ControlMask)
         mods |= mod_control | mod_action;
      if (state & Mod1Mask)
         mods |= mod_alt;
      if (state & Mod4Mask)
         mods |= mod_super;

      KeySym raw;
      bool has_keysym = false;
      if(event.type == KeyPress) {
         // detect text entry from the input context (only for KeyPress per Xlib docs)
         Status status;
         std::wstring s;
         size_t c = 4; // arbitrary initial size, should work for most inputs
         do {
            s.resize(c);
            c = XwcLookupString(view->host()->x_ic, const_cast<XKeyEvent*>(&event),
                                s.data(), s.size(),
                                &raw, &status);
         } while(status == XBufferOverflow);
         if(status == XLookupChars || status == XLookupBoth) {
            for(const auto& cp: s) {
               if (cp < 32 || (cp > 126 && cp < 160)) {
                  // skip control chars
                  continue;
               }

               // FIXME: wchar_t is different than uint32_t
               view->text({ static_cast<uint32_t>(cp), mods });
            }
         }
         if(status == XLookupKeySym || status == XLookupBoth) {
            has_keysym = true;
         }
      }
      else {
         raw = XLookupKeysym(const_cast<XKeyEvent*>(&event), 0);
         if(raw != NoSymbol) {
            has_keysym = true;
         }
      }

      if(has_keysym) {
         KeySym upper, lower;
         XConvertCase(raw, &lower, &upper);
         key_code key = translate_key(lower);

         auto const action = event.type == KeyPress ? key_action::press : key_action::release;
         handle_key(view, { key, action, mods });
      }
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
            // allow input method to filter keystrokes
            if (XFilterEvent(const_cast<XEvent*>(&event), None))
               break;

            on_key(view, event.xkey);
            break;
         case MappingNotify:
            if(event.xmapping.request == MappingKeyboard || event.xmapping.request == MappingModifier) {
               XRefreshKeyboardMapping(const_cast<XMappingEvent*>(&event.xmapping));
            }
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
