/*=============================================================================
   Copyright (c) 2020 Michał Urbański
   Copyright (c) 2024 Flole
   Copyright (c) 2024 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/status_bar.hpp>
#include <elements/support/draw_utils.hpp>
#include <elements/support/theme.hpp>
#include <elements/view.hpp>
#include <algorithm>

namespace cycfi::elements
{
   view_limits status_bar_base::limits(basic_context const& ctx) const
   {
      auto const fg_limits = foreground().limits(ctx);
      auto bg_limits = background().limits(ctx);

      bg_limits.min.y = std::max(bg_limits.min.y, fg_limits.min.y);
      bg_limits.min.x = std::max(bg_limits.min.x, fg_limits.min.x);

      if (bg_limits.max.x > bg_limits.max.y)
         bg_limits.max.y = std::max(bg_limits.max.y, fg_limits.max.y);
      else
         bg_limits.max.x = std::max(bg_limits.max.x, fg_limits.max.x);

      return bg_limits;
   }

   void status_bar_base::layout(context const& ctx)
   {
      {
         context sctx {ctx, &background(), ctx.bounds};
         sctx.bounds = background_bounds(sctx);
         background().layout(sctx);
      }
      {
         context sctx {ctx, &foreground(), ctx.bounds};
         sctx.bounds = foreground_bounds(sctx);
         foreground().layout(sctx);
      }
   }

   void status_bar_base::draw(context const& ctx)
   {
      if (intersects(ctx.bounds, ctx.view_bounds()))
      {
         {
            context sctx {ctx, &background(), ctx.bounds};
            sctx.bounds = background_bounds(sctx);
            background().draw(sctx);
         }
         {
            context sctx {ctx, &foreground(), ctx.bounds};
            sctx.bounds = foreground_bounds(sctx);
            foreground().draw(sctx);
         }
      }
   }

   void status_bar_base::value(double val)
   {
      _value = clamp(val, 0.0, 1.0);
   }

   rect status_bar_base::background_bounds(context const& ctx) const
   {
      auto const limits_ = background().limits(ctx);
      auto bounds = ctx.bounds;
      bounds.height(std::min<float>(limits_.max.y, bounds.height()));
      bounds.width(std::min<float>(limits_.max.x, bounds.width()));
      return bounds;
   }

   rect status_bar_base::foreground_bounds(context const& ctx) const
   {
      auto bounds = ctx.bounds;

      if (bounds.width() > bounds.height())
         bounds.width(bounds.width() * value());
      else
         bounds.height(bounds.height() * (1.0 - value()));

      return bounds;
   }

   // Implement the start method as per the patch
   void busy_bar_base::start_pos(double val)
   {
      _start_pos = clamp(val, 0.0, 1.0);
   }

   // Implement foreground_bounds to include the start value
   rect busy_bar_base::foreground_bounds(context const& ctx) const
   {
      auto bounds = ctx.bounds;

      if (bounds.width() > bounds.height())
         bounds.width(bounds.width() * value());
      else
         bounds.height(bounds.height() * (1.0 - value()));

      // Adjust bounds based on the start value
      if (bounds.width() > bounds.height())
         bounds.left = bounds.left + (bounds.width() * start_pos());
      else
         bounds.bottom = bounds.bottom + (bounds.height() * start_pos());

      return bounds;
   }

   void busy_bar_base::start(view& view_, duration time)
   {
      _time = time;
      animate(view_);
   }

   void busy_bar_base::stop(view& view_)
   {
      using namespace std::chrono_literals;
      _time = 0ms;
      animate(view_);
   }

   void busy_bar_base::animate(view& view_)
   {
      if (_time == _time.zero())
      {
         _status = -0.2;
         value(0.0);
         start_pos(0.0);
         view_.refresh();
         return;
      }

      _status += 0.01;
      start_pos(_status);
      value(_status + 0.2);
      if (_status >= 1.0)
         _status = -0.2;
      view_.refresh();
      view_.post(std::chrono::duration_cast<std::chrono::milliseconds>(_time),
         [&view_, this]()
         {
            animate(view_);
         }
      );
   }
}
