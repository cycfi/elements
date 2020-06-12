/*=============================================================================
   Copyright (c) 2020 Michał Urbański

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#if !defined(ELEMENTS_WINDOWS_UTILS_MAY_07_2020)
#define ELEMENTS_WINDOWS_UTILS_MAY_07_2020

#include <Windows.h>

namespace cycfi::elements {

   inline float get_scale_for_window(HWND hwnd)
   {
      #ifdef ELEMENTS_HOST_ONLY_WIN7
      (void) hwnd;
      return 1.0f;
      #else
      return GetDpiForWindow(hwnd) / 96.0f;
      #endif
   }

}

#endif
