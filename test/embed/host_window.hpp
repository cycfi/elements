/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
// The host's side: a native top-level window for a library to embed into,
// and the loop that delivers its events. One implementation per platform:
// host_window_win.cpp, host_window_mac.mm, host_window_x11.cpp.
#if !defined(ELEMENTS_EMBED_TEST_HOST_WINDOW_HPP_OCTOBER_9_2026)
#define ELEMENTS_EMBED_TEST_HOST_WINDOW_HPP_OCTOBER_9_2026

namespace embed_test
{
   struct window
   {
      // An HWND, an NSView (the content view) or an X11 Window.
      void* handle = nullptr;
   };

   window   open_window(int width, int height);
   void     close_window(window const& w);

   // Delivers the window system's events for about the given time.
   void     run_events(int ms);
}

#endif
