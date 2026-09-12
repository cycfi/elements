/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/app.hpp>
#include "wayland_host.hpp"

#include <wayland-client.h>
#include <poll.h>

// Thin standalone event pump. All shared state + per-event handling live in
// base_view.cpp (the self-contained core). Mirrors the X11/Windows hosts.

namespace cycfi::elements
{
   app::app(std::string name)
    : _app_name(std::move(name))
   {
      get_display();   // ensure the platform layer is initialised
   }

   app::~app()
   {
   }

   void app::run()
   {
      _running = true;
      wl_display* d = get_display();
      int fd = wl_display_get_fd(d);

      while (_running)
      {
         // Standard Wayland prepare-read/poll/read loop, with a 1ms timeout so
         // poll_views() (the asio io_context: deferred refresh, animation) runs
         // even when no Wayland events arrive.
         while (wl_display_prepare_read(d) != 0)
            wl_display_dispatch_pending(d);
         wl_display_flush(d);

         pollfd pfd { fd, POLLIN, 0 };
         int n = poll(&pfd, 1, 1);   // 1ms
         if (n > 0 && (pfd.revents & POLLIN))
            wl_display_read_events(d);
         else
            wl_display_cancel_read(d);

         wl_display_dispatch_pending(d);
         poll_views();
      }
   }

   void app::stop()
   {
      _running = false;
   }
}
