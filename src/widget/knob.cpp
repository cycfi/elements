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
   rect knob::limits(basic_context const& ctx) const
   {
      return { 16, 16, full_extent, full_extent };
   }

   void knob::draw(context const& ctx)
   {
      ctx.theme().draw_knob(_pos, ctx.bounds);
   }

   widget* knob::click(context const& ctx, mouse_button btn)
   {
      if (ctx.theme().knob_hit_test(ctx.bounds, ctx.cursor_pos()))
      {
         _tracking = true;
         reposition(ctx);
      }
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
      point center = center_point(ctx.bounds);
      float angle = -std::atan2(mp.x-center.x, mp.y-center.y);
      if (angle < 0.0f)
         angle += 2 * M_PI;

      float const travel = 0.82;
      float const rng = (2 * M_PI) * travel;
      float const offs = (2 * M_PI) * (1-travel)/2;

      float val = (angle-offs) / rng;
      if (std::abs(val - _pos) < 0.25)
         limit(_pos = val, 0.0f, 1.0f);
      ctx.window.draw();
   }

   bool knob::is_control() const
   {
      return true;
   }
}
