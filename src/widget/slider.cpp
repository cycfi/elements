/*=======================================================================================
   Copyright (c) 2016-2017 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=======================================================================================*/
#include <photon/widget/slider.hpp>
#include <photon/support/theme.hpp>
#include <photon/view.hpp>
#include <cmath>

namespace photon
{
   widget_limits slider_base::limits(basic_context const& ctx) const
   {
      auto  limits_ = body().limits(ctx);
      auto  ind_limits = indicator().limits(ctx);

      if ((_is_horiz = limits_.max.x > limits_.max.y))
      {
         limits_.min.y = std::max<float>(limits_.min.y, ind_limits.min.y);
         limits_.max.y = std::max<float>(limits_.max.y, ind_limits.max.y);
         limits_.min.x = std::max<float>(limits_.min.x, ind_limits.min.x * 2);
         limits_.max.x = std::max<float>(limits_.max.x, limits_.max.x);
      }
      else
      {
         limits_.min.x = std::max<float>(limits_.min.x, ind_limits.min.x);
         limits_.max.x = std::max<float>(limits_.max.x, ind_limits.max.x);
         limits_.min.y = std::max<float>(limits_.min.y, ind_limits.min.y * 2);
         limits_.max.y = std::max<float>(limits_.max.y, limits_.max.y);
      }

      return limits_;
   }

   void slider_base::layout(context const& ctx)
   {
      {
         context sctx { ctx, &body(), ctx.bounds };
         sctx.bounds = indicator_bounds(sctx);
         body().layout(sctx);
      }
      {
         context sctx { ctx, &indicator(), ctx.bounds };
         sctx.bounds = body_bounds(sctx);
         indicator().layout(sctx);
      }
   }

   void slider_base::draw(context const& ctx)
   {
      if (intersects(ctx.bounds, ctx.view.dirty()))
      {
         {
            context sctx { ctx, &body(), ctx.bounds };
            sctx.bounds = body_bounds(sctx);
            body().draw(sctx);
         }
         {
            context sctx { ctx, &indicator(), ctx.bounds };
            sctx.bounds = indicator_bounds(sctx);
            indicator().draw(sctx);
         }
      }
   }

   bool slider_base::scroll(context const& ctx, point dir, point p)
   {
      double new_value = value() + (_is_horiz ? -dir.x : dir.y) * 0.005;
      clamp(new_value, 0.0, 1.0);
      if (value() != new_value)
      {
         value(new_value);
         ctx.view.refresh(ctx);
         return true;
      }
      return false;
   }

   rect slider_base::body_bounds(context const& ctx) const
   {
      auto  limits_ = body().limits(ctx);
      auto  bounds = ctx.bounds;

      if (_is_horiz)
      {
         auto h = bounds.height();
         bounds.height(std::min<float>(limits_.max.y, h));
         bounds = center_v(bounds, ctx.bounds);
      }
      else
      {
         auto w = bounds.width();
         bounds.width(std::min<float>(limits_.max.x, w));
         bounds = center_h(bounds, ctx.bounds);
      }
      return bounds;
   }

   rect slider_base::indicator_bounds(context const& ctx) const
   {
      auto  bounds = ctx.bounds;
      auto  w = bounds.width();
      auto  h = bounds.height();
      auto  limits_ = indicator().limits(ctx);
      auto  ind_w = limits_.max.x;
      auto  ind_h = limits_.max.y;

      if (_is_horiz)
      {
         bounds.width(ind_w);
         return bounds.move((w - ind_w) * value(), 0);
      }
      else
      {
         bounds.height(ind_h);
         return bounds.move(0, (h - ind_h) * (1.0 - value()));
         // Note: for vertical sliders, 0.0 is at the bottom, hence 1.0-value()
      }
   }

   double slider_base::value_from_point(context const& ctx, point p)
   {
      auto  bounds = ctx.bounds;
      auto  w = bounds.width();
      auto  h = bounds.height();

      auto  limits_ = indicator().limits(ctx);
      auto  ind_w = limits_.max.x;
      auto  ind_h = limits_.max.y;
      auto  new_value = 0.0;

      if (_is_horiz)
      {
         new_value = (p.x - (bounds.left + (ind_w / 2))) / (w - ind_w);
      }
      else
      {
         new_value = 1.0 - ((p.y - (bounds.top + (ind_h / 2))) / (h - ind_h));
         // Note: for vertical sliders, 0.0 is at the bottom, hence 1.0-computed_value
      }

      clamp(new_value, 0.0, 1.0);
      return new_value;
   }

   void slider_base::begin_tracking(context const& ctx, info& track_info)
   {
      auto ind_bounds = indicator_bounds(ctx);
      if (ind_bounds.includes(track_info.current))
      {
         auto cp = center_point(ind_bounds);
         track_info.offset.x = track_info.current.x - cp.x;
         track_info.offset.y = track_info.current.y - cp.y;
      }
   }

   void slider_base::keep_tracking(context const& ctx, info& track_info)
   {
      if (track_info.current != track_info.previous)
      {
         double new_value = value_from_point(ctx, track_info.current);
         if (_value != new_value)
         {
            value(new_value);
            ctx.view.refresh(ctx);
         }
      }
   }

   void slider_base::end_tracking(context const& ctx, info& track_info)
   {
      double new_value = value_from_point(ctx, track_info.current);
      if (_value != new_value)
      {
         value(new_value);
         ctx.view.refresh(ctx);
      }
   }

   void slider_base::value(double val)
   {
      clamp(val, 0.0, 1.0);
      _value = val;
   }
   
   double  slider_base::value() const
   {
      return _value;
   }
   
   void basic_slider_base::value(double val)
   {
      slider_base::value(val);
      if (on_change)
         on_change(val);
   }
   
   void basic_selector_base::select(size_t val)
   {
      if (on_change)
         on_change(val);
   }
}
