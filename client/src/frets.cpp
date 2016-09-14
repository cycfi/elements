/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <infinity/frets.hpp>
#include <photon/view.hpp>
#include <cmath>

namespace infinity
{
   using photon::canvas;
   using photon::circle;
   using photon::full_extent;
   using photon::clamp_max;
   using photon::color;

   namespace colors = photon::colors;

   namespace
   {
      void set_gradient(rect bounds, canvas& cnv)
      {
         auto gradient = canvas::linear_gradient{
            { bounds.left, bounds.top },
            { bounds.right, bounds.top }
         };
         gradient.add_color_stop({ 0.0, { 255, 255, 255, 150 } });
         gradient.add_color_stop({ 1.0, { 127, 127, 127, 100 } });
         cnv.fill_style(gradient);
      }

      void draw_fret(rect bounds, canvas& cnv)
      {
         auto state = cnv.new_state();
         cnv.begin_path();
         cnv.rect(bounds);
         set_gradient(bounds, cnv);
         cnv.fill();
      }

      void draw_bridge(rect bounds, canvas& cnv)
      {
         cnv.begin_path();
         cnv.round_rect(bounds, bounds.width()/3);
         set_gradient(bounds, cnv);
         cnv.fill();
      }

      void draw_marker(circle c, photon::canvas& cnv)
      {
         cnv.begin_path();
         cnv.circle(c);
         cnv.fill_style(colors::antique_white.opacity(0.4));
         cnv.fill();
      }

      void draw_frets(rect bounds, canvas& cnv)
      {
         float const fret_size = 3;
         float       w = bounds.width();
         float       x = 0;
         float       y = bounds.top + 5;

         for (int i = -1; i < 13; ++i)
         {
            float prev = x;
            x = bounds.left + (w / std::pow(2.0f, i/12.0f));
            draw_fret({ x, bounds.top, x + fret_size, bounds.bottom }, cnv);
            switch (i)
            {
               case 0:
               case 3:
               case 5:
               case 7:
               case 9:
               case 12:
                  {
                     float pos = (fret_size/2) + x + ((prev-x)/2);
                     draw_marker({ pos, y, 3 }, cnv);
                     if (i == 0 || i == 12)
                        draw_marker({ pos, y+15, 3 }, cnv);
                  }
                  break;

               default:
                  break;
            };
         }
      }
   }

   widget_limits frets::limits(basic_context const& ctx) const
   {
      return { { 300, 60 }, { full_extent, full_extent } };
   }

   void frets::draw(context const& ctx)
   {
      float const scale_len = 0.8;
      float const w = 600 * scale_len;
      float const h = w * 0.15;
      rect const bbox = { 0, 0, 640, 150 };

      auto& cnv = ctx.canvas;
      auto  state = cnv.new_state();
      auto  scale = ctx.bounds.width() / bbox.width();
      float sch = bbox.height() * scale;
      float offs = (ctx.bounds.height()-sch)/2;

      cnv.scale({ scale, scale });
      cnv.translate({ ctx.bounds.left/scale, (ctx.bounds.top+offs)/scale });
      draw_frets(center({ 0, 0, w, h }, bbox), cnv);

      // The bridge
      float br_height = w * 0.2f;
      rect br_rect = { 0, 0, w, br_height };
      br_rect = center(br_rect, bbox);
      br_rect.width(8);
      draw_bridge(br_rect.move(-8, 0), cnv);
    }
}
