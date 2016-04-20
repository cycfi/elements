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
   rect slider_widget::limits(basic_context const& ctx) const
   {
      return { 16, 16, full_extent, full_extent };
   }

   void slider_widget::draw(context const& ctx)
   {
      ctx.theme().draw_slider(_pos, ctx.bounds);
   }

   widget* slider_widget::click(context const& ctx, mouse_button btn)
   {
      point p = ctx.cursor_pos();
      auto  cp = ctx.theme().slider_knob_position(_pos, ctx.bounds);

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

   void slider_widget::drag(context const& ctx, mouse_button btn)
   {
      reposition(ctx);
   }

   void slider_widget::reposition(context const& ctx)
   {
      point    p = ctx.cursor_pos();
      double   x = ctx.bounds.left;
      double   y = ctx.bounds.top;
      double   w = ctx.bounds.width();
      double   h = ctx.bounds.height();
      auto     cp = ctx.theme().slider_knob_position(_pos, ctx.bounds);

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
         _pos = (p.y-y) / h;
      }

      limit(_pos, 0.0, 1.0);
      ctx.window.draw();
   }

   bool slider_widget::is_control() const
   {
      return true;
   }
}
