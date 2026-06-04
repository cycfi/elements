/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_X11_HOST_HPP)
#define ELEMENTS_X11_HOST_HPP

#include <X11/Xlib.h>
#include <functional>

// The X11 host's shared platform layer lives in base_view.cpp (the
// self-contained core, mirroring the Windows/macOS hosts). app.cpp and
// window.cpp are thin conveniences on top; nothing here depends on either, so
// the core works in the windowless/plugin case where neither exists.

namespace cycfi::elements
{
   struct host_window;

   // Shared display + platform, opened lazily on first use (no app required).
   Display* get_display();
   Atom     get_wm_delete_atom();
   XIM      get_xim();
   double   display_scale();                  // global Xft.dpi/96 (fallback)
   double   monitor_scale(int root_x, int root_y);  // per-monitor (XRandR)
   double   window_scale(::Window w);         // scale of monitor under window center

   // The X11 Window backing a host_window (defined in window.cpp).
   ::Window get_window(host_window& h);

   // Window-close registry: window.cpp registers a callback fired on WM_DELETE.
   void register_close(::Window w, std::function<void()> on_close);
   void unregister_close(::Window w);

   // Event-pump entry points — driven by app::run() in standalone mode, or by
   // an external host loop (e.g. a plugin) in the windowless case.
   void dispatch_event(XEvent& ev);   // route one X event
   void poll_views();                 // drive each view's deferred io_context
}

#endif
