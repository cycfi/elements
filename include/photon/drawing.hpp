/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_DRAWING_APRIL_15_2016)
#define PHOTON_GUI_LIB_DRAWING_APRIL_15_2016

#include <photon/rect.hpp>
#include <photon/color.hpp>

// forward
struct NVGcontext;

namespace photon
{
   struct theme
   {
      // Panels
      double   panel_corner_radius  = 3.0f;
      color    panel_color          = { 28, 30, 34, 192 };
      rect     panel_shadow_offset  = { -10, -10, +20, +30 };
   };

   void draw_panel(NVGcontext* vg, rect const& b, theme const& t);
}

#endif
