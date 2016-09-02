/*=================================================================================================
   Copyright (cnv) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <elf/pickup.hpp>
#include <photon/view.hpp>
#include <cmath>

namespace elf
{
   //namespace
   //{
   //   canvas::state prepare(rect& bounds, float slant, canvas& cnv)
   //   {
   //      auto state = cnv.new_state();
   //
   //      float w = bounds.width();
   //      float h = bounds.height();
   //
   //      cnv.translate({ bounds.left + (w/2), bounds.top + (h/2) });
   //      bounds = bounds.move(-(bounds.left + (w/2)), -(bounds.top + (h/2)));
   //
   //      cnv.rotate(slant);
   //      return state;
   //   }
   //
   //   bool hit_test_pickup(rect bounds, float slant, point mp, canvas& cnv)
   //   {
   //      auto state = prepare(bounds, slant, cnv);
   //
   //      cnv.begin_path();
   //      cnv.round_rect(bounds, bounds.width()/2);
   //      cnv.fill_color({ 0, 0, 0, 0 });
   //      cnv.fill();
   //
   //      bool r = cnv.on_fill(mp);
   //      return r;
   //   }
   //
   //   void draw_pickup(rect bounds, float slant, bool hilite, theme const& theme)
   //   {
   //      canvas& cnv = theme.canvas();
   //      auto state = prepare(bounds, slant, cnv);
   //
   //      auto outline_color = theme.frame_color;
   //      auto glow_color = theme.indicator_color;
   //      auto fill_color = theme.controls_color;
   //
   //      // Fill
   //      cnv.begin_path();
   //      cnv.round_rect(bounds, bounds.width()/2);
   //      cnv.fill_color(fill_color);
   //      cnv.fill();
   //
   //      if (hilite)
   //      {
   //         outline_color = outline_color.opacity(1).level(0.8);
   //         glow_color = glow_color.opacity(1).level(1.5);
   //      }
   //
   //      // Outline
   //      cnv.stroke_color(outline_color);
   //      cnv.stroke_width(2);
   //      cnv.stroke();
   //
   //      // Glow
   //      float glow_size = 10;
   //
   //      cnv.begin_path();
   //      auto outer_r = bounds.inset(-glow_size, -glow_size);
   //      cnv.round_rect(outer_r, outer_r.width()/2);
   //      cnv.round_rect(bounds, bounds.width()/2);
   //      cnv.path_winding(photon::canvas::hole);
   //
   //      photon::paint glow_paint
   //         = cnv.box_gradient(bounds, bounds.width()/2, glow_size
   //          , glow_color, { 0, 0, 0, 0 }
   //         );
   //
   //      cnv.fill_paint(glow_paint);
   //      cnv.fill();
   //   }
   //
   //   bool hit_test_rotator(rect bounds, float slant, point mp, theme const& theme)
   //   {
   //      canvas& cnv = theme.canvas();
   //      auto state = prepare(bounds, slant, cnv);
   //
   //      float size = (bounds.height() / 3);
   //      bounds = { bounds.left, bounds.bottom, bounds.right, bounds.bottom + size };
   //      cnv.begin_path();
   //      cnv.rect(bounds);
   //      cnv.fill_color({ 0, 0, 0, 0 });
   //      cnv.fill();
   //
   //      return cnv.on_fill(mp);
   //   }
   //
   //   point draw_rotator(rect bounds, float slant, theme& theme)
   //   {
   //      canvas& cnv = theme.canvas();
   //      auto state = prepare(bounds, slant, cnv);
   //
   //      // Draw rotator icon
   //      float size = (bounds.height() / 6);
   //      bounds = { bounds.left, bounds.bottom, bounds.right, bounds.bottom + size };
   //      photon::text_utils(theme).draw_icon(bounds, icons::arrows_cw, size);
   //
   //      return cnv.transform_point(center_point(bounds));
   //   }
   //}
   //
   //rect pickup::limits(basic_context const& ctx) const
   //{
   //   return { 400, 100, full_extent, full_extent };
   //}
   //
   //void pickup::draw(context const& ctx)
   //{
   //   rect  r1, r2;
   //   pickup_bounds(ctx, r1, r2);
   //   rect  pu_bounds = r1;
   //   if (!_single)
   //      pu_bounds.right = r2.right;
   //
   //   auto     mp = ctx.cursor_pos();
   //   auto&    cnv = ctx.theme().canvas();
   //   auto     thm = dynamic_cast<theme const&>(ctx.theme());
   //
   //   if (_single)
   //   {
   //      bool hilite = hit_test_pickup(r1, _slant, mp, cnv);
   //      draw_pickup(r1, _slant, hilite, thm);
   //   }
   //   else
   //   {
   //      bool hilite =
   //         hit_test_pickup(r1, _slant, mp, cnv) ||
   //         hit_test_pickup(r2, _slant, mp, cnv)
   //         ;
   //      draw_pickup(r1, _slant, hilite, thm);
   //      draw_pickup(r2, _slant, hilite, thm);
   //   }
   //
   //   if (hit_test_rotator(pu_bounds, _slant, mp, thm))
   //      _rotator_pos = draw_rotator(pu_bounds, _slant, thm);
   //}
   //
   //widget* pickup::hit_test(context const& ctx, point p)
   //{
   //   return hit(ctx, p) ? this : 0;
   //}
   //
   //widget* pickup::click(context const& ctx, mouse_button btn)
   //{
   //   if (btn.is_pressed)
   //   {
   //      _tracking = start;
   //      if (reposition(ctx, btn))
   //         return ctx.widget;
   //   }
   //   _tracking = none;
   //   return widget::click(ctx, btn);
   //}
   //
   //void pickup::drag(context const& ctx, mouse_button btn)
   //{
   //   if (_tracking == tracking_move || _tracking == tracking_rotate)
   //      reposition(ctx, btn);
   //}
   //
   //bool pickup::is_control() const
   //{
   //   return true;
   //}
   //
   //bool pickup::reposition(context const& ctx, mouse_button btn)
   //{
   //   point mp = ctx.cursor_pos();
   //
   //   rect r1, r2;
   //   pickup_bounds(ctx, r1, r2);
   //   rect pu_bounds = _single ? r1 : rect{ r1.left, r1.top, r2.right, r2.bottom };
   //
   //   if (_tracking == start)
   //   {
   //      int what = hit(ctx, mp);
   //      if (what == 1) // hit the pickup
   //      {
   //         // start tracking move
   //         _tracking = tracking_move;
   //         _offset = point{ mp.x-pu_bounds.left, mp.y-pu_bounds.top };
   //      }
   //      else if (what == 2)
   //      {
   //         // start tracking rotate
   //         _tracking = tracking_rotate;
   //         _offset = point{ mp.x-_rotator_pos.x, mp.y-_rotator_pos.y };
   //         if (btn.num_clicks == 2)
   //         {
   //            _slant = 0;
   //            ctx.window.draw();
   //            return true;
   //         }
   //      }
   //   }
   //
   //   // continue tracking move
   //   if (_tracking == tracking_move)
   //   {
   //      float w = (ctx.bounds.width() * scale_len) + pu_bounds.width();
   //      float offs = (ctx.bounds.width() - w) / 2;
   //      mp.x -= _offset.x + ctx.bounds.left + offs;
   //
   //      double align = mp.x / (w - pu_bounds.width());
   //      clamp(align, 0.0, 1.0);
   //      _pos = align;
   //      ctx.window.draw();
   //      return true;
   //   }
   //
   //   // continue tracking rotate
   //   if (_tracking == tracking_rotate)
   //   {
   //      mp = mp.move(-_offset.x, -_offset.y);
   //      auto center = center_point(pu_bounds);
   //      float angle = -std::atan2(mp.x-center.x, mp.y-center.y);
   //
   //      clamp(angle, -0.4, 0.4);
   //      _slant = angle;
   //      ctx.window.draw();
   //      return true;
   //   }
   //
   //   return false;
   //}
   //
   //int pickup::hit(context const& ctx, point p) const
   //{
   //   rect  r1, r2;
   //   pickup_bounds(ctx, r1, r2);
   //   rect  r = r1;
   //   if (!_single)
   //      r.right = r2.right;
   //
   //   auto&    cnv = ctx.theme().canvas();
   //   auto     thm = dynamic_cast<theme const&>(ctx.theme());
   //
   //   if (_single)
   //   {
   //      if (hit_test_pickup(r1, _slant, p, cnv))
   //         return 1;
   //   }
   //   else
   //   {
   //      if (hit_test_pickup(r1, _slant, ctx.cursor_pos(), cnv) ||
   //         hit_test_pickup(r2, _slant, ctx.cursor_pos(), cnv))
   //         return 1;
   //   }
   //
   //   if (hit_test_rotator(r, _slant, p, thm))
   //      return 2;
   //
   //   return 0;
   //}
   //
   //void pickup::pickup_bounds(context const& ctx, rect& r1, rect& r2) const
   //{
   //   auto     bounds = ctx.bounds;
   //   float    w = bounds.width() * scale_len;
   //   float    h = w * 0.19;
   //   clamp_max(h, ctx.bounds.height() * 0.8); // 0.8 to accomodate the rotator
   //
   //   float pu_w = h * 0.25;
   //   rect  pu_bounds  = { 0, 0, pu_w, h };
   //   if (!_single)
   //      pu_bounds.right += pu_w + 4;
   //
   //   rect  active_bounds = { 0, 0, w + pu_bounds.width(), h };
   //   active_bounds = center(active_bounds, bounds);
   //   pu_bounds = align(pu_bounds, active_bounds, _pos, 0.5);
   //
   //   if (_single)
   //   {
   //      r1 = pu_bounds;
   //   }
   //   else
   //   {
   //      r2 = r1 = pu_bounds;
   //      r1.right = r1.left + pu_w;
   //      r2.left = r2.right - pu_w;
   //   }
   //}
}
