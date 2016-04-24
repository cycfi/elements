/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/port.hpp>
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

      rect e_limits = subject()->limits(ctx);

      rect vscroll_bounds = {
         ctx.bounds.left + ctx.bounds.width() - 10,
         ctx.bounds.top,
         ctx.bounds.right,
         ctx.bounds.bottom
      };

      ctx.theme().draw_scroll_bar(_valign, e_limits.top, vscroll_bounds);

      rect hscroll_bounds = {
         ctx.bounds.left,
         ctx.bounds.top + ctx.bounds.height() - 10,
         ctx.bounds.right,
         ctx.bounds.bottom
      };

      ctx.theme().draw_scroll_bar(_halign, e_limits.left, hscroll_bounds);

      nvgRestore(ctx.canvas());
   }
}
