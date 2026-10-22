/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements/window.hpp>
#include <elements/support.hpp>
#include "x11_host.hpp"

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <algorithm>
#include <cmath>

namespace cycfi::elements
{
   struct host_window
   {
      ::Window              window = 0;
      view_limits           limits = {{0, 0}, {10E6, 10E6}};
   };

   ::Window get_window(host_window& h) { return h.window; }

   window::window(std::string const& name, int /*style_*/, rect const& bounds)
    : _window(new host_window)
   {
      Display* d = get_display();
      int screen = DefaultScreen(d);
      ::Window root = RootWindow(d, screen);
      // Scale of the monitor the window will open on (window not created yet).
      double scale = monitor_scale(int(std::lround(bounds.left)),
                                   int(std::lround(bounds.top)));

      int x = int(std::lround(bounds.left * scale));
      int y = int(std::lround(bounds.top * scale));
      int w = int(std::lround(bounds.width() * scale));
      int h = int(std::lround(bounds.height() * scale));

      _window->window = XCreateSimpleWindow(
         d, root, x, y, w, h, 0,
         BlackPixel(d, screen), WhitePixel(d, screen)
      );

      // Don't let X paint the window background: the backing redraw already
      // fills the whole window, and an X background clear on each resize step
      // shows up as flicker (a white flash) behind the new frame.
      XSetWindowBackgroundPixmap(d, _window->window, None);

      // The bounds' position is meant: without this, a window manager
      // places a new window where it chooses.
      if (XSizeHints* hints = XAllocSizeHints())
      {
         hints->flags = PPosition | PSize;
         hints->x = x;
         hints->y = y;
         hints->width = w;
         hints->height = h;
         XSetWMNormalHints(d, _window->window, hints);
         XFree(hints);
      }

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
      double scale = window_scale(_window->window);
      return {float(attr.width / scale), float(attr.height / scale)};
   }

   void window::size(point const& p)
   {
      // Clamped here as well as by the size hints: without a window
      // manager, nothing else applies them.
      auto const& l = _window->limits;
      float const w = std::clamp(p.x, l.min.x, std::max(l.min.x, l.max.x));
      float const h = std::clamp(p.y, l.min.y, std::max(l.min.y, l.max.y));
      Display* d = get_display();
      double scale = window_scale(_window->window);
      XResizeWindow(d, _window->window,
         (unsigned)std::lround(w * scale), (unsigned)std::lround(h * scale));
   }

   void window::limits(view_limits limits_)
   {
      _window->limits = limits_;
      Display* d = get_display();
      double scale = window_scale(_window->window);
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

   // The proportion is not kept here: a fit leaves the space over empty.
   void window::aspect_ratio(float /*ratio*/)
   {
   }

   namespace
   {
      // The frame the window manager put around the window, as
      // _NET_FRAME_EXTENTS gives it: left, right, top, bottom; zero
      // without one.
      void frame_extents(Display* d, ::Window w, long& left, long& top)
      {
         left = top = 0;
         Atom prop = XInternAtom(d, "_NET_FRAME_EXTENTS", True);
         if (prop == None)
            return;
         Atom type;
         int format = 0;
         unsigned long n = 0, after = 0;
         unsigned char* data = nullptr;
         if (XGetWindowProperty(d, w, prop, 0, 4, False, XA_CARDINAL
            , &type, &format, &n, &after, &data) == Success && data)
         {
            if (type == XA_CARDINAL && format == 32 && n >= 4)
            {
               auto const* v = reinterpret_cast<long const*>(data);
               left = v[0];
               top = v[2];
            }
            XFree(data);
         }
      }
   }

   // The top left of the window as the user sees it: the frame's, when
   // the window manager gave it one, as position(p) places it.
   point window::position() const
   {
      Display* d = get_display();
      ::Window child;
      int x = 0, y = 0;
      ::Window root = DefaultRootWindow(d);
      XTranslateCoordinates(d, _window->window, root, 0, 0, &x, &y, &child);
      long left = 0, top = 0;
      frame_extents(d, _window->window, left, top);
      double scale = window_scale(_window->window);
      return {float((x - left) / scale), float((y - top) / scale)};
   }

   void window::position(point const& p)
   {
      Display* d = get_display();
      double scale = window_scale(_window->window);
      XMoveWindow(d, _window->window,
         int(std::lround(p.x * scale)), int(std::lround(p.y * scale)));
   }
}
