/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/slider.hpp>
#include <photon/theme.hpp>
#include <photon/support.hpp>
#include <photon/app.hpp>
#include <photon/window.hpp>

namespace photon
{
   float slider::aspect_ratio    = 0.25;  // the slider's aspect ratio
   float slider::slot_size       = 0.3;   // fraction of width
   float slider::knob_size       = 0.6;   // fraction of size (width or height)

   namespace
   {
      void draw_em(canvas& canvas_, rect bounds)
      {
         canvas_.begin_path();
         canvas_.rect(bounds);
         canvas_.stroke_color(colors::white.opacity(0.5));
         canvas_.stroke_width(1);
         canvas_.stroke();
      }
   }

   rect slider::limits(basic_context const& ctx) const
   {
      return { 32, 32, full_extent, full_extent };
   }

   namespace
   {
      // calculates the slider rectangles for its bounds, slot and knob
      struct slider_bounds
      {
         slider_bounds(context const& ctx, double _pos)
          : bounds(ctx.bounds)
          , slot_r(ctx.bounds)
         {
            auto  w = bounds.width();
            auto  h = bounds.height();

            if (w > h)
            {
               bounds.height(std::min(w * slider::aspect_ratio, h));
               bounds = center_v(bounds, ctx.bounds);

               slot_r.height(bounds.height() * slider::slot_size);
               slot_r = center_v(slot_r, ctx.bounds);

               knob_r = bounds;
               knob_r.width(bounds.height() * slider::knob_size);
               knob_r = align_h(knob_r, ctx.bounds, _pos);
            }
            else
            {
               bounds.width(std::min(h * slider::aspect_ratio, w));
               bounds = center_h(bounds, ctx.bounds);

               slot_r.width(bounds.width() * slider::slot_size);
               slot_r = center_h(slot_r, ctx.bounds);

               knob_r = bounds;
               knob_r.height(bounds.width() * slider::knob_size);
               knob_r = align_v(knob_r, ctx.bounds, 1.0-_pos);
            }
         }

         rect  bounds;
         rect  slot_r;
         rect  knob_r;
      };

      void draw_slider_slot(
         canvas& canvas_, point pos, rect bounds,
         color controls_color, color indicator_color)
      {
         bool  h = bounds.width() > bounds.height();
         auto  from = h ? bounds.top_left() : bounds.bottom_left();
         auto  to = h ? bounds.top_right() : bounds.top_left();

         paint grad
            = canvas_.linear_gradient(
                  from, to,
                  controls_color, indicator_color.level(1.5)
               );

         canvas_.begin_path();
         canvas_.rect(bounds);
         canvas_.fill_color(controls_color.opacity(0.6));
         canvas_.fill();

         rect  ind_r = bounds;
         if (h)
            ind_r.right = pos.x;
         else
            ind_r.top = pos.y;

         canvas_.begin_path();
         canvas_.rect(ind_r);
         canvas_.fill_paint(grad);
         canvas_.fill();
      }

      void draw_knob(canvas& canvas_, rect bounds, color fill_color)
      {
         float w = bounds.width();
         float h = bounds.height();
         float corner_radius = ((w > h) ? w : h) / 5;

         // Fill
         canvas_.begin_path();
         canvas_.round_rect(bounds, corner_radius);
         canvas_.fill_color(fill_color);
         canvas_.fill();

         // Bevel
         canvas_.begin_path();
         canvas_.round_rect(bounds, corner_radius);
         canvas_.rect(bounds.inset(w/2, h/2));
         canvas_.path_winding(canvas::hole);

         paint bvc
            = canvas_.box_gradient(bounds, corner_radius, corner_radius
             , color(0, 0, 0, 0), color(0, 0, 0, 128)
            );

         canvas_.fill_paint(bvc);
         canvas_.fill();

         // Drop shadow
         canvas_.begin_path();
         canvas_.rect(bounds.inset(-10, -10));
         canvas_.round_rect(bounds, corner_radius);
         canvas_.path_winding(canvas::hole);

         paint shadow_paint
            = canvas_.box_gradient(bounds, corner_radius, corner_radius
             , color(0, 0, 0, 128), color(0, 0, 0, 0)
            );

         canvas_.fill_paint(shadow_paint);
         canvas_.fill();

         // Outline
         canvas_.begin_path();
         canvas_.round_rect(bounds, corner_radius);
         canvas_.stroke_width(1);
         canvas_.stroke_color(fill_color.level(0.6));
         canvas_.stroke();
      }

      void draw_knob_indicator(
         canvas& canvas_, rect bounds, color indicator_color)
      {
         float w = bounds.width();
         float h = bounds.height();
         float size = std::min(w, h);

         paint gr
            = canvas_.linear_gradient(
                  bounds.top_right(), bounds.bottom_right(),
                  color{ 0, 0, 0, 90 }, color{ 127, 127, 127, 90 }
               );

         canvas_.begin_path();
         canvas_.round_rect(bounds, size * 0.3);
         canvas_.fill_color(indicator_color);
         canvas_.fill();
         canvas_.fill_paint(gr);

         float ind_glow = size * 0.7;
         paint glow_paint
            = canvas_.box_gradient(bounds.inset(-ind_glow/4, -ind_glow/4)
             , ind_glow, ind_glow, indicator_color, color(0, 0, 0, 0)
            );

         canvas_.begin_path();
         canvas_.rect(bounds.inset(-ind_glow, -ind_glow));
         canvas_.round_rect(bounds, size * 0.3);
         canvas_.path_winding(canvas::hole);
         canvas_.fill_paint(glow_paint);
         canvas_.fill();
      }
   }

   void slider::draw(context const& ctx)
   {
      bool           hilite = _tracking || hit_test(ctx, ctx.cursor_pos());
      slider_bounds  sl_pos{ ctx, _pos };

      //draw_em(ctx.theme().canvas(), bounds);
      draw_slot(ctx.theme(), _pos, sl_pos.knob_r, sl_pos.slot_r, hilite);
      draw_knob(ctx.theme(), _pos, sl_pos.knob_r, hilite);
      draw_indicator(ctx.theme(), _pos, sl_pos.knob_r, hilite);
   }

   void slider::draw_slot(theme& thm, float pos, rect knob_r, rect bounds, bool hilite)
   {
      auto  indicator_color = thm.indicator_color;
      auto  controls_color = thm.controls_color.opacity(1.0).level(1.2);

      if (hilite)
      {
         controls_color = controls_color.level(1.5);
         indicator_color = indicator_color.level(1.3);
      }

      draw_slider_slot(
         thm.canvas(), center_point(knob_r), bounds,
         controls_color, indicator_color
      );
   }

   void slider::draw_knob(theme& thm, float pos, rect bounds, bool hilite)
   {
      auto  controls_color = thm.controls_color.opacity(1.0);

      if (hilite)
         controls_color = controls_color.level(1.5);

      photon::draw_knob(thm.canvas(), bounds, controls_color);
   }

   void slider::draw_indicator(theme& thm, float pos, rect bounds, bool hilite)
   {
      auto  indicator_color = thm.indicator_color.level(1.5);
      float w = bounds.width();
      float h = bounds.height();
      rect  ind_r = bounds;

      if (w > h)
      {
         ind_r.height(h * 0.25);
         ind_r.width(w * 0.6);
      }
      else
      {
         ind_r.width(w * 0.25);
         ind_r.height(h * 0.6);
      }

      ind_r = center(ind_r, bounds);

      if (hilite)
         indicator_color = indicator_color.opacity(1).level(2.0);

      draw_knob_indicator(thm.canvas(), ind_r, indicator_color);
   }

   widget* slider::hit_test(context const& ctx, point p)
   {
      if (ctx.bounds.includes(p))
      {
         slider_bounds  sl_pos{ ctx, _pos };
         if (sl_pos.bounds.includes(p))
            return this;
      }
      return 0;
   }

   widget* slider::click(context const& ctx, mouse_button btn)
   {
      point          p = ctx.cursor_pos();
      slider_bounds  sl_pos{ ctx, _pos };
      _tracking = btn.is_pressed;

      // If the mouse is inside the knob, record the offset from
      // the knob's center. We'll use this offset to compensate for
      // mouse tracking in the reposition function to avoid sudden knob
      // movements.
      if (sl_pos.knob_r.includes(p))
      {
         auto cp = center_point(sl_pos.knob_r);
         _offset = point{ p.x-cp.x, p.y-cp.y };
      }
      else
      {
         _offset = point{};
      }

      reposition(ctx);
      return this;
   }

   void slider::drag(context const& ctx, mouse_button btn)
   {
      if (_tracking)
         reposition(ctx);
   }

   bool slider::is_control() const
   {
      return true;
   }

   void slider::reposition(context const& ctx)
   {
      slider_bounds  sl_pos{ ctx, _pos };
      double         w = ctx.bounds.width();
      double         h = ctx.bounds.height();

      // See comment in the click function. We subtract the offset to
      // compensate for mouse tracking to avoid sudden knob movements.
      point mp = ctx.cursor_pos();
      mp = mp.move(-_offset.x, -_offset.y);

      if (w > h)
      {
         // inset by knob size;
         double kw = sl_pos.knob_r.width();
         w -= kw;
         _pos = (mp.x - (sl_pos.bounds.left + kw/2)) / w;
      }
      else
      {
         // inset by knob size;
         double kh = sl_pos.knob_r.height();
         h -= kh;
         _pos = 1.0-((mp.y - (sl_pos.bounds.top + kh/2)) / h);
      }

      limit(_pos, 0.0, 1.0);
      ctx.window.draw();
   }

/*
   float slider::knob_size  = 0.25;  // fraction of size (width or height)
   float slider::slot_size    = 0.15;  // fraction of size (width or height)

   namespace
   {
      //void draw_slider_knob(canvas& canvas_, circle cp, color fill_color)
      //{
      //   float radius = cp.radius;
      //   float shadow = radius/8;
      //
      //   // Knob Shadow
      //   paint shc
      //      = canvas_.radial_gradient(
      //            point{ cp.cx, cp.cy }, radius, radius+shadow,
      //            color{ 0, 0, 0, 64 }, color{ 0, 0, 0, 0 }
      //         );
      //
      //   canvas_.begin_path();
      //   canvas_.circle({ cp.cx, cp.cy, radius+shadow });
      //   canvas_.circle(cp);
      //   canvas_.path_winding(canvas::hole);
      //   canvas_.fill_paint(shc);
      //   canvas_.fill();
      //
      //   // Knob
      //   float    bevel = radius/5;
      //
      //   paint knob
      //      = canvas_.linear_gradient(
      //            point{ cp.cx, cp.cy-(radius-bevel) },
      //            point{ cp.cx, cp.cy+(radius-bevel) },
      //            color{ 255, 255, 255, 16 }, color{ 0, 0, 0, 16 }
      //         );
      //
      //   canvas_.begin_path();
      //   canvas_.circle({ cp.cx, cp.cy, radius });
      //   canvas_.fill_color(fill_color);
      //   canvas_.fill();
      //   canvas_.fill_paint(knob);
      //
      //   // Knob bevel
      //   paint bvc
      //      = canvas_.radial_gradient(
      //            point{ cp.cx, cp.cy }, radius-bevel, radius,
      //            color{ 0, 0, 0, 0 }, color{ 0, 0, 0, 64 }
      //         );
      //
      //   canvas_.begin_path();
      //   canvas_.circle(cp);
      //   canvas_.circle({ cp.cx, cp.cy, radius-bevel });
      //   canvas_.path_winding(canvas::hole);
      //   canvas_.fill_paint(bvc);
      //   canvas_.fill();
      //
      //   // Knob Outline
      //   canvas_.begin_path();
      //   canvas_.circle(circle{ cp.cx, cp.cy, cp.radius-1.0f });
      //   canvas_.stroke_width(2);
      //   canvas_.stroke_color(fill_color.level(0.6));
      //   canvas_.stroke();
      //}

      void draw_slider_slot(canvas& canvas_, rect bounds, float r)
      {
         paint bg
            = canvas_.box_gradient(bounds.inset(0.5, 0.5).move(1, 1), r, 2
             , color(0, 0, 0, 32), color(0, 0, 0, 128)
            );

         canvas_.begin_path();
         canvas_.round_rect(bounds, r);
         canvas_.fill_paint(bg);
         canvas_.fill();
      }

      //void draw_slider_knob_dot(canvas& canvas_, circle cp, color color_)
      //{
      //   canvas_.begin_path();
      //   canvas_.circle(cp);
      //   canvas_.fill_color(color_);
      //   canvas_.fill();
      //
      //   float glow = 1.4;
      //   paint glow_paint
      //      = canvas_.radial_gradient({ cp.cx, cp.cy }
      //       , cp.radius, cp.radius*glow, color_, color(0, 0, 0, 0)
      //      );
      //
      //   canvas_.begin_path();
      //   canvas_.circle({ cp.cx, cp.cy, cp.radius*glow });
      //   canvas_.circle(cp);
      //   canvas_.path_winding(canvas::hole);
      //   canvas_.fill_paint(glow_paint);
      //   canvas_.fill();
      //}

      void draw_slider_gauge(
         canvas& canvas_, point pos, rect bounds,
         float r, color color_, color indicator_color)
      {
         auto  from = (bounds.width() > bounds.height())
            ? bounds.top_left() : bounds.bottom_left();
         auto  to = (bounds.width() > bounds.height())
            ? bounds.top_right() : bounds.top_left();

         paint grad
            = canvas_.linear_gradient(
                  from, to,
                  color_, indicator_color.level(1.5)
               );

         //if (bounds.width() > bounds.height())
         //   bounds.right = pos.x;
         //else
         //   bounds.top = pos.y;

         auto state = canvas_.new_state();
         rect  clip_r = bounds;

         if (bounds.width() > bounds.height())
            clip_r.left = pos.x;
         else
            clip_r.top = pos.y;

         canvas_.clip(clip_r);

         canvas_.begin_path();
         canvas_.round_rect(bounds, r);
         canvas_.fill_paint(grad);
         canvas_.fill();
      }

      circle slider_knob_position(
         canvas& canvas_, float pos, rect bounds, float size)
      {
         float x = bounds.left;
         float y = bounds.top;
         float w = bounds.width();
         float h = bounds.height();

         if (w > h)
         {
            // horizontal
            float    cy = y + h * 0.5;
            float    kr = h * size;

            // inset by size;
            w -= kr * 2;
            x += kr;

            return { x+(pos*w), cy, kr };
         }
         else
         {
            // vertical
            float    cx = x + w * 0.5;
            float    kr = w * size;

            // inset by size;
            h -= kr * 2;
            y += kr;

            return { cx, y+h-(pos*h), kr };
         }
      }
   }

   rect slider::limits(basic_context const& ctx) const
   {
      return { 16, 16, full_extent, full_extent };
   }

   widget* slider::hit_test(context const& ctx, point p)
   {
      if (ctx.bounds.includes(p))
         return this;
      return 0;
   }

   void slider::draw_slot(theme& thm, rect bounds, float radius)
   {
      draw_slider_slot(thm.canvas(), bounds, radius);
   }

   void slider::draw_knob(theme& thm, float pos, rect bounds, bool hilite)
   {
      circle   cp = knob_position(thm, pos, bounds);
      circle   dotc = { cp.cx, cp.cy, cp.radius / 3 };
      auto     indicator_color = thm.indicator_color.level(1.5);
      auto     controls_color = thm.controls_color.opacity(1.0);

      if (hilite)
      {
         controls_color = controls_color.level(1.5);
         indicator_color = indicator_color.opacity(1).level(2.0);
      }

      //draw_slider_knob(thm.canvas(), cp, controls_color);
      //draw_slider_knob_dot(thm.canvas(), dotc, indicator_color);
   }

   circle slider::knob_position(theme& thm, float pos, rect bounds)
   {
      return slider_knob_position(thm.canvas(), pos, bounds, knob_size);
   }

   void slider::draw_gauge(theme& thm, rect bounds, float radius, bool hilite)
   {
      auto  indicator_color = thm.indicator_color;
      auto  controls_color = thm.controls_color.opacity(1.0);
      auto  pos = knob_position(thm, _pos, bounds);

      if (hilite)
      {
         controls_color = controls_color.opacity(1).level(1.5);
         indicator_color = indicator_color.opacity(1).level(1.3);
      }

      draw_slider_gauge(
         thm.canvas(), { pos.cx, pos.cy }, bounds, radius,
         controls_color, indicator_color
      );
   }

   void slider::draw(context const& ctx)
   {
      auto&       thm = ctx.theme();
      auto const& bounds = ctx.bounds;
      float       w = bounds.width();
      float       h = bounds.height();
      bool        hilite = _tracking || hit_test(ctx, ctx.cursor_pos());
      auto        state = thm.canvas().new_state();

      if (w > h)
      {
         // horizontal
         float sl = h * slot_size;
         float radius = sl/2;
         rect  slor_r = bounds.inset(0, (h-sl)/2);
         draw_slot(thm, slor_r, radius);
         draw_gauge(thm, slor_r, radius, hilite);
      }
      else
      {
         // vertical
         float sl = w * slot_size;
         float radius = sl/2;
         rect  slor_r = bounds.inset((w-sl)/2, 0);
         draw_slot(thm, slor_r, radius);
         draw_gauge(thm, slor_r, radius, hilite);
      }

      draw_knob(thm, _pos, bounds, hilite);
   }

   widget* slider::click(context const& ctx, mouse_button btn)
   {
      point p = ctx.cursor_pos();
      auto  cp = knob_position(ctx.theme(), _pos, ctx.bounds);
      _tracking = btn.is_pressed;

      // If the mouse is inside the knob, record the offset from
      // the knob's center. We'll use this offset to compensate for
      // mouse tracking in the reposition function to avoid sudden knob
      // movements.
      if (cp.bounds().includes(p))
         _offset = point{ p.x-cp.cx, p.y-cp.cy };
      else
         _offset = point{};

      reposition(ctx);
      return this;
   }

   void slider::drag(context const& ctx, mouse_button btn)
   {
      if (_tracking)
         reposition(ctx);
   }

   void slider::reposition(context const& ctx)
   {
      point    p = ctx.cursor_pos();
      float    x = ctx.bounds.left;
      float    y = ctx.bounds.top;
      float    w = ctx.bounds.width();
      float    h = ctx.bounds.height();
      auto     cp = knob_position(ctx.theme(), _pos, ctx.bounds);

      // See comment in the click function. We subtract the offset to
      // compensate for mouse tracking to avoid sudden knob movements.
      p.x -= _offset.x;
      p.y -= _offset.y;

      if (w > h)
      {
         // inset by radius;
         w -= cp.radius * 2;
         x += cp.radius;
         _pos = (p.x-x) / w;
      }
      else
      {
         // inset by radius;
         h -= cp.radius * 2;
         y += cp.radius;
         _pos = 1.0f-((p.y-y) / h);
      }

      limit(_pos, 0.0, 1.0);
      ctx.window.draw();
   }

   bool slider::is_control() const
   {
      return true;
   }
*/


}
