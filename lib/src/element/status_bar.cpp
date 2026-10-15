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
      if (intersects(ctx.bounds, ctx.visible_bounds()))
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

   // A horizontal bar fills from the left, a vertical one from the bottom.
   rect status_bar_base::foreground_bounds(context const& ctx) const
   {
      auto bounds = ctx.bounds;

      if (bounds.width() > bounds.height())
         bounds.width(bounds.width() * value());
      else
         bounds.top = bounds.bottom - bounds.height() * value();

      return bounds;
   }

   using namespace std::chrono_literals;

   busy_bar_base::busy_bar_base(
      double init_value
    , double start_pos
   )
    : status_bar_base{init_value}
    , _start_pos{start_pos}
    , _animation_width{0.2}
    , _status{-_animation_width}
    , _time{0ms}
   {
   }

   // A copy does not share the original's animation: it starts stopped.
   busy_bar_base::busy_bar_base(busy_bar_base const& rhs)
    : status_bar_base{rhs}
    , _start_pos{rhs._start_pos}
    , _animation_width{rhs._animation_width}
    , _status{-_animation_width}
    , _time{0ms}
   {
   }

   busy_bar_base::~busy_bar_base()
   {
      *_alive = false;
   }

   void busy_bar_base::start_pos(double val)
   {
      _start_pos = clamp(val, 0.0, 1.0);
   }

   void busy_bar_base::animation_width(double val)
   {
      _animation_width = clamp(val, 0.0, 1.0);
   }

   // The segment from start_pos to value: across from the left in a
   // horizontal bar, up from the bottom in a vertical one.
   rect busy_bar_base::foreground_bounds(context const& ctx) const
   {
      auto bounds = ctx.bounds;
      auto const from = std::min(start_pos(), value());
      auto const to = std::max(start_pos(), value());

      if (bounds.width() > bounds.height())
      {
         auto const w = bounds.width();
         bounds.right = bounds.left + w * to;
         bounds.left = bounds.left + w * from;
      }
      else
      {
         auto const h = bounds.height();
         bounds.top = bounds.bottom - h * to;
         bounds.bottom = bounds.bottom - h * from;
      }
      return bounds;
   }

   void busy_bar_base::start(view& view_, duration time)
   {
      bool should_start = is_stopped();
      _time = time;
      if (should_start)
         animate(view_);
   }

   void busy_bar_base::stop(view& view_)
   {
      _time = 0ms;
      animate(view_);
   }

   // A segment animation_width wide sweeps across the bar, entering at one
   // end and leaving at the other, one hundredth of the bar per step.
   void busy_bar_base::animate(view& view_)
   {
      if (is_stopped())
      {
         _status = -_animation_width;
         value(0.0);
         start_pos(0.0);
         view_.refresh(*this);
         return;
      }

      _status += 0.01;
      if (_status >= 1.0)
         _status = -_animation_width;
      start_pos(_status);
      value(_status + _animation_width);
      view_.refresh(*this);
      view_.post(std::chrono::duration_cast<std::chrono::milliseconds>(_time),
         [&view_, this, alive = _alive]()
         {
            if (*alive)
               animate(view_);
         }
      );
   }

   std::string status_bar_base::class_name() const
   {
      return "status_bar";
   }

   std::string busy_bar_base::class_name() const
   {
      return "busy_bar";
   }
}
