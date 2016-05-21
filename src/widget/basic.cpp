/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/basic.hpp>

namespace photon
{
   float   panel::corner_radius  = 3.0;
   rect    panel::shadow_offset  = { -10, -10, +20, +30 };

   void panel::draw(context const& ctx)
   {
      auto& canvas_ = ctx.canvas();
      auto const& bounds = ctx.bounds;

      // Panel fill
      canvas_.begin_path();
      canvas_.round_rect(ctx.bounds, corner_radius);
      canvas_.fill_color(ctx.theme().panel_color);
      canvas_.fill();

      // Drop shadow
      rect offs = shadow_offset;
      canvas_.begin_path();
      rect  shr =
         {  bounds.left+offs.left, bounds.top+offs.top,
            bounds.right+offs.right, bounds.bottom+offs.bottom
         };

      canvas_.rect(shr);
      canvas_.round_rect(bounds, corner_radius);
      canvas_.path_winding(canvas::hole);

      paint shadow_paint
         = canvas_.box_gradient(bounds.move(0, 2), corner_radius*2, 10
          , color(0, 0, 0, 128), color(0, 0, 0, 0)
         );

      canvas_.fill_paint(shadow_paint);
      canvas_.fill();
   }

   float   frame::corner_radius  = 3.0;
   float   frame::stroke_width = 0.8;

   void frame::draw(context const& ctx)
   {
      auto& canvas_ = ctx.canvas();
      auto const& bounds = ctx.bounds;

      canvas_.begin_path();
      canvas_.round_rect(bounds, corner_radius);
      canvas_.stroke_color(ctx.theme().frame_color);
      canvas_.stroke_width(stroke_width);
      canvas_.stroke();
   }

   float   title_bar::corner_radius  = 3.0;

   void title_bar::draw(context const& ctx)
   {
      auto&       canvas_ = ctx.canvas();
      auto const& bounds = ctx.bounds;

      auto paint_ = canvas_.linear_gradient(
          point{ bounds.left, bounds.top },
          point{ bounds.left, bounds.bottom },
          color{ 255, 255, 255, 16 },
          color{ 0, 0, 0, 16 }
      );

      canvas_.begin_path();
      canvas_.round_rect(bounds, corner_radius);
      canvas_.fill_paint(paint_);
      canvas_.fill();

      canvas_.begin_path();
      canvas_.move_to(point{ bounds.left+0.5f, bounds.bottom-0.5f });
      canvas_.line_to(point{ bounds.right-0.5f, bounds.bottom-0.5f });
      canvas_.stroke_color(color{ 0, 0, 0, 32 });
      canvas_.stroke_width(1);
      canvas_.stroke();
   }
}
