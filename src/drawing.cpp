/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/drawing.hpp>
#include <nanovg.h>

namespace photon
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

   void draw_panel(NVGcontext* vg, rect const& b, theme const& t)
   {
      double   x = b.left;
      double   y = b.top;
      double   w = b.width();
      double   h = b.height();
      color    c = t.panel_color;
      double   r = t.panel_corner_radius;
      rect     sh = t.panel_shadow_offset;

      // Round Rectangle
      nvgBeginPath(vg);
      nvgRoundedRect(vg, x, y, w, h, r);
      nvgFillColor(vg, nvgRGBA(c));
      nvgFill(vg);

      // Drop shadow
      nvgBeginPath(vg);
      nvgRect(vg, x+sh.left, y+sh.top, w+sh.right, h+sh.bottom);
      nvgRoundedRect(vg, x, y, w, h, r);
      nvgPathWinding(vg, NVG_HOLE);

      NVGpaint shadow_paint
         = nvgBoxGradient(
               vg, x, y+2, w, h, r*2, 10
             , ::nvgRGBA(0,0,0,128), ::nvgRGBA(0,0,0,0));

      nvgFillPaint(vg, shadow_paint);
      nvgFill(vg);
   }
}
