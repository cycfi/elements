/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/draw_utils.hpp>

namespace photon { namespace draw
{
   void round_rect_fill(NVGcontext* vg, rect const& r, color const& c, float corner_radius)
   {
      float x = r.left;
      float y = r.top;
      float w = r.width();
      float h = r.height();

      nvgBeginPath(vg);
      nvgRoundedRect(vg, x, y, w, h, corner_radius);
      nvgFillColor(vg, nvgRGBA(c));
      nvgFill(vg);
   }

   void round_rect_shadow(NVGcontext* vg, rect const& r, color const& c, float corner_radius)
   {
      float x = r.left;
      float y = r.top;
      float w = r.width();
      float h = r.height();

      auto trans = c;
      trans.alpha = 0.5f;

      NVGpaint shadow_paint
         = nvgBoxGradient(vg, x, y+2, w,h, corner_radius*2, 10, nvgRGBA(trans), nvgRGBA(c));
      nvgBeginPath(vg);
      nvgRect(vg, x-10, y-10, w+20, h+30);
      nvgRoundedRect(vg, x, y, w, h, corner_radius);
      nvgPathWinding(vg, NVG_HOLE);
      nvgFillPaint(vg, shadow_paint);
      nvgFill(vg);
   }

}}