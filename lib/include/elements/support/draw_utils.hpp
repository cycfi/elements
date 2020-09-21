/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_DRAW_UTILS_OCTOBER_27_2017)
#define ELEMENTS_DRAW_UTILS_OCTOBER_27_2017

#include <elements/support/canvas.hpp>

namespace cycfi::elements
{
   namespace radial_consts
   {
      constexpr double _2pi = 2 * M_PI;
      constexpr double travel = 0.83;
      constexpr double range = _2pi * travel;
      constexpr double start_angle = _2pi * (1 - travel) / 2;
      constexpr double offset = (2 * M_PI) * (1 - travel) / 2;
   }

   void  draw_box_vgradient(canvas& cnv, rect bounds, float corner_radius = 4.0);
   void  draw_panel(canvas& cnv, rect bounds, color c, float corner_radius = 4.0);
   void  draw_button(canvas& cnv, rect bounds, color c, float corner_radius = 4.0);
   void  draw_knob(canvas& cnv, circle cp, color c);
   void  draw_indicator(canvas& cnv, rect bounds, color c);
   void  draw_thumb(canvas& cnv, circle cp, color c, color ic);
   void  draw_track(canvas& cnv, rect bounds);

   void  draw_radial_indicator(canvas& cnv, circle cp, float val, color c);
   void  draw_radial_marks(canvas& cnv, circle cp, float size, color c);
   void  draw_radial_labels(
            canvas& cnv, circle cp, float font_size
          , std::string const labels[], std::size_t _num_labels
         );
}

#endif
