/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   The headless host's window and view: plain state, with no windowing
   system under them. Shared by its base_view.cpp and window.cpp.
=============================================================================*/
#if !defined(ELEMENTS_HEADLESS_HOST_SEPTEMBER_13_2026)
#define ELEMENTS_HEADLESS_HOST_SEPTEMBER_13_2026

#include <elements/base_view.hpp>
#include <vector>

namespace cycfi::elements
{
   struct host_window
   {
      rect              bounds;
      view_limits       limits;
      base_view*        view = nullptr;   // the view made on it, if any
   };

   struct host_view
   {
      extent            size;
      float             scale = 1;
      point             cursor_position;
      std::vector<rect> invalidated;      // refresh areas, in order asked
      host_window*      window = nullptr;
   };
}

#endif
