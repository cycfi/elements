/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/panel.hpp>
#include <photon/drawing.hpp>
#include <photon/support.hpp>
#include <photon/app.hpp>
#include <photon/window.hpp>

namespace photon
{
   void panel_widget::draw(layout_info const& l)
   {
      draw_panel(l.window.context(), l.bounds, l.app.theme);
   }
}
