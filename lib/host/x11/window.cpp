/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements/window.hpp>
#include <elements/support.hpp>
#include "x11_host.hpp"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <cmath>

namespace cycfi::elements
{
   struct host_window
   {
      ::Window window = 0;
   };

   ::Window get_window(host_window& h) { return h.window; }

   window::window(std::string const& name, int /*style_*/, rect const& bounds)
    : _window(new host_window)
   {
      Display* d = get_display();
      int screen = DefaultScreen(d);
      ::Window root = RootWindow(d, screen);
      double scale = display_scale();

      int x = int(std::lround(bounds.left * scale));
      int y = int(std::lround(bounds.top * scale));
      int w = int(std::lround(bounds.width() * scale));
      int h = int(std::lround(bounds.height() * scale));

      _window->window = XCreateSimpleWindow(
         d, root, x, y, w, h, 0,
         BlackPixel(d, screen), WhitePixel(d, screen)
      );

      XStoreName(d, _window->window, name.c_str());

      // Window-close protocol → window::on_close
      Atom wm_delete = get_wm_delete_atom();
      XSetWMProtocols(d, _window->window, &wm_delete, 1);
      register_close(_window->window, [this]() { if (on_close) on_close(); });

      XMapWindow(d, _window->window);
   }

   window::~window()
   {
      Display* d = get_display();
      unregister_close(_window->window);
      XDestroyWindow(d, _window->window);
      delete _window;
   }

   point window::size() const
   {
      Display* d = get_display();
      XWindowAttributes attr;
      XGetWindowAttributes(d, _window->window, &attr);
      double scale = display_scale();
      return {float(attr.width / scale), float(attr.height / scale)};
   }

   void window::size(point const& p)
   {
      Display* d = get_display();
      double scale = display_scale();
      XResizeWindow(d, _window->window,
         (unsigned)std::lround(p.x * scale), (unsigned)std::lround(p.y * scale));
   }

   void window::limits(view_limits limits_)
   {
      Display* d = get_display();
      double scale = display_scale();
      XSizeHints* hints = XAllocSizeHints();
      if (!hints)
         return;
      constexpr float max = 10E6;
      hints->flags      = PMinSize | PMaxSize;
      hints->min_width  = int(std::lround(limits_.min.x * scale));
      hints->min_height = int(std::lround(limits_.min.y * scale));
      hints->max_width  = int(std::lround(std::min<float>(limits_.max.x, max) * scale));
      hints->max_height = int(std::lround(std::min<float>(limits_.max.y, max) * scale));
      XSetWMNormalHints(d, _window->window, hints);
      XFree(hints);
   }

   point window::position() const
   {
      Display* d = get_display();
      ::Window child;
      int x = 0, y = 0;
      ::Window root = DefaultRootWindow(d);
      XTranslateCoordinates(d, _window->window, root, 0, 0, &x, &y, &child);
      double scale = display_scale();
      return {float(x / scale), float(y / scale)};
   }

   void window::position(point const& p)
   {
      Display* d = get_display();
      double scale = display_scale();
      XMoveWindow(d, _window->window,
         int(std::lround(p.x * scale)), int(std::lround(p.y * scale)));
   }
}
