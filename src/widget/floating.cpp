/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/floating.hpp>

namespace photon
{
   inline rect floating_widget::limits(basic_context const& ctx) const
   {
      return full_limits;
   }

   inline void floating_widget::prepare_subject(context& ctx)
   {
      rect  e_limits = this->subject().limits(ctx);
      ctx.bounds = this->bounds();
      float w = ctx.bounds.width();
      float h = ctx.bounds.height();

      if (w < e_limits.left)
         ctx.bounds.width(e_limits.left);
      else if (w > e_limits.right)
         ctx.bounds.width(e_limits.right);

      if (h < e_limits.top)
         ctx.bounds.height(e_limits.top);
      else if (w > e_limits.bottom)
         ctx.bounds.height(e_limits.bottom);
   }
}
