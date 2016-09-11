/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <elf/pickup.hpp>
#include <photon/view.hpp>
#include <cmath>

namespace elf
{
   using photon::full_extent;
   using photon::canvas;
   using photon::clamp_max;
   using photon::widget;
   using photon::clamp;
   using photon::get_theme;

   namespace icons = photon::icons;
   float const scale_len = 0.8;

   namespace
   {
      canvas::state prepare(rect& bounds, float slant, canvas& canvas_)
      {
         auto state = canvas_.new_state();

         float l = bounds.left;
         float t = bounds.top;
         float w = bounds.width();
         float h = bounds.height();

         canvas_.translate({ l+(w/2), t+(h/2) });
         bounds = bounds.move(-(l+(w/2)), -(t+(h/2)));
         canvas_.rotate(slant);
         return state;
      }

      bool hit_test_pickup(rect bounds, float slant, point mp, canvas& canvas_)
      {
         auto state = prepare(bounds, slant, canvas_);

         canvas_.begin_path();
         canvas_.round_rect(bounds, bounds.width()/2);
         return canvas_.hit_test(canvas_.device_to_user(mp));
      }

      void draw_pickup(rect bounds, float slant, bool hilite, context const& ctx, char id)
      {
         auto        canvas_ = ctx.canvas();
         auto const& theme = get_theme();
         auto        state = prepare(bounds, slant, canvas_);

         auto outline_color = theme.frame_color;
         auto glow_color = theme.indicator_color;
         auto fill_color = theme.controls_color;

         // Fill
         canvas_.begin_path();
         canvas_.fill_style(fill_color);
         canvas_.fill_round_rect(bounds, bounds.width()/2);

         if (hilite)
         {
            outline_color = outline_color.opacity(1).level(0.8);
            glow_color = glow_color.opacity(1).level(1.5);
         }

         // Glow
         auto  alpha = glow_color.alpha;
         auto  radius = bounds.width()/2;

         canvas_.line_width(5);
         canvas_.stroke_style(glow_color.opacity(alpha * 0.1));
         canvas_.stroke_round_rect(bounds, radius);

         canvas_.line_width(4);
         canvas_.stroke_style(glow_color.opacity(alpha * 0.3));
         canvas_.stroke_round_rect(bounds, radius);

         canvas_.line_width(3);
         canvas_.stroke_style(glow_color.opacity(alpha * 0.5));
         canvas_.stroke_round_rect(bounds, radius);

         canvas_.line_width(2);
         canvas_.stroke_style(glow_color.opacity(alpha * 0.7));
         canvas_.stroke_round_rect(bounds, radius);

         // Outline
         canvas_.line_width(1);
         canvas_.stroke_style(outline_color);
         canvas_.stroke_round_rect(bounds, radius);
         
         // ID
         if (hilite)
         {
            canvas_.font("Roboto", 16);
            canvas_.fill_style(glow_color);
            canvas_.text_align(canvas_.middle | canvas_.center);
            float cx = bounds.left + (bounds.width() / 2);
            float cy = bounds.top + (bounds.height() / 4);
            char id_[] = { id, 0 };
            canvas_.fill_text(point{ cx, cy }, id_);
         }
      }

      bool hit_test_rotator(rect bounds, float slant, point mp, context const& ctx)
      {
         auto  canvas_ = ctx.canvas();
         auto  state = prepare(bounds, slant, canvas_);

         float size = (bounds.height() / 4);
         bounds = { bounds.left, bounds.bottom-10, bounds.right, bounds.bottom + size };
         canvas_.begin_path();
         canvas_.rect(bounds);
         return canvas_.hit_test(canvas_.device_to_user(mp));
      }

      point draw_rotator(rect bounds, float slant, context const& ctx)
      {
         auto        canvas_ = ctx.canvas();
         auto const& theme = get_theme();
         auto        state = prepare(bounds, slant, canvas_);

         // Draw rotator icon
         float  height = bounds.height();
         float  size = height / 4;
         bounds = { bounds.left, bounds.bottom-10, bounds.right, bounds.bottom + size };

         canvas_.font("photon_basic", 16);
         canvas_.fill_style(theme.icon_color);
         draw_icon(canvas_, bounds, icons::cycle);
         return canvas_.user_to_device(center_point(bounds));
      }
   }

   widget_limits pickup::limits(basic_context const& ctx) const
   {
      return { { 400, 100 }, { full_extent, full_extent } };
   }

   void pickup::draw(context const& ctx)
   {
      rect  r1, r2;
      pickup_bounds(ctx, r1, r2);

      auto  mp = ctx.cursor_pos();
      auto  canvas_ = ctx.canvas();

      rect  pu_bounds = r1;
      if (_type == double_)
         pu_bounds.right = r2.right;
      
      bool  is_tracking = _tracking != none;
      bool  hit_rotator = hit_test_rotator(pu_bounds, _slant, mp, ctx);

      if (_type == single)
      {
         bool hilite = is_tracking || hit_rotator || hit_test_pickup(r1, _slant, mp, canvas_);
         draw_pickup(r1, _slant, hilite, ctx, _id);
      }
      else
      {
         bool hilite =
            is_tracking ||
            hit_rotator ||
            hit_test_pickup(r1, _slant, mp, canvas_) ||
            hit_test_pickup(r2, _slant, mp, canvas_)
            ;
         draw_pickup(r1, _slant, hilite, ctx, _id);
         draw_pickup(r2, _slant, hilite, ctx, _id);
      }

      if (hit_rotator)
         _rotator_pos = draw_rotator(pu_bounds, _slant, ctx);
   }

   widget* pickup::hit_test(context const& ctx, point p)
   {
      return hit(ctx, p) ? this : nullptr;
   }

   bool pickup::cursor(context const& ctx, point p, cursor_tracking status)
   {
      ctx.view.refresh(ctx.bounds);
      return true;
   }

   bool pickup::reposition(context const& ctx, point mp)
   {
      rect r1, r2;
      pickup_bounds(ctx, r1, r2);
      rect pu_bounds = (_type == single) ? r1 : rect{ r1.left, r1.top, r2.right, r2.bottom };

      if (_tracking == start)
      {
         hit_item what = hit(ctx, mp);
         if (what == hit_pickup)
         {
            // start tracking move
            _tracking = tracking_move;
            _offset = point{ mp.x-pu_bounds.left, mp.y-pu_bounds.top };
         }
         else if (what == hit_rotator)
         {
            // start tracking rotate
            _tracking = tracking_rotate;
            _offset = point{ mp.x-_rotator_pos.x, mp.y-_rotator_pos.y };
         }
      }

      // continue tracking move
      if (_tracking == tracking_move)
      {
         float bw = ctx.bounds.width();
         float w = (bw * scale_len) + pu_bounds.width();
         float offs = (bw - w) / 2;
         mp.x -= _offset.x + ctx.bounds.left + offs;

         double align = mp.x / (w - pu_bounds.width());
         clamp(align, 0.0, 1.0);
         _pos = align;
         ctx.view.refresh(ctx.bounds);
         return true;
      }

      // continue tracking rotate
      if (_tracking == tracking_rotate)
      {
         mp = mp.move(-_offset.x, -_offset.y);
         auto center = center_point(pu_bounds);
         float angle = -std::atan2(mp.x-center.x, mp.y-center.y);

         clamp(angle, -0.4, 0.4);
         _slant = angle;
         ctx.view.refresh(ctx.bounds);
         return true;
      }

      return false;
   }

   pickup::hit_item pickup::hit(context const& ctx, point p) const
   {
      rect  r1, r2;
      pickup_bounds(ctx, r1, r2);
      rect  r = r1;
      if (_type == double_)
         r.right = r2.right;

      auto  canvas_ = ctx.canvas();

      if (_type == single)
      {
         if (hit_test_pickup(r1, _slant, p, canvas_))
            return hit_pickup;
      }
      else
      {
         if (hit_test_pickup(r1, _slant, ctx.cursor_pos(), canvas_) ||
            hit_test_pickup(r2, _slant, ctx.cursor_pos(), canvas_))
            return hit_pickup;
      }

      rect  pu_bounds = r1;
      if (_type == double_)
         pu_bounds.right = r2.right;
      if (hit_test_rotator(pu_bounds, _slant, p, ctx))
         return hit_rotator;

      return hit_none;
   }

   void pickup::pickup_bounds(context const& ctx, rect& r1, rect& r2) const
   {
      auto        bounds = ctx.bounds;
      float       w = bounds.width() * scale_len;
      float       h = w * 0.19;

      clamp_max(h, ctx.bounds.height() * (1/1.3));  // 0.8 to accomodate the rotator

      float pu_w = h * 0.25;
      rect  pu_bounds  = { 0, 0, pu_w, h };
      if (_type == double_)
         pu_bounds.right += pu_w + 4;

      rect  active_bounds = { 0, 0, w + pu_bounds.width(), h };
      active_bounds = center(active_bounds, bounds);
      pu_bounds = align(pu_bounds, active_bounds, _pos, 0.5);

      if (_type == single)
      {
         r1 = pu_bounds;
      }
      else
      {
         r2 = r1 = pu_bounds;
         r1.right = r1.left + pu_w;
         r2.left = r2.right - pu_w;
      }
   }

   widget* pickup::click(context const& ctx, mouse_button btn)
   {
      if (!btn.down && btn.num_clicks == 2)
      {
         _slant = 0;
         ctx.view.refresh(ctx.bounds);
      }
      return tracker<>::click(ctx, btn);
   }

   void pickup::begin_tracking(context const& ctx, info& track_info)
   {
      _tracking = start;
      reposition(ctx, track_info.current);
   }

   void pickup::keep_tracking(context const& ctx, info& track_info)
   {
      if (_tracking == tracking_move || _tracking == tracking_rotate)
         reposition(ctx, track_info.current);
   }

   void pickup::end_tracking(context const& ctx, info& track_info)
   {
      _tracking = none;
   }
}
