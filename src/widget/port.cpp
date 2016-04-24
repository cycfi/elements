/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/port.hpp>
#include <photon/window.hpp>
#include <algorithm>
#include <nanovg.h>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // port
   ////////////////////////////////////////////////////////////////////////////////////////////////
   rect port_widget::limits(basic_context const& ctx) const
   {
      rect e_limits = subject()->limits(ctx);
      return rect{ 0, 0, e_limits.right, e_limits.bottom };
   }

   void port_widget::prepare_subject(context& ctx)
   {
      rect     e_limits          = subject()->limits(ctx);
      double   elem_width        = e_limits.left;
      double   elem_height       = e_limits.top;
      double   available_width   = ctx.parent->bounds.width();
      double   available_height  = ctx.parent->bounds.height();

      ctx.bounds.left -= (elem_width - available_width) * _halign;
      ctx.bounds.width(elem_width);
      ctx.bounds.top -= (elem_height - available_height) * _valign;
      ctx.bounds.height(elem_height);
   }

   void port_widget::draw(context const& ctx)
   {
      nvgSave(ctx.canvas());
      nvgScissor(ctx.canvas(), ctx.bounds.left, ctx.bounds.top, ctx.bounds.width(), ctx.bounds.height());
      proxy::draw(ctx);
   }

   void scroller_widget::draw(context const& ctx)
   {
      port_widget::draw(ctx);
      rect  e_limits = subject()->limits(ctx);

      bool     has_h = e_limits.left > ctx.bounds.width();
      bool     has_v = e_limits.top > ctx.bounds.height();
      double   scroll_bar_width = ctx.theme().scroll_bar_width;

      if (has_v)
      {
         rect vscroll_bounds = {
            ctx.bounds.left + ctx.bounds.width() - scroll_bar_width,
            ctx.bounds.top,
            ctx.bounds.right,
            ctx.bounds.bottom - (has_h ? scroll_bar_width : 0)
         };

         ctx.theme().draw_scroll_bar(valign(), e_limits.top, vscroll_bounds);
      }

      if (has_h)
      {
         rect hscroll_bounds = {
            ctx.bounds.left,
            ctx.bounds.top + ctx.bounds.height() - scroll_bar_width,
            ctx.bounds.right - (has_v ? scroll_bar_width : 0),
            ctx.bounds.bottom
         };

         ctx.theme().draw_scroll_bar(halign(), e_limits.left, hscroll_bounds);
      }
   }

   bool scroller_widget::focus(focus_request r)
   {
      return true;
   }

   bool scroller_widget::scroll(context const& ctx, point const& p)
   {
      rect     e_limits = subject()->limits(ctx);
      double   dx = (-p.x / e_limits.left) * ctx.theme().scroll_bar_speed;
      double   dy = (-p.y / e_limits.top) * ctx.theme().scroll_bar_speed;
      double   alx = halign() + dx;
      double   aly = valign() + dy;

      limit(alx, 0.0, 1.0);
      limit(aly, 0.0, 1.0);

      halign(alx);
      valign(aly);
      ctx.window.draw();
      return true;
   }

   bool scroller_widget::is_control() const
   {
      return true;
   }
}
