/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_DRAWING_MAY_21_2016)
#define PHOTON_GUI_LIB_WIDGET_DRAWING_MAY_21_2016

#include <photon/canvas.hpp>

namespace photon
{
   void round_rect(
      canvas& _canvas,
      rect bounds, float radius,
      color color_
   );

   void drop_shadow(
      canvas& _canvas,
      rect bounds, float radius, rect shadow_offset
   );
}

#endif