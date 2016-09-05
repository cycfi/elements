/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/dial.hpp>
#include <photon/support/theme.hpp>
#include <photon/view.hpp>
#include <cmath>

#include <iostream>

namespace photon
{
   double const _2pi = 2 * M_PI;
   double const travel = 0.83;
   double const range = _2pi * travel;
   double const start_angle = _2pi * (1-travel)/2;

   dial::dial(widget_ptr image, double init_value)
    : _value(init_value)
    , _image(image)
   {
      clamp(_value, 0.0, 1.0);
      _image->value(_value);
   }

   rect dial::limits(basic_context const& ctx) const
   {
      return _image->limits(ctx);
   }

   void dial::draw(context const& ctx)
   {
      _image->draw(ctx);
   }

   void dial::value(double val)
   {
      _value = val;
      _image->value(_value);
   }

   double dial::value_from_point(context const& ctx, point p)
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
         double new_value = value_from_point(ctx, track_info.current);
         if (_value != new_value)
         {
            value(new_value);
            ctx.view.refresh(ctx.bounds);
         }
      }
   }

   void dial::end_tracking(context const& ctx, info& track_info)
   {
   }
}
