/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/slider.hpp>
#include <photon/drawing.hpp>
#include <photon/support.hpp>
#include <photon/app.hpp>
#include <photon/window.hpp>

namespace photon
{
   rect slider_widget::limits() const
   {
      return { 16, 16, full_extent, full_extent };
   }

   void slider_widget::draw(layout_info const& l)
   {
      //pos = 0;
      l.app.theme()->draw_slider(l.window.context(), pos, l.bounds);
   }
}
