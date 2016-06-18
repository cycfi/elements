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
#include <cmath>

namespace photon
{
   rect slider::limits(basic_context const& ctx) const
   {
      return { 32, 32, full_extent, full_extent };
   }

   namespace
   {
      // calculates the slider rectangles for its bounds, slot and knob
      struct slider_bounds
      {
         slider_bounds(context const& ctx, double _pos, slider::constants k)
          : bounds(ctx.bounds)
          , slot_r(ctx.bounds)
         {
            auto  w = bounds.width();
            auto  h = bounds.height();

            if (w > h)
            {
               bounds.height(std::min(w * k.aspect_ratio, h));
               bounds = center_v(bounds, ctx.bounds);

               slot_r.height(bounds.height() * k.slot_size);
               slot_r = center_v(slot_r, ctx.bounds);

               knob_r = bounds;
               knob_r.width(bounds.height() * k.knob_size);
               knob_r = align_h(knob_r, ctx.bounds, _pos);
            }
            else
            {
               bounds.width(std::min(h * k.aspect_ratio, w));
               bounds = center_h(bounds, ctx.bounds);

               slot_r.width(bounds.width() * k.slot_size);
               slot_r = center_h(slot_r, ctx.bounds);

               knob_r = bounds;
               knob_r.height(bounds.width() * k.knob_size);
               knob_r = align_v(knob_r, ctx.bounds, 1.0-_pos);
            }
         }

         bool is_horizontal() const
         {
            return bounds.width() > bounds.height();
         }

         rect  bounds;
         rect  slot_r;
         rect  knob_r;
      };

      void draw_slider_gauge(
         canvas& canvas_, point pos, rect bounds,
         color controls_color, color indicator_color)
      {
         bool  horiz = bounds.width() > bounds.height();
         rect  ind_r = bounds;
         auto  to    = horiz ? bounds.top_right() : bounds.top_left();
         auto  from  = horiz ? bounds.top_left() : bounds.bottom_left();
         auto  radius = (horiz ? bounds.height() : bounds.width()) * 0.4;

         if (horiz)
            ind_r.right = pos.x;
         else
            ind_r.top = pos.y;

         paint grad
            = canvas_.linear_gradient(
                  from, to,
                  controls_color, indicator_color.level(1.5)
               );

         canvas_.begin_path();
         canvas_.round_rect(ind_r, radius);
         canvas_.fill_paint(grad);
         canvas_.fill();
      }

      void draw_slider_slot(
         canvas& canvas_, point pos, rect bounds,
         color controls_color)
      {
         bool  horiz = bounds.width() > bounds.height();
         auto  radius = (horiz ? bounds.height() : bounds.width()) * 0.4;

         paint bg
            = canvas_.box_gradient(bounds.inset(0.5, 0.5).move(1, 1), radius, 2
             , color(0, 0, 0, 32), color(0, 0, 0, 128)
            );

         canvas_.begin_path();
         canvas_.round_rect(bounds, radius);
         canvas_.fill_paint(bg);
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

         // Fill
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

         // Glow
         float ind_glow = size * 0.7;
         paint glow_paint
            = canvas_.box_gradient(bounds.inset(-ind_glow * 0.25, -ind_glow * 0.25)
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
      slider_bounds  sl_pos{ ctx, _pos, dimensions() };

      draw_slot(ctx.theme(), sl_pos.knob_r, sl_pos.slot_r, hilite);
      if (_draw_gauge)
         draw_gauge(ctx.theme(), sl_pos.knob_r, sl_pos.slot_r, hilite);
      draw_knob(ctx.theme(), sl_pos.knob_r, sl_pos.is_horizontal(), hilite);
      draw_indicator(ctx.theme(), sl_pos.knob_r, sl_pos.is_horizontal(), hilite);
   }

   void slider::draw_slot(theme& thm, rect knob_r, rect bounds, bool hilite)
   {
      auto  controls_color = thm.controls_color.opacity(1.0).level(1.6);

      if (hilite)
         controls_color = controls_color.level(1.5);

      draw_slider_slot(
         thm.canvas(), center_point(knob_r), bounds, controls_color
      );
   }

   void slider::draw_gauge(theme& thm, rect knob_r, rect bounds, bool hilite)
   {
      auto  indicator_color = thm.indicator_color.level(1.2);
      auto  controls_color = thm.controls_color.opacity(1.0).level(1.6);

      if (hilite)
      {
         controls_color = controls_color.level(1.5);
         indicator_color = indicator_color.level(1.3);
      }

      draw_slider_gauge(
         thm.canvas(), center_point(knob_r), bounds,
         controls_color, indicator_color
      );
   }

   void slider::draw_knob(theme& thm, rect bounds, bool horiz, bool hilite)
   {
      auto  controls_color = thm.controls_color.opacity(1.0);

      if (hilite)
         controls_color = controls_color.level(1.5);

      photon::draw_knob(thm.canvas(), bounds, controls_color);
   }

   void slider::draw_indicator(theme& thm, rect bounds, bool horiz, bool hilite)
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
         slider_bounds  sl_pos{ ctx, _pos, dimensions() };
         if (sl_pos.bounds.includes(p))
            return this;
      }
      return 0;
   }

   widget* slider::click(context const& ctx, mouse_button btn)
   {
      point          p = ctx.cursor_pos();
      slider_bounds  sl_pos{ ctx, _pos, dimensions() };
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

   bool slider::scroll(context const& ctx, point p)
   {
      double   new_pos = _pos + ((ctx.bounds.width() < ctx.bounds.height()) ? p.y : -p.x) * 0.01;
      clamp(new_pos, 0.0, 1.0);
      if (_pos != new_pos)
      {
         if (on_change)
            new_pos = on_change(new_pos);
         _pos = new_pos;
         if (_pos != new_pos)
         {
            ctx.window.draw();
            return true;
         }
      }
      return false;
   }

   void slider::reposition(context const& ctx)
   {
      slider_bounds  sl_pos{ ctx, _pos, dimensions() };

      // See comment in the click function. We subtract the offset to
      // compensate for mouse tracking to avoid sudden knob movements.
      point mp = ctx.cursor_pos();
      mp = mp.move(-_offset.x, -_offset.y);

      double   w = sl_pos.bounds.width();
      double   h = sl_pos.bounds.height();
      double   new_pos;

      if (w > h)
      {
         // inset by knob size;
         double kw = sl_pos.knob_r.width();
         w -= kw;
         new_pos = (mp.x - (sl_pos.bounds.left + kw/2)) / w;
      }
      else
      {
         // inset by knob size;
         double kh = sl_pos.knob_r.height();
         h -= kh;
         new_pos = 1.0-((mp.y - (sl_pos.bounds.top + kh/2)) / h);
      }

      clamp(new_pos, 0.0, 1.0);
      if (_pos != new_pos)
      {
         if (on_change)
            new_pos = on_change(new_pos);
         if (_pos != new_pos)
         {
            _pos = new_pos;
            ctx.window.draw();
         }
      }
   }

   image_slider::image_slider(
      image_ptr img_, float aspect_ratio_
    , float knob_size_, point oversize_
    , bool draw_gauge)
    : slider(draw_gauge)
    , _img(img_)
    , _aspect_ratio(aspect_ratio_)
    , _knob_size(knob_size_)
    , _oversize(oversize_)
   {}

   void image_slider::draw_knob(theme& thm, rect bounds, bool horiz, bool hilite)
   {
      rect  img_bounds = {
         0, 0, bounds.width() * _oversize.x, bounds.height() * _oversize.y
      };

      img_bounds = center(img_bounds, bounds);
      _img->draw(img_bounds);
   }

   void image_slider::draw_indicator(theme& thm, rect bounds, bool horiz, bool hilite)
   {
   }

   slider::constants image_slider::dimensions() const
   {
      return { _aspect_ratio, 0.25, _knob_size };
   }
}
