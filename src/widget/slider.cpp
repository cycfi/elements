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
   slider::slider(widget_ptr indicator, widget_ptr body, double init_value)
    : _indicator(indicator)
    , _body(body)
    , _value(init_value)
    , _is_horiz(false)
   {}

   rect slider::limits(basic_context const& ctx) const
   {
      auto  limits_ = body()->limits(ctx);
      auto  ind_limits = indicator()->limits(ctx);

      if ((_is_horiz = limits_.right > limits_.bottom))
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

   void slider::draw(context const& ctx)
   {
      if (intersects(ctx.bounds, ctx.view.dirty()))
      {
         {
            auto  limits_ = body()->limits(ctx);
            auto  bounds = ctx.bounds;

            if (_is_horiz)
            {
               auto h = bounds.height();
               bounds.height(std::min<float>(limits_.bottom, h));
               bounds = center_v(bounds, ctx.bounds);
            }
            else
            {
               auto w = bounds.width();
               bounds.width(std::min<float>(limits_.right, w));
               bounds = center_h(bounds, ctx.bounds);
            }

            context sctx { ctx, _body.get(), bounds };
            _body->draw(sctx);
         }
         {
            context sctx { ctx, _indicator.get(), ctx.bounds };
            sctx.bounds = indicator_bounds(sctx);
            _indicator->draw(sctx);
         }
      }
   }

   bool slider::scroll(context const& ctx, point p)
   {
      double new_value = value() + ((ctx.bounds.width() < ctx.bounds.height()) ? p.y : -p.x) * 0.01;
      clamp(new_value, 0.0, 1.0);
      if (value() != new_value)
      {
         value(new_value);
         ctx.view.refresh(ctx.bounds);
         return true;
      }
      return false;
   }

   rect slider::indicator_bounds(context const& ctx) const
   {
      auto  bounds = ctx.bounds;
      auto  w = bounds.width();
      auto  h = bounds.height();
      auto  limits_ = indicator()->limits(ctx);
      auto  ind_w = limits_.right;
      auto  ind_h = limits_.bottom;

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

   double slider::value_from_point(context const& ctx, point p)
   {
      auto  bounds = ctx.bounds;
      auto  w = bounds.width();
      auto  h = bounds.height();

      auto  limits_ = indicator()->limits(ctx);
      auto  ind_w = limits_.right;
      auto  ind_h = limits_.bottom;
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

   void slider::begin_tracking(context const& ctx, info& track_info)
   {
      auto ind_bounds = indicator_bounds(ctx);
      if (ind_bounds.includes(track_info.current))
      {
         auto cp = center_point(ind_bounds);
         track_info.offset.x = track_info.current.x - cp.x;
         track_info.offset.y = track_info.current.y - cp.y;
      }
   }

   void slider::keep_tracking(context const& ctx, info& track_info)
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

   void slider::end_tracking(context const& ctx, info& track_info)
   {
      double new_value = value_from_point(ctx, track_info.current);
      if (_value != new_value)
      {
         value(new_value);
         ctx.view.refresh(ctx.bounds);
      }
   }
}
