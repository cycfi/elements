/*=============================================================================
   Copyright (c) 2020 Michal Urbanski

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/progress_bar.hpp>
#include <elements/support/draw_utils.hpp>
#include <elements/support/theme.hpp>
#include <elements/view.hpp>
#include <algorithm>

namespace cycfi::elements
{
   view_limits progress_bar_base::limits(basic_context const& ctx) const
   {
      auto const fg_limits = foreground().limits(ctx);
      auto       bg_limits = background().limits(ctx);

      bg_limits.min.y = std::max(bg_limits.min.y, fg_limits.min.y);
      bg_limits.min.x = std::max(bg_limits.min.x, fg_limits.min.x);

      if (bg_limits.max.x > bg_limits.max.y)
         bg_limits.max.y = std::max(bg_limits.max.y, fg_limits.max.y);
      else
         bg_limits.max.x = std::max(bg_limits.max.x, fg_limits.max.x);

      return bg_limits;
   }

   void progress_bar_base::layout(context const& ctx)
   {
      {
         context sctx { ctx, &background(), ctx.bounds };
         sctx.bounds = background_bounds(sctx);
         background().layout(sctx);
      }
      {
         context sctx { ctx, &foreground(), ctx.bounds };
         sctx.bounds = foreground_bounds(sctx);
         foreground().layout(sctx);
      }
   }

   void progress_bar_base::draw(context const& ctx)
   {
      if (intersects(ctx.bounds, ctx.view_bounds()))
      {
         {
            context sctx { ctx, &background(), ctx.bounds };
            sctx.bounds = background_bounds(sctx);
            background().draw(sctx);
         }
         {
            context sctx { ctx, &foreground(), ctx.bounds };
            sctx.bounds = foreground_bounds(sctx);
            foreground().draw(sctx);
         }
      }
   }

   void progress_bar_base::value(double val)
   {
      _value = clamp(val, 0.0, 1.0);
   }

   rect progress_bar_base::background_bounds(context const& ctx) const
   {
      auto const limits_ = background().limits(ctx);
      auto bounds = ctx.bounds;
      bounds.height(std::min<float>(limits_.max.y, bounds.height()));
      bounds.width(std::min<float>(limits_.max.x, bounds.width()));
      return bounds;
   }

   rect progress_bar_base::foreground_bounds(context const& ctx) const
   {
      auto bounds = ctx.bounds;

      if (_is_horiz)
         bounds.width(bounds.width() * value());
      else
         bounds.height(bounds.height() * (1.0 - value()));

      return bounds;
   }
}
