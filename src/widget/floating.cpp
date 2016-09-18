/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/floating.hpp>
#include <photon/support/context.hpp>

namespace photon
{
   inline widget_limits floating_widget::limits(basic_context const& ctx) const
   {
      return full_limits;
   }

   inline void floating_widget::prepare_subject(context& ctx)
   {
      ctx.bounds = this->bounds();
      auto  e_limits = this->subject().limits(ctx);
      float w = ctx.bounds.width();
      float h = ctx.bounds.height();

      if (w < e_limits.min.x)
         ctx.bounds.width(e_limits.min.x);
      else if (w > e_limits.max.x)
         ctx.bounds.width(e_limits.max.x);

      if (h < e_limits.min.y)
         ctx.bounds.height(e_limits.min.y);
      else if (h > e_limits.max.y)
         ctx.bounds.height(e_limits.max.y);
   }
}
