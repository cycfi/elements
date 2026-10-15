/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   The user's scroll direction on Linux, shared by the GTK, X11 and Wayland
   hosts: +1 on both axes for natural scrolling, -1 for traditional. Read
   from GNOME's touchpad setting through gsettings, -1 where it cannot be
   read, and read again at most every 10 seconds, in case the user changes
   it.
=============================================================================*/
#if !defined(ELEMENTS_HOST_SCROLL_DIRECTION_OCTOBER_15_2026)
#define ELEMENTS_HOST_SCROLL_DIRECTION_OCTOBER_15_2026

#include <elements/base_view.hpp>
#include <array>
#include <chrono>
#include <cstdio>
#include <memory>
#include <string>

namespace cycfi::elements::linux_host
{
   inline point read_scroll_direction()
   {
      std::string output;
      std::unique_ptr<FILE, decltype(&pclose)> pipe(
         popen("gsettings get org.gnome.desktop.peripherals.touchpad natural-scroll 2>/dev/null", "r"),
         pclose);
      if (pipe)
      {
         std::array<char, 128> buffer;
         while (fgets(buffer.data(), int(buffer.size()), pipe.get()))
            output += buffer.data();
      }
      if (output.rfind("true", 0) == 0)
         return {+1.0f, +1.0f};
      return {-1.0f, -1.0f};
   }

   inline point scroll_direction()
   {
      using namespace std::chrono;
      static auto last_call = steady_clock::now();
      static point dir = read_scroll_direction();

      auto const now = steady_clock::now();
      if (now - last_call >= seconds(10))
      {
         dir = read_scroll_direction();
         last_call = now;
      }
      return dir;
   }
}

#endif
