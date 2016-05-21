/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/basic.hpp>
#include <photon/widget/drawing.hpp>

namespace photon
{
   float   panel::corner_radius  = 3.0;
   rect    panel::shadow_offset  = { -10, -10, +20, +30 };

   void panel::draw(context const& ctx)
   {
      // Panel fill
      round_rect(ctx.canvas(), ctx.bounds, corner_radius, ctx.theme().panel_color);

      // Drop shadow
      drop_shadow(ctx.canvas(), ctx.bounds, corner_radius, shadow_offset);
   }

   void frame::draw(context const& ctx)
   {
      ctx.theme().draw_frame(ctx.bounds);
   }

   void title_bar::draw(context const& ctx)
   {
      ctx.theme().draw_title_bar(ctx.bounds);
   }
}
