/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/panel.hpp>
#include <photon/draw_utils.hpp>
#include <photon/support.hpp>
#include <photon/app.hpp>
#include <photon/window.hpp>

namespace photon
{
   void panel::draw(layout_info const& l)
   {
      auto const& theme = l.app.theme;
      auto ctx = l.window.context();

      draw::round_rect_fill(ctx, l.bounds, theme.panel_color, theme.panel_corner_radius);
      draw::round_rect_shadow(ctx, l.bounds, theme.shadow_color, theme.panel_corner_radius);
   }
}
