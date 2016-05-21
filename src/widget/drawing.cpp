/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/drawing.hpp>

namespace photon
{
   void round_rect(
      canvas& _canvas,
      rect bounds, float radius,
      color color_
   )
   {
      _canvas.begin_path();
      _canvas.round_rect(bounds, radius);
      _canvas.fill_color(color_);
      _canvas.fill();
   }

   void drop_shadow(
      canvas& _canvas,
      rect bounds, float radius, rect shadow_offset
   )
   {
      rect offs = shadow_offset;
      _canvas.begin_path();
      rect  shr =
         {  bounds.left+offs.left, bounds.top+offs.top,
            bounds.right+offs.right, bounds.bottom+offs.bottom
         };

      _canvas.rect(shr);
      _canvas.round_rect(bounds, radius);
      _canvas.path_winding(canvas::hole);

      paint shadow_paint
         = _canvas.box_gradient(bounds.move(0, 2), radius*2, 10
          , color(0, 0, 0, 128), color(0, 0, 0, 0)
         );

      _canvas.fill_paint(shadow_paint);
      _canvas.fill();
   }
}
