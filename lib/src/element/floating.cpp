/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/floating.hpp>
#include <elements/support/context.hpp>

namespace cycfi::elements
{
   view_limits floating_element::limits(basic_context const& ctx) const
   {
      auto e_limits = this->subject().limits(ctx);
      return {{e_limits.min.x, e_limits.min.y}, {full_extent, full_extent}};
   }

   void floating_element::prepare_subject(context& ctx)
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

      ctx.bounds = center(ctx.bounds, this->bounds());
      this->bounds(ctx.bounds);
   }

   void floating_element::minimize(context& ctx)
   {
      auto  e_limits = this->subject().limits(ctx);
      auto bounds = this->bounds();
      bounds.width(e_limits.min.x);
      bounds.height(e_limits.min.y);
      this->bounds(bounds);
   }

   void floating_element::maximize(context& ctx)
   {
      auto  e_limits = this->subject().limits(ctx);
      auto bounds = this->bounds();
      bounds.width(e_limits.max.x);
      bounds.height(e_limits.max.y);
      this->bounds(bounds);
   }
}

