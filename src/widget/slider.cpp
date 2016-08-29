/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/slider.hpp>
#include <photon/support/theme.hpp>
#include <photon/view.hpp>
#include <cmath>

namespace photon
{
   rect slider::limits(basic_context const& ctx) const
   {
      auto  limits_ = body()->limits(ctx);
      auto  ind_limits = indicator()->limits(ctx);
      auto  is_horiz = limits_.right > limits_.bottom;

      if (is_horiz)
      {
         limits_.top = std::max<float>(limits_.top, ind_limits.top);
         limits_.bottom = std::max<float>(limits_.bottom, ind_limits.bottom);
         limits_.left = std::max<float>(limits_.left, ind_limits.left * 2);
         limits_.right = std::max<float>(limits_.left, limits_.right);
      }
      else
      {
         limits_.left = std::max<float>(limits_.left, ind_limits.left);
         limits_.right = std::max<float>(limits_.right, ind_limits.right);
         limits_.top = std::max<float>(limits_.top, ind_limits.top * 2);
         limits_.bottom = std::max<float>(limits_.top, limits_.bottom);
      }

      return limits_;
   }

   void slider::prepare_body(context& ctx)
   {
      auto  limits_ = body()->limits(ctx);
      auto  bounds = ctx.bounds;
      auto  w = bounds.width();
      auto  h = bounds.height();
      auto  is_horiz = w > h;

      if (is_horiz)
      {
         bounds.height(std::min<float>(limits_.bottom, h));
         ctx.bounds = center_v(bounds, ctx.bounds);
      }
      else
      {
         bounds.width(std::min<float>(limits_.right, w));
         ctx.bounds = center_h(bounds, ctx.bounds);
      }
   }

   void slider::prepare_indicator(context& ctx)
   {
      auto  bounds = ctx.bounds;
      auto  w = bounds.width();
      auto  h = bounds.height();
      auto  is_horiz = w > h;
      auto  limits_ = indicator()->limits(ctx);
      auto  ind_w = limits_.right;
      auto  ind_h = limits_.bottom;

      if (is_horiz)
      {
         bounds.height(ind_h);
         ctx.bounds = bounds.move((w - ind_w) * value(), 0);
      }
      else
      {
         bounds.width(ind_w);
         ctx.bounds = bounds.move(0, (h - ind_h) * (1.0 - value()));
         // Note: for vertical sliders, 0.0 is at the bottom, hence 1.0-value()
      }
   }

   double slider::value(context const& ctx, point p)
   {
      auto  bounds = ctx.bounds;
      auto  w = bounds.width();
      auto  h = bounds.height();
      auto  is_horiz = w > h;
      auto  limits_ = indicator()->limits(ctx);
      auto  ind_w = limits_.right;
      auto  ind_h = limits_.bottom;
      auto  new_value = 0.0;

      if (is_horiz)
         new_value = (p.x - (bounds.left + (ind_w / 2))) / (w - ind_w);
      else
         new_value = (p.y - (bounds.top + (ind_h / 2))) / (h - ind_h);

      clamp(new_value, 0.0, 1.0);
      return new_value;
   }
}
