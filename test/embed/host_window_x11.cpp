/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include "host_window.hpp"
#include <X11/Xlib.h>
#include <chrono>
#include <cstdint>
#include <thread>

namespace embed_test
{
   namespace
   {
      // The host's own connection. Each library opens its own, and
      // delivers its events itself, through embed_pump.
      Display* display()
      {
         static Display* d = XOpenDisplay(nullptr);
         return d;
      }
   }

   window open_window(int width, int height)
   {
      auto d = display();
      if (!d)
         return {};
      auto win = XCreateSimpleWindow(d, DefaultRootWindow(d)
       , 0, 0, width, height, 0, 0, 0);
      XMapWindow(d, win);
      XSync(d, false);
      return {reinterpret_cast<void*>(static_cast<std::uintptr_t>(win))};
   }

   void close_window(window const& w)
   {
      if (auto d = display())
      {
         XDestroyWindow(d, static_cast<::Window>(
            reinterpret_cast<std::uintptr_t>(w.handle)));
         XSync(d, false);
      }
   }

   void run_events(int ms)
   {
      using clock = std::chrono::steady_clock;
      auto const end = clock::now() + std::chrono::milliseconds(ms);
      auto d = display();
      while (clock::now() < end)
      {
         while (d && XPending(d))
         {
            XEvent ev;
            XNextEvent(d, &ev);
         }
         std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }
   }
}
