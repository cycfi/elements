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
   float slider::knob_radius  = 0.25;  // fraction of size (width or height)
   float slider::slot_size    = 0.15;  // fraction of size (width or height)

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

   void draw_slider_knob(canvas& canvas_, circle cp, color color_)
   {
      // Knob Shadow
      paint bg
         = canvas_.radial_gradient(
               point{ cp.cx, cp.cy+1 }, cp.radius-3, cp.radius+3,
               color{ 0, 0, 0, 64 }, color{ 0, 0, 0, 0 }
            );

      canvas_.begin_path();
      canvas_.rect(cp.bounds().inset(-3, -3).move(-3, -3));
      canvas_.circle(cp);
      canvas_.path_winding(canvas::hole);
      canvas_.fill_paint(bg);
      canvas_.fill();

      // Knob
      paint knob
         = canvas_.linear_gradient(
               point{ cp.cx, cp.cy-cp.radius }, point{ cp.cx, cp.cy+cp.radius },
               color{ 255, 255, 255, 16 }, color{ 0, 0, 0, 16 }
            );

      canvas_.begin_path();
      canvas_.circle(circle{ cp.cx, cp.cy, cp.radius-1 });
      canvas_.fill_color(color_);
      canvas_.fill();
      canvas_.fill_paint(knob);
      canvas_.fill();

      canvas_.begin_path();
      canvas_.circle(circle{ cp.cx, cp.cy, cp.radius-0.5f });
      canvas_.stroke_color(color_.level(0.6));
      canvas_.stroke_width(1.0f);
      canvas_.stroke();
   }

   void draw_slider_knob_dot(canvas& canvas_, circle cp, color color_)
   {
      canvas_.begin_path();
      canvas_.circle(cp);
      canvas_.fill_color(color_.level(1.5));
      canvas_.fill();

      float glow = 1.4;
      paint glow_paint
         = canvas_.radial_gradient({ cp.cx, cp.cy }
          , cp.radius, cp.radius*glow, color_, color(0, 0, 0, 0)
         );

      canvas_.begin_path();
      canvas_.circle({ cp.cx, cp.cy, cp.radius*glow });
      canvas_.circle(cp);
      canvas_.path_winding(canvas::hole);
      canvas_.fill_paint(glow_paint);
      canvas_.fill();
   }

   void draw_slider_gauge(
      canvas& canvas_, circle pos, rect bounds,
      float r, color color_, color indicator_color)
   {
      auto  from = (bounds.width() > bounds.height())
         ? bounds.top_left() : bounds.bottom_left();
      auto  to = (bounds.width() > bounds.height())
         ? bounds.top_right() : bounds.top_left();

      paint grad
         = canvas_.linear_gradient(
               from, to,
               color_.level(1.5), indicator_color.level(1.5)
            );

      if (bounds.width() > bounds.height())
         bounds.right = pos.cx;
      else
         bounds.top = pos.cy;

      canvas_.begin_path();
      canvas_.round_rect(bounds, r);
      canvas_.fill_paint(grad);
      canvas_.fill();
   }

   circle slider_knob_position(canvas& canvas_, float pos, rect bounds, float radius)
   {
      float x = bounds.left;
      float y = bounds.top;
      float w = bounds.width();
      float h = bounds.height();

      if (w > h)
      {
         // horizontal
         float    cy = y + h * 0.5;
         float    kr = h * radius;

         // inset by radius;
         w -= kr * 2;
         x += kr;

         return { x+(pos*w), cy, kr };
      }
      else
      {
         // vertical
         float    cx = x + w * 0.5;
         float    kr = w * radius;

         // inset by radius;
         h -= kr * 2;
         y += kr;

         return { cx, y+h-(pos*h), kr };
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
      circle   c = knob_position(thm, pos, bounds);
      circle   dotc = { c.cx, c.cy, c.radius / 3 };
      auto     indicator_color = thm.indicator_color;
      auto     controls_color = thm.controls_color.opacity(1.0f).level(1.5);

      if (hilite)
      {
         controls_color = controls_color.opacity(1).level(1.5);
         indicator_color = indicator_color.opacity(1).level(1.5);
      }

      draw_slider_knob(thm.canvas(), c, controls_color);
      draw_slider_knob_dot(thm.canvas(), dotc, indicator_color);
   }

   circle slider::knob_position(theme& thm, float pos, rect bounds)
   {
      return slider_knob_position(thm.canvas(), pos, bounds, knob_radius);
   }

   void slider::draw_gauge(theme& thm, rect bounds, float radius, bool hilite)
   {
      auto     indicator_color = thm.indicator_color;
      auto     controls_color = thm.controls_color.opacity(1.0f);
      circle   pos = knob_position(thm, _pos, bounds);

      if (hilite)
      {
         controls_color = controls_color.opacity(1).level(1.5);
         indicator_color = indicator_color.opacity(1).level(1.5);
      }

      draw_slider_gauge(
         thm.canvas(), pos, bounds, radius,
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
}
