/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/app.hpp>
#include "x11_host.hpp"

#include <X11/Xlib.h>
#include <sys/select.h>

// Thin standalone event pump. All per-event handling and shared state live in
// base_view.cpp (the self-contained core), so this file knows nothing about
// views or windows — mirroring the Windows host's GetMessage loop. In the
// windowless/plugin case this file is not used at all; the host drives the loop
// and calls dispatch_event()/poll_views() directly.

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
      Display* d = get_display();
      int fd = ConnectionNumber(d);

      while (_running)
      {
         while (XPending(d))
         {
            XEvent ev;
            XNextEvent(d, &ev);
            dispatch_event(ev);
         }

         // Drive each view's asio io_context (deferred refresh, animation).
         poll_views();

         // Block up to ~1ms for the next event so polling stays at ~1kHz.
         fd_set rd;
         FD_ZERO(&rd);
         FD_SET(fd, &rd);
         timeval tv;
         tv.tv_sec = 0;
         tv.tv_usec = 1000;
         select(fd + 1, &rd, nullptr, nullptr, &tv);
      }
   }

   void app::stop()
   {
      _running = false;
   }
}
