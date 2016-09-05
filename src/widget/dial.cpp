/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/dial.hpp>
#include <photon/support/theme.hpp>
#include <photon/view.hpp>
#include <cmath>

namespace photon
{
   double const _2pi = 2 * M_PI;
   double const travel = 0.83;
   double const range = _2pi * travel;
   double const start_angle = _2pi * (1-travel)/2;
   double const offset_angle = M_PI / 2.0;

   dial::dial(widget_ptr indicator, widget_ptr body, double init_value)
    : _indicator(indicator)
    , _body(body)
    , _value(init_value)
   {}

   point dial::indicator_point(context const& ctx) const
   {
      rect  bounds = body_bounds(ctx);
      float radius = (bounds.width() / 2) * 0.8;
      point center = center_point(bounds);
      float angle = (range * value()) + offset_angle + start_angle;

      // Compute the new dot position based on the angle
      return
         {
            float(center.x + (radius * cos(angle))),
            float(center.y + (radius * sin(angle)))
         };
   }

   void dial::draw(context const& ctx)
   {
      if (intersects(ctx.bounds, ctx.view.dirty()))
      {
         {
            context sctx { ctx, _body.get(), ctx.bounds };
            prepare_body(sctx);
            _body->draw(sctx);
         }
         {
            context sctx { ctx, _indicator.get(), ctx.bounds };
            prepare_indicator(sctx);
            _indicator->draw(sctx);
         }
      }
   }

   void dial::prepare_indicator(context& ctx)
   {
      point p = indicator_point(ctx);
      auto  ind_limits = _indicator->limits(ctx);
      rect  bounds{ 0, 0, ind_limits.right, ind_limits.bottom };

      bounds = bounds.move_to(
         (p.x - (bounds.width() / 2)),
         (p.y - (bounds.height() / 2))
      );
      ctx.bounds = bounds;
   }

   rect dial::body_bounds(context const& ctx) const
   {
      auto  bounds = ctx.bounds;
      auto  w = bounds.width();
      auto  h = bounds.height();
      if (w < h)
         bounds.height(w);
      else
         bounds.width(h);
      return center(bounds, ctx.bounds);
   }

   void dial::prepare_body(context& ctx)
   {
      ctx.bounds = body_bounds(ctx);
   }

   double dial::value(context const& ctx, point p)
   {
      point center = center_point(ctx.bounds);
      double angle = -std::atan2(p.x-center.x, p.y-center.y);
      if (angle < 0.0)
         angle += _2pi;

      float val = (angle-start_angle) / range;
      if (std::abs(val - value()) < 0.6)
      {
         clamp(val, 0.0, 1.0);
         return val;
      }

      return value();
   }

   void dial::begin_tracking(context const& ctx, info& track_info)
   {
   }

   void dial::keep_tracking(context const& ctx, info& track_info)
   {
      if (track_info.current != track_info.previous)
      {
         double new_value = value(ctx, track_info.current);
         if (_value != new_value)
         {
            _value = new_value;
            ctx.view.refresh(ctx.bounds);
         }
      }
   }

   void dial::end_tracking(context const& ctx, info& track_info)
   {
   }
}
