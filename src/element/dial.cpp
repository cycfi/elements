/*=============================================================================
   Copyright (c) 2016-2017 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <photon/element/dial.hpp>
#include <photon/support/theme.hpp>
#include <photon/support/draw_utils.hpp>
#include <photon/view.hpp>
#include <cmath>

#include <iostream>

namespace photon
{
   double const _2pi = 2 * M_PI;
   double const travel = 0.83;
   double const range = _2pi * travel;
   double const start_angle = _2pi * (1-travel)/2;

   dial_base::dial_base(double init_value)
    : _value(init_value)
   {
      clamp(_value, 0.0, 1.0);
   }

   void dial_base::prepare_subject(context& ctx)
   {
      proxy_base::prepare_subject(ctx);
      subject().value(_value);
   }

   void dial_base::value(double val)
   {
      clamp(val, 0.0, 1.0);
      _value = val;
      subject().value(_value);
      if (on_change)
         on_change(_value);
   }

   double dial_base::value_from_point(context const& ctx, point p)
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

   void dial_base::begin_tracking(context const& ctx, info& track_info)
   {
   }

   void dial_base::keep_tracking(context const& ctx, info& track_info)
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

   void dial_base::end_tracking(context const& ctx, info& track_info)
   {
   }

   bool dial_base::scroll(context const& ctx, point dir, point p)
   {
      value(value() + dir.y * 0.005);
      ctx.view.refresh(ctx);
      return true;
   }

   view_limits basic_knob::limits(basic_context const& ctx) const
   {
      return { { _size, _size }, { _size, _size } };
   }

   void basic_knob::draw(context const& ctx)
   {
      auto& thm = get_theme();
      auto& cnv = ctx.canvas;
      auto  indicator_color = thm.indicator_color.level(1.5);
      auto  cp = circle{ center_point(ctx.bounds), ctx.bounds.width()/2 };

      draw_knob(cnv, cp, colors::black);
      draw_indicator(cnv, cp, _value, indicator_color);
   }

   void basic_knob::value(double val)
   {
      _value = val;
   }
}
