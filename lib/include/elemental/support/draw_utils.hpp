/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_ELEMENTAL_GUI_LIB_DRAW_UTILS_OCTOBER_27_2017)
#define CYCFI_ELEMENTAL_GUI_LIB_DRAW_UTILS_OCTOBER_27_2017

#include <elemental/support/canvas.hpp>

namespace cycfi { namespace elemental
{
   void draw_box_vgradient(canvas& cnv, rect bounds, float corner_radius = 4.0);
   void draw_panel(canvas& cnv, rect bounds, color c, float corner_radius = 4.0);
   void draw_button(canvas& cnv, rect bounds, color c, float corner_radius = 4.0);
   void draw_knob(canvas& cnv, circle cp, color c);
   void draw_indicator(canvas& cnv, rect bounds, color c);
   void draw_thumb(canvas& cnv, circle cp, color c, color ic);
   void draw_track(canvas& cnv, rect bounds);
}}

#endif
