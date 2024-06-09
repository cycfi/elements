/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/slider.hpp>
#include <elements/support/theme.hpp>
#include <elements/view.hpp>
#include <cmath>

namespace cycfi::elements
{
   view_limits slider_base::limits(basic_context const& ctx) const
   {
      auto  limits_ = track().limits(ctx);
      auto  tmb_limits = thumb().limits(ctx);

      // We multiply thumb min limits by 2 so that there is always some space to move it.
      if (_is_horiz = limits_.max.x > limits_.max.y; _is_horiz)
      {
         limits_.min.y = std::max<float>(limits_.min.y, tmb_limits.min.y);
         limits_.max.y = std::max<float>(limits_.max.y, tmb_limits.max.y);
         limits_.min.x = std::max<float>(limits_.min.x, tmb_limits.min.x * 2);
      }
      else
      {
         limits_.min.x = std::max<float>(limits_.min.x, tmb_limits.min.x);
         limits_.max.x = std::max<float>(limits_.max.x, tmb_limits.max.x);
         limits_.min.y = std::max<float>(limits_.min.y, tmb_limits.min.y * 2);
      }

      return limits_;
   }

   void slider_base::layout(context const& ctx)
   {
      {
         context sctx {ctx, &track(), ctx.bounds};
         sctx.bounds = track_bounds(sctx);
         track().layout(sctx);
      }
      {
         context sctx {ctx, &thumb(), ctx.bounds};
         sctx.bounds = thumb_bounds(sctx);
         thumb().layout(sctx);
      }
   }

   void slider_base::draw(context const& ctx)
   {
      if (intersects(ctx.bounds, ctx.view_bounds()))
      {
         {
            context sctx {ctx, &track(), ctx.bounds};
            sctx.bounds = track_bounds(sctx);
            track().draw(sctx);
         }
         {
            context sctx {ctx, &thumb(), ctx.bounds};
            sctx.bounds = thumb_bounds(sctx);
            thumb().draw(sctx);
         }
      }
   }

   bool slider_base::scroll(context const& ctx, point dir, point p)
   {
      auto sdir = scroll_direction();
      double new_value = value() + (_is_horiz ? dir.x*sdir.x + !dir.x*dir.y*-sdir.y : dir.y * -sdir.y) * 0.005;
      clamp(new_value, 0.0, 1.0);
      track_scroll(ctx, dir, p);
      edit_value(new_value);
      ctx.view.refresh(ctx);
      return true;
   }

   rect slider_base::track_bounds(context const& ctx) const
   {
      auto  limits_ = track().limits(ctx);
      auto  bounds = ctx.bounds;
      auto  th_bounds = thumb_bounds(ctx);

      if (_is_horiz)
      {
         bounds.height(std::min<float>(limits_.max.y, bounds.height()));
         auto w2 = th_bounds.width() / 2;
         bounds.left += w2;
         bounds.right -= w2;
         bounds = center_v(bounds, ctx.bounds);
      }
      else
      {
         bounds.width(std::min<float>(limits_.max.x, bounds.width()));
         auto h2 = th_bounds.height() / 2;
         bounds.top += h2;
         bounds.bottom -= h2;
         bounds = center_h(bounds, ctx.bounds);
      }
      return bounds;
   }

   rect slider_base::thumb_bounds(context const& ctx) const
   {
      auto  bounds = ctx.bounds;
      auto  w = bounds.width();
      auto  h = bounds.height();
      auto  limits_ = thumb().limits(ctx);
      auto  tmb_w = limits_.max.x;
      auto  tmb_h = limits_.max.y;

      if (_is_horiz)
      {
         bounds.width(tmb_w);
         return bounds.move((w - tmb_w) * value(), 0);
      }
      else
      {
         bounds.height(tmb_h);
         return bounds.move(0, (h - tmb_h) * (1.0 - value()));
         // Note: for vertical sliders, 0.0 is at the bottom, hence 1.0-value()
      }
   }

   double slider_base::value_from_point(context const& ctx, point p)
   {
      auto  bounds = ctx.bounds;
      auto  w = bounds.width();
      auto  h = bounds.height();

      auto  limits_ = thumb().limits(ctx);
      auto  tmb_w = limits_.max.x;
      auto  tmb_h = limits_.max.y;
      auto  new_value = 0.0;

      // Note: for vertical sliders, 0.0 is at the bottom, hence 1.0-computed_value
      if (_is_horiz)
         new_value = (p.x - (bounds.left + (tmb_w / 2))) / (w - tmb_w);
      else
         new_value = 1.0 - ((p.y - (bounds.top + (tmb_h / 2))) / (h - tmb_h));
      return clamp(new_value, 0.0, 1.0);
   }

   void slider_base::begin_tracking(context const& ctx, tracker_info& track_info)
   {
      auto tmb_bounds = thumb_bounds(ctx);
      if (tmb_bounds.includes(track_info.current))
      {
         auto cp = center_point(tmb_bounds);
         track_info.offset.x = track_info.current.x - cp.x;
         track_info.offset.y = track_info.current.y - cp.y;
      }
   }

   void slider_base::keep_tracking(context const& ctx, tracker_info& track_info)
   {
      if (track_info.current != track_info.previous)
      {
         double new_value = value_from_point(ctx, track_info.current);
         if (_value != new_value)
         {
            edit_value(new_value);
            ctx.view.refresh(ctx);
         }
      }
   }

   void slider_base::end_tracking(context const& ctx, tracker_info& track_info)
   {
      double new_value = value_from_point(ctx, track_info.current);
      if (_value != new_value)
      {
         edit_value(new_value);
         ctx.view.refresh(ctx);
      }
   }

   void slider_base::value(double val)
   {
      _value = clamp(val, 0.0, 1.0);
   }

   double slider_base::value() const
   {
      return _value;
   }

   void basic_slider_base::edit_value(double val)
   {
      slider_base::edit_value(val);
      if (on_change)
         on_change(val);
   }

   void basic_selector_base::select(size_t val)
   {
      if (on_change)
         on_change(val);
   }
}
