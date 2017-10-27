/*=============================================================================
   Copyright (c) 2016-2017 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(PHOTON_GUI_LIB_DRAW_UTILS_OCTOBER_27_2017)
#define PHOTON_GUI_LIB_DRAW_UTILS_OCTOBER_27_2017

#include <photon/support/canvas.hpp>

namespace photon
{
   void draw_box_vgradient(canvas& cnv, rect bounds, float corner_radius = 4.0);
   void draw_panel(canvas& cnv, rect bounds, color c, float corner_radius = 4.0);
   void draw_button(canvas& cnv, rect bounds, color c, float corner_radius = 4.0);
   void draw_knob(canvas& cnv, circle cp, color c);
   void draw_indicator(canvas& cnv, rect bounds, color c);
   void draw_indicator(canvas& cnv, circle cp, float val, color c);
}

#endif
