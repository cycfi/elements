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

   void theme::draw_panel(NVGcontext* vg, rect const& b)
   {
      double   x = b.left;
      double   y = b.top;
      double   w = b.width();
      double   h = b.height();
      color    c = panel_color;
      double   r = panel_corner_radius;
      rect     sh = panel_shadow_offset;

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
             , ::nvgRGBA(0, 0, 0, 128), ::nvgRGBA(0, 0, 0, 0)
            );

      nvgFillPaint(vg, shadow_paint);
      nvgFill(vg);
   }

   void theme::draw_slider(NVGcontext* vg, double pos, rect const& b)
   {
      double   x = b.left;
      double   y = b.top;
      double   w = b.width();
      double   h = b.height();

      double   cy = y + h * 0.5;
      double   sl = h * slider_slot_size;

      nvgSave(vg);

      // Slot
      NVGpaint bg
         = nvgBoxGradient(
               vg, x, cy-(sl/2)+1, w, sl, 2, 2,
               ::nvgRGBA(0, 0, 0, 32), ::nvgRGBA(0, 0, 0, 128)
            );

      nvgBeginPath(vg);
      nvgRoundedRect(vg, x, cy-(sl/2), w, sl, sl/3);
      nvgFillPaint(vg, bg);
      nvgFill(vg);

      draw_slider_knob(vg, pos, b);

      nvgRestore(vg);
   }

   void theme::draw_slider_knob(NVGcontext* vg, double pos, rect const& b)
   {
      double   x = b.left;
      double   y = b.top;
      double   w = b.width();
      double   h = b.height();

      double   cy = y + h * 0.5;
      double   kr = h * slider_knob_radius;
      
      // inset by radius;
      w -= kr * 2;
      x += kr;

      // change pos from fraction to actual pixel coord
      pos = x + (pos * w);

      // Knob Shadow
      NVGpaint bg
         = nvgRadialGradient(
               vg, pos, cy+1, kr-3, kr+3,
               ::nvgRGBA(0, 0, 0, 64), ::nvgRGBA(0, 0, 0, 0)
            );

      nvgBeginPath(vg);
      nvgRect(vg, pos-kr-5, cy-kr-5, kr*2+5+5, kr*2+5+5+3);
      nvgCircle(vg, pos, cy, kr);
      nvgPathWinding(vg, NVG_HOLE);
      nvgFillPaint(vg, bg);
      nvgFill(vg);

      // Knob
      NVGpaint knob
         = nvgLinearGradient(
               vg, x, cy-kr, x, cy+kr,
               ::nvgRGBA(255, 255, 255, 16), ::nvgRGBA(0, 0, 0, 16)
            );

      nvgBeginPath(vg);
      nvgCircle(vg, pos, cy, kr-1);
      nvgFillColor(vg, nvgRGBA(slider_knob_fill_color));
      nvgFill(vg);
      nvgFillPaint(vg, knob);
      nvgFill(vg);

      nvgBeginPath(vg);
      nvgCircle(vg, pos, cy, kr-0.5f);
      nvgStrokeColor(vg, nvgRGBA(slider_knob_outline_color));
      nvgStroke(vg);
   }
}
