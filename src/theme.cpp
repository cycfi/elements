/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/theme.hpp>

namespace photon
{
   void theme::load_fonts()
   {
      _canvas.new_font("icons", "./assets/fonts/photon_basic.ttf");
      _canvas.new_font("sans", "./assets/fonts/Roboto-Regular.ttf");
      _canvas.new_font("sans-bold", "./assets/fonts/Roboto-Bold.ttf");
   }
}
