/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_DRAW_UTILS_APRIL_11_2016)
#define PHOTON_GUI_LIB_WIDGET_DRAW_UTILS_APRIL_11_2016

#include <photon/rect.hpp>
#include <photon/color.hpp>
#include "nanovg.h"

namespace photon { namespace draw
{
   inline NVGcolor nvgRGBA(color const& c)
   {
      NVGcolor color;
      color.r = c.red;
      color.g = c.green;
      color.b = c.blue;
      color.a = c.alpha;
      return color;
   }

   void round_rect_fill(NVGcontext* vg, rect const& r, color const& c, float corner_radius);
   void round_rect_shadow(NVGcontext* vg, rect const& r, color const& c, float corner_radius);
}}

#endif