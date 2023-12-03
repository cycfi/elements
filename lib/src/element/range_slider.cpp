/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman 
   Copyright (c) 2023 Kristian Lytje

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/range_slider.hpp>
#include <elements/support/theme.hpp>
#include <elements/view.hpp>
#include <cmath>

namespace cycfi { namespace elements
{
   view_limits range_slider_base::limits(basic_context const& ctx) const
   {
      auto  limits_ = track().limits(ctx);
      auto  tmb_limit1 = thumb().first.get().limits(ctx);
      auto  tmb_limit2 = thumb().second.get().limits(ctx);

      // We multiply thumb min limits by 2 so that there is always some space to move it.
      if (_is_horiz = limits_.max.x > limits_.max.y; _is_horiz)
      {
         limits_.min.y = std::max<float>({limits_.min.y, tmb_limit1.min.y, tmb_limit2.min.y});
         limits_.max.y = std::max<float>({limits_.max.y, tmb_limit1.max.y, tmb_limit2.max.y});
         limits_.min.x = std::max<float>({limits_.min.x, tmb_limit1.min.x * 2, tmb_limit2.min.x * 2});
      }
      else
      {
         limits_.min.x = std::max<float>({limits_.min.x, tmb_limit1.min.x, tmb_limit2.min.x});
         limits_.max.x = std::max<float>({limits_.max.x, tmb_limit1.max.x, tmb_limit2.max.x});
         limits_.min.y = std::max<float>({limits_.min.y, tmb_limit1.min.y * 2, tmb_limit2.min.y * 2});
      }

      return limits_;
   }

   void range_slider_base::layout(context const& ctx)
   {
      {
         context sctx { ctx, &track(), ctx.bounds };
         sctx.bounds = track_bounds(sctx);
         track().layout(sctx);
      }
      auto bounds = thumb_bounds(ctx);
      auto thumbs = thumb();
      {
         context sctx { ctx, &thumbs.first.get(), ctx.bounds };
         sctx.bounds = bounds.first;
         thumbs.first.get().layout(sctx);
      }
      {
         context sctx { ctx, &thumbs.second.get(), ctx.bounds };
         sctx.bounds = bounds.second;
         thumbs.second.get().layout(sctx);
      }
   }

   void range_slider_base::draw(context const& ctx)
   {
      if (intersects(ctx.bounds, ctx.view_bounds()))
      {
         {
            context sctx { ctx, &track(), ctx.bounds };
            sctx.bounds = track_bounds(sctx);
            track().draw(sctx);
         }
         auto bounds = thumb_bounds(ctx);
         auto thumbs = thumb();
         {
            context sctx { ctx, &thumbs.first.get(), ctx.bounds };
            sctx.bounds = bounds.first;
            thumbs.first.get().draw(sctx);
         }
         {
            context sctx { ctx, &thumbs.second.get(), ctx.bounds };
            sctx.bounds = bounds.second;
            thumbs.second.get().draw(sctx);
         }
      }
   }

   rect range_slider_base::track_bounds(context const& ctx) const
   {
      auto  limits_ = track().limits(ctx);
      auto  bounds = ctx.bounds;
      auto  th_bounds = thumb_bounds(ctx);

      if (_is_horiz)
      {
         bounds.height(std::min<float>(limits_.max.y, bounds.height()));
         auto w = std::max(th_bounds.first.width(), th_bounds.second.width())*0.5;
         bounds.left += w;
         bounds.right -= w;
         bounds = center_v(bounds, ctx.bounds);
      }
      else
      {
         bounds.width(std::min<float>(limits_.max.x, bounds.width()));
         auto h = std::max(th_bounds.first.height(), th_bounds.second.height())*0.5;
         bounds.top += h;
         bounds.bottom -= h;
         bounds = center_h(bounds, ctx.bounds);
      }
      return bounds;
   }

   std::pair<rect, rect> range_slider_base::thumb_bounds(context const& ctx) const
   {
      auto get_single_bound = [this] (context const& ctx, auto const& thumb, double value) {
         auto  bounds = ctx.bounds;
         auto  w = bounds.width();
         auto  h = bounds.height();
         auto  limits_ = thumb.limits(ctx);
         auto  tmb_w = limits_.max.x;
         auto  tmb_h = limits_.max.y;

         if (_is_horiz)
         {
            bounds.width(tmb_w);
            return bounds.move((w - tmb_w) * value, 0);
         }
         else
         {
            bounds.height(tmb_h);
            return bounds.move(0, (h - tmb_h) * (1.0 - value));
            // Note: for vertical sliders, 0.0 is at the bottom, hence 1.0-value()
         }
      };
      auto vals = value();
      return std::make_pair(get_single_bound(ctx, thumb().first.get(), vals.first), get_single_bound(ctx, thumb().second.get(), vals.second));
   }

   inline auto value_from_point = [] (context const& ctx, point p, auto const& thumb, bool _is_horiz)
   {
      auto  bounds = ctx.bounds;
      auto  w = bounds.width();
      auto  h = bounds.height();

      auto  limits_ = thumb.limits(ctx); 
      auto  tmb_w = limits_.max.x;
      auto  tmb_h = limits_.max.y;
      auto  new_value = 0.0;

      // Note: for vertical sliders, 0.0 is at the bottom, hence 1.0-computed_value
      if (_is_horiz)
         new_value = (p.x - (bounds.left + (tmb_w / 2))) / (w - tmb_w);
      else
         new_value = 1.0 - ((p.y - (bounds.top + (tmb_h / 2))) / (h - tmb_h));
      return clamp(new_value, 0.0, 1.0);
   };

   void range_slider_base::begin_tracking(context const& ctx, tracker_info& track_info)
   {
      auto tmb_bounds = thumb_bounds(ctx); 
      if (tmb_bounds.first.includes(track_info.current))
      {
         auto cp = center_point(tmb_bounds.first);
         track_info.offset.x = track_info.current.x - cp.x;
         track_info.offset.y = track_info.current.y - cp.y;
         this->_active_thumb = 0;
      }
      else if (tmb_bounds.second.includes(track_info.current))
      {
         auto cp = center_point(tmb_bounds.second);
         track_info.offset.x = track_info.current.x - cp.x;
         track_info.offset.y = track_info.current.y - cp.y;
         this->_active_thumb = 1;
      }
   }

   void range_slider_base::keep_tracking(context const& ctx, tracker_info& track_info)
   {
      auto th_bounds = thumb_bounds(ctx);
      auto tr_bounds = track_bounds(ctx);
      auto th_w = (th_bounds.first.width() + th_bounds.second.width())*0.5/tr_bounds.width();
      if (track_info.current != track_info.previous)
      {
         switch (_active_thumb)
         {
            case 0: {
               auto new_value = value_from_point(ctx, track_info.current, thumb().first.get(), _is_horiz);
               new_value = std::min(new_value, _value.second - th_w);
               if (_value.first != new_value)
               {
                  edit_value({new_value, _value.second});
                  ctx.view.refresh(ctx);
               }
               break;
            }
            case 1: {
               auto new_value = value_from_point(ctx, track_info.current, thumb().second.get(), _is_horiz);
               new_value = std::max(new_value, _value.first + th_w);
               if (_value.second != new_value)
               {
                  edit_value({_value.first, new_value});
                  ctx.view.refresh(ctx);
               }
               break;
            }
         }
      }
   }

   void range_slider_base::end_tracking(context const& ctx, tracker_info& track_info)
   {
      auto th_bounds = thumb_bounds(ctx);
      auto tr_bounds = track_bounds(ctx);
      auto th_w = (th_bounds.first.width() + th_bounds.second.width())*0.5/tr_bounds.width();
      switch (_active_thumb)
      {
         case 0: {
            auto new_value = value_from_point(ctx, track_info.current, thumb().first.get(), _is_horiz);
            new_value = std::min(new_value, _value.second - th_w);
            if (_value.first != new_value)
            {
               edit_value({new_value, _value.second});
               ctx.view.refresh(ctx);
            }
            break;
         }
         case 1: {
            auto new_value = value_from_point(ctx, track_info.current, thumb().second.get(), _is_horiz);
            new_value = std::max(new_value, _value.first + th_w);
            if (_value.second != new_value)
            {
               edit_value({_value.first, new_value});
               ctx.view.refresh(ctx);
            }
            break;
         }
      }
      _active_thumb = -1;
   }

   void range_slider_base::value(std::pair<double, double> val)
   {
      _value = {clamp(val.first, 0.0, 1.0), clamp(val.second, 0.0, 1.0)};
   }

   std::pair<double, double> range_slider_base::value() const
   {
      return _value;
   }

   void basic_range_slider_base::edit_value_first(double val)
   {
      range_slider_base::edit_value_first(val);
      if (on_change.first)
         on_change.first(val);
   }

   void basic_range_slider_base::edit_value_second(double val)
   {
      range_slider_base::edit_value_second(val);
      if (on_change.second)
         on_change.second(val);
   }
}}