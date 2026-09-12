/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_WAYLAND_HOST_HPP)
#define ELEMENTS_WAYLAND_HOST_HPP

#include <wayland-client.h>
#include <functional>

// The Wayland host's shared platform layer lives in base_view.cpp (the
// self-contained core, mirroring the X11/Windows/macOS hosts). app.cpp and
// window.cpp are thin conveniences on top. Standalone-only (Wayland has no
// cross-process plugin embedding; plugins use the X11 host).

struct libdecor;

namespace cycfi::elements
{
   struct host_window;
   class base_view;

   // Shared Wayland connection + platform, opened lazily on first use.
   wl_display* get_display();
   libdecor*   get_libdecor();

   // Globals (bound from the registry; may be null if unsupported).
   wl_compositor* get_compositor();
   wl_shm*        get_shm();

   // Per-surface HiDPI (wp_fractional_scale_v1 + wp_viewport). Opaque here.
   void  attach_fractional_scale(host_window& w);

   // Link a window's surface to its base_view, and render hooks used by the
   // libdecor frame callbacks in window.cpp.
   void  bind_view(host_window& w, base_view* view);
   void  view_configure(host_window& w, int content_w, int content_h);
   void  view_render(host_window& w);

   // Event-pump entry points (app::run() or an external loop).
   void  dispatch_events();   // flush + dispatch pending Wayland events
   void  poll_views();        // drive each view's deferred io_context
}

#endif
