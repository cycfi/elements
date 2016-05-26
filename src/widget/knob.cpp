/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/knob.hpp>
#include <photon/theme.hpp>
#include <photon/support.hpp>
#include <photon/app.hpp>
#include <photon/window.hpp>
#include <cmath>

namespace photon
{
   namespace
   {
      void draw_knob(canvas& canvas_, circle cp, color fill_color)
      {
         float radius = cp.radius;
         float shadow = radius/8;

         // Knob Shadow
         paint shc
            = canvas_.radial_gradient(
                  point{ cp.cx, cp.cy }, radius, radius+shadow,
                  color{ 0, 0, 0, 64 }, color{ 0, 0, 0, 0 }
               );

         canvas_.begin_path();
         canvas_.circle({ cp.cx, cp.cy, radius+shadow });
         canvas_.circle(cp);
         canvas_.path_winding(canvas::hole);
         canvas_.fill_paint(shc);
         canvas_.fill();

         // Knob
         float    bevel = radius/5;

         paint knob
            = canvas_.linear_gradient(
                  point{ cp.cx, cp.cy-(radius-bevel) },
                  point{ cp.cx, cp.cy+(radius-bevel) },
                  color{ 255, 255, 255, 16 }, color{ 0, 0, 0, 16 }
               );

         canvas_.begin_path();
         canvas_.circle({ cp.cx, cp.cy, radius });
         canvas_.fill_color(fill_color);
         canvas_.fill();
         canvas_.fill_paint(knob);

         // Knob bevel
         paint bvc
            = canvas_.radial_gradient(
                  point{ cp.cx, cp.cy }, radius-bevel, radius,
                  color{ 0, 0, 0, 0 }, color{ 0, 0, 0, 64 }
               );

         canvas_.begin_path();
         canvas_.circle(cp);
         canvas_.circle({ cp.cx, cp.cy, radius-bevel });
         canvas_.path_winding(canvas::hole);
         canvas_.fill_paint(bvc);
         canvas_.fill();

         // Knob Outline
         canvas_.begin_path();
         canvas_.circle(circle{ cp.cx, cp.cy, cp.radius-1.0f });
         canvas_.stroke_width(1);
         canvas_.stroke_color(fill_color.level(0.6));
         canvas_.stroke();
      }

      point draw_knob_indicator(
         canvas& canvas_, circle cp, float pos, color indicator_color)
      {
         auto state = canvas_.new_state();

         canvas_.translate({ cp.cx, cp.cy });

         float const travel = 0.82;
         float const rng = (2 * M_PI) * travel;
         float const offs = (2 * M_PI) * (1-travel)/2;
         canvas_.rotate(offs + (pos * rng));

         float r = cp.radius;
         float ind_w = r/8;
         float ind_h = r/2.5;
         rect  ind_r = { -ind_w/2, -ind_h/2, ind_w/2, ind_h/2 };
         ind_r = ind_r.move(0, r*0.7);

         // Fill
         paint gr
            = canvas_.linear_gradient(
                  ind_r.top_right(), ind_r.bottom_right(),
                  color{ 0, 0, 0, 90 }, color{ 127, 127, 127, 90 }
               );

         canvas_.begin_path();
         canvas_.round_rect(ind_r, ind_w * 0.3);
         canvas_.fill_color(indicator_color);
         canvas_.fill();
         canvas_.fill_paint(gr);

         // Glow
         float ind_glow = ind_w * 0.7;
         paint glow_paint
            = canvas_.box_gradient(ind_r.inset(-ind_glow * 0.25, -ind_glow * 0.25)
             , ind_glow, ind_glow, indicator_color, color(0, 0, 0, 0)
            );

         canvas_.begin_path();
         canvas_.rect(ind_r.inset(-ind_glow, -ind_glow));
         canvas_.round_rect(ind_r, ind_w * 0.3);
         canvas_.path_winding(canvas::hole);
         canvas_.fill_paint(glow_paint);
         canvas_.fill();

         return canvas_.transform_point(center_point(ind_r));
      }

      void draw_knob_gauge(
         canvas& canvas_, float pos, circle cp
       , color controls_color, color indicator_color)
      {
         auto state = canvas_.new_state();

         float const travel = 0.82;
         float const rng = (2 * M_PI) * travel;
         float const offs = (M_PI/2) + ((2 * M_PI) * (1-travel)/2);
         float const pos_ = offs + (pos * rng);

         paint grad
            = canvas_.linear_gradient(
                  point{ cp.cx-cp.radius, cp.cy }, point{ cp.cx+cp.radius, cp.cy },
                  controls_color, indicator_color.level(1.5)
               );

         canvas_.begin_path();
         canvas_.arc({ cp.cx, cp.cy, cp.radius*1.25f }, offs, offs +  rng);
         canvas_.stroke_width(cp.radius/5);
         canvas_.stroke_color(controls_color.opacity(0.6));
         canvas_.stroke();

         canvas_.begin_path();
         canvas_.arc({ cp.cx, cp.cy, cp.radius*1.25f }, offs, pos_);
         canvas_.stroke_width(cp.radius/5);
         canvas_.stroke_paint(grad);
         canvas_.stroke();
      }
   }

   void knob::draw_knob(theme& thm, float pos, rect bounds, bool hilite)
   {
      auto     c = center_point(bounds);
      auto     r = std::min(bounds.width(), bounds.height())/2;
      circle   cp = { c.x, c.y, r };
      auto     controls_color = thm.controls_color;

      if (hilite)
         controls_color = controls_color.opacity(0.5).level(1.5);

      photon::draw_knob(thm.canvas(), cp, controls_color);
   }

   point knob::draw_indicator(theme& thm, float pos, rect bounds, bool hilite)
   {
      auto     c = center_point(bounds);
      auto     r = std::min(bounds.width(), bounds.height())/2;
      circle   cp = { c.x, c.y, r };
      auto     indicator_color = thm.indicator_color.level(1.5);

      if (hilite)
         indicator_color = indicator_color.opacity(1).level(2.0);

      return draw_knob_indicator(thm.canvas(), cp, pos, indicator_color);
   }

   void knob::draw_gauge(theme& thm, float pos, rect bounds, bool hilite)
   {
      auto  c = center_point(bounds);
      auto  r = std::min(bounds.width(), bounds.height())/2;
      auto  cp = circle{ c.x, c.y, r };

      auto  indicator_color = thm.indicator_color.level(1.2);
      auto  controls_color = thm.controls_color.opacity(1.0).level(1.5);

      if (hilite)
      {
         controls_color = controls_color.level(1.5);
         indicator_color = indicator_color.level(1.3);
      }

      draw_knob_gauge(
         thm.canvas(), pos, cp, controls_color, indicator_color
      );
   }

   rect knob::limits(basic_context const& ctx) const
   {
      return { 16, 16, full_extent, full_extent };
   }

   widget* knob::hit_test(context const& ctx, point p)
   {
      auto&    canvas_ = ctx.canvas();
      auto     c = center_point(ctx.bounds);
      auto     r = std::min(ctx.bounds.width(), ctx.bounds.height())/2;

      canvas_.begin_path();
      canvas_.circle(circle{ c.x, c.y, r });
      canvas_.fill_color({ 0, 0, 0, 0 });
      canvas_.fill();

      if (canvas_.on_fill(p))
         return this;
      return 0;
   }

   void knob::draw(context const& ctx)
   {
      auto&    thm = ctx.theme();
      auto     mp = ctx.cursor_pos();
      bool     hilite = _tracking || hit_test(ctx, mp);

      // make the bounds smaller to accomodate the gauge
      rect  b = ctx.bounds;
      b.width(b.width() * 0.72);
      b.height(b.height() * 0.72);
      b = center(b, ctx.bounds);

      draw_knob(thm, _pos, b, hilite);
      draw_gauge(thm, _pos, b, hilite);
      _indicator_pos = draw_indicator(thm, _pos, b, hilite);
   }

   widget* knob::click(context const& ctx, mouse_button btn)
   {
      _tracking = btn.is_pressed;
      if (_tracking)
      {
         point mp = ctx.cursor_pos();
         _offset = point{ mp.x-_indicator_pos.x, mp.y-_indicator_pos.y };
         reposition(ctx);
      }
      ctx.window.draw();

      return this;
   }

   void knob::drag(context const& ctx, mouse_button btn)
   {
      if (_tracking)
         reposition(ctx);
   }

   void knob::reposition(context const& ctx)
   {
      point mp = ctx.cursor_pos();
      mp = mp.move(-_offset.x, -_offset.y);
      _offset.x *= 0.95;
      _offset.y *= 0.95;

      point center = center_point(ctx.bounds);
      float angle = -std::atan2(mp.x-center.x, mp.y-center.y);
      if (angle < 0.0f)
         angle += 2 * M_PI;

      float const travel = 0.82;
      float const rng = (2 * M_PI) * travel;
      float const offs = (2 * M_PI) * (1-travel)/2;

      float val = (angle-offs) / rng;
      if (std::abs(val - _pos) < 0.6)
         limit(_pos = val, 0.0, 1.0);
      ctx.window.draw();
   }

   bool knob::is_control() const
   {
      return true;
   }

   bool knob::scroll(context const& ctx, point p)
   {
      _pos += p.y * 0.01;
      limit(_pos, 0.0, 1.0);
      ctx.window.draw();
      return true;
   }
}
