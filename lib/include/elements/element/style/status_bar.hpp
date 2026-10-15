/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_STYLE_STATUS_BAR_OCTOBER_14_2026)
#define ELEMENTS_STYLE_STATUS_BAR_OCTOBER_14_2026

#include <elements/element/status_bar.hpp>
#include <elements/element/style/track.hpp>

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Stock status bars: a square track, filled by a square fill track.
   // `size` is the thickness; a vertical bar fills from the bottom.
   ////////////////////////////////////////////////////////////////////////////
   template <unsigned size = 6, bool vertical = false>
   inline auto progress_bar(double init_value = 0.0)
   {
      return progress_bar(
         basic_track<size, vertical, track_shape::square>()
       , basic_fill_track<size, vertical, track_shape::square>()
       , init_value
      );
   }

   template <unsigned size = 6, bool vertical = false>
   inline auto busy_bar(double init_value = 0.0, double start_pos = 0.0)
   {
      return busy_bar(
         basic_track<size, vertical, track_shape::square>()
       , basic_fill_track<size, vertical, track_shape::square>()
       , init_value
       , start_pos
      );
   }

   // A VU meter: a progress bar in green, then yellow above 0.7, then red
   // above 0.9 of the bar.
   template <unsigned size = 6, bool vertical = false>
   inline auto vu_meter(double init_value = 0.0)
   {
      return progress_bar(
         basic_track<size, vertical, track_shape::square>()
       , basic_fill_track<size, vertical, track_shape::square>().fill_colors({
            {0.7f, rgba(46, 204, 64, 255)}
          , {0.9f, rgba(255, 210, 0, 255)}
          , {1.0f, rgba(230, 40, 40, 255)}
         })
       , init_value
      );
   }
}

#endif
