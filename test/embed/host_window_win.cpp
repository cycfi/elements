/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include "host_window.hpp"
#include <windows.h>
#include <chrono>

namespace embed_test
{
   window open_window(int width, int height)
   {
      static bool registered = []
      {
         WNDCLASSW wc = {};
         wc.lpfnWndProc = DefWindowProcW;
         wc.hInstance = GetModuleHandleW(nullptr);
         wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
         wc.lpszClassName = L"ElementsEmbedTestHost";
         return RegisterClassW(&wc) != 0;
      }();
      (void) registered;

      RECT r{0, 0, width, height};
      AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, false);
      HWND hwnd = CreateWindowW(
         L"ElementsEmbedTestHost", L"embed_test", WS_OVERLAPPEDWINDOW,
         CW_USEDEFAULT, CW_USEDEFAULT, r.right - r.left, r.bottom - r.top,
         nullptr, nullptr, GetModuleHandleW(nullptr), nullptr
      );
      ShowWindow(hwnd, SW_SHOWNOACTIVATE);
      return {hwnd};
   }

   void close_window(window const& w)
   {
      DestroyWindow(static_cast<HWND>(w.handle));
   }

   void run_events(int ms)
   {
      using clock = std::chrono::steady_clock;
      auto const end = clock::now() + std::chrono::milliseconds(ms);
      while (clock::now() < end)
      {
         MSG msg;
         while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
         {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
         }
         Sleep(10);
      }
   }
}
