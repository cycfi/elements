/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman
   Copyright (c) 2023 Kristian Lytje

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/range_slider.hpp>
#include <elements/support/theme.hpp>
#include <elements/view.hpp>
#include <cmath>

namespace cycfi::elements
{
   view_limits range_slider_base::limits(basic_context const& ctx) const
   {
      auto  limits_ = track().limits(ctx);
      auto  tmb_limit1 = thumb().first.get().limits(ctx);
      auto  tmb_limit2 = thumb().second.get().limits(ctx);

      // We multiply thumb min limits by 2 so that there is always some space to move it.
      if (_is_horiz = limits_.max.x > limits_.max.y; _is_horiz)
      {
         limits_.min.y = std::max<float>(
            {limits_.min.y, tmb_limit1.min.y, tmb_limit2.min.y}
         );
         limits_.max.y = std::max<float>(
            {limits_.max.y, tmb_limit1.max.y, tmb_limit2.max.y}
         );
         limits_.min.x = std::max<float>(
            {limits_.min.x, tmb_limit1.min.x * 2, tmb_limit2.min.x * 2}
         );
      }
      else
      {
         limits_.min.x = std::max<float>(
            {limits_.min.x, tmb_limit1.min.x, tmb_limit2.min.x}
         );
         limits_.max.x = std::max<float>(
            {limits_.max.x, tmb_limit1.max.x, tmb_limit2.max.x}
         );
         limits_.min.y = std::max<float>(
            {limits_.min.y, tmb_limit1.min.y * 2, tmb_limit2.min.y * 2}
         );
      }

      return limits_;
   }

   void range_slider_base::layout(context const& ctx)
   {
      {
         context sctx {ctx, &track(), ctx.bounds};
         sctx.bounds = track_bounds(sctx);
         track().layout(sctx);
      }
      auto bounds = thumb_bounds(ctx);
      auto thumbs = thumb();
      {
         context sctx {ctx, &thumbs.first.get(), ctx.bounds};
         sctx.bounds = bounds.first;
         thumbs.first.get().layout(sctx);
      }
      {
         context sctx {ctx, &thumbs.second.get(), ctx.bounds};
         sctx.bounds = bounds.second;
         thumbs.second.get().layout(sctx);
      }
   }

   void range_slider_base::draw(context const& ctx)
   {
      if (intersects(ctx.bounds, ctx.view_bounds()))
      {
         {
            context sctx {ctx, &track(), ctx.bounds};
            sctx.bounds = track_bounds(sctx);
            track().draw(sctx);
         }
         auto bounds = thumb_bounds(ctx);
         auto thumbs = thumb();

         auto draw_first = [&]()
         {
            context sctx {ctx, &thumbs.first.get(), ctx.bounds};
            sctx.bounds = bounds.first;
            thumbs.first.get().draw(sctx);
         };

         auto draw_second = [&]()
         {
            context sctx {ctx, &thumbs.second.get(), ctx.bounds};
            sctx.bounds = bounds.second;
            thumbs.second.get().draw(sctx);
         };

         switch (_state)
         {
            case state::idle_1:
            case state::moving_thumb1:
            case state::scrolling_thumb1:
            {
               draw_second();
               draw_first();
               break;
            }
            default:
            {
               draw_first();
               draw_second();
               break;
            }
         }
      }
   }

   float range_slider_base::handle_possible_collision_first(context const& ctx, float val) const
   {
      auto th_bounds = thumb_bounds(ctx);
      auto tr_bounds = track_bounds(ctx);
      auto th_w = (th_bounds.first.width() + th_bounds.second.width())*0.5/tr_bounds.width();
      return std::min<float>(val, _value.second - th_w);
   }

   float range_slider_base::handle_possible_collision_second(context const& ctx, float val) const
   {
      auto th_bounds = thumb_bounds(ctx);
      auto tr_bounds = track_bounds(ctx);
      auto th_w = (th_bounds.first.width() + th_bounds.second.width())*0.5/tr_bounds.width();
      return std::max<float>(val, _value.first + th_w);
   }

   // since we only have a single scroll function to work with, we do it as follows:
   // 1. if no thumbs are active, determine which is closest to the mouse
   //    i. store the initial mouse position
   //    ii. scroll the thumb
   // 2. if a thumb is active:
   //    i. check if we are close to the initial mouse position
   //    iia. if so, scroll the thumb
   //    iib. if not, we assume it is a new separate scroll action and go back to 1.
   bool range_slider_base::scroll(context const& ctx, point dir, point p)
   {
      static point start_p = p;
      auto th_bounds = thumb_bounds(ctx);

      auto scroll_thumb1 = [this, &ctx, &dir, &p]()
      {
         auto sdir = scroll_direction();
         auto new_value = value_first() + (_is_horiz ? dir.x*sdir.x + !dir.x*dir.y*-sdir.y : dir.y*-sdir.y) * 0.005;
         new_value = handle_possible_collision_first(ctx, new_value);
         clamp(new_value, 0.0, 1.0);
         track_scroll(ctx, dir, p);
         edit_value_first(new_value);
      };

      auto scroll_thumb2 = [this, &ctx, &dir, &p]()
      {
         auto sdir = scroll_direction();
         auto new_value = value_second() + (_is_horiz ? dir.x*sdir.x  + !dir.x*dir.y*-sdir.y : dir.y*-sdir.y) * 0.005;
         new_value = handle_possible_collision_second(ctx, new_value);
         clamp(new_value, 0.0, 1.0);
         track_scroll(ctx, dir, p);
         edit_value_second(new_value);
      };

      // check if mouse has moved too far
      if (81 < std::pow(start_p.x - p.x, 2) + std::pow(start_p.y - p.y, 2))
      {
         switch (_state)
         {
            case state::scrolling_thumb2: {
               _state = state::idle_2;
               break;
            }
            default: {
               _state = state::idle_1;
               break;
            }
         }
      }

      switch (_state)
      {
         case state::scrolling_thumb1:
         {
            scroll_thumb1();
            break;
         }

         case state::scrolling_thumb2:
         {
            scroll_thumb2();
            break;
         }

         default:
         {
            start_p = p;
            auto& th1 = th_bounds.first;
            auto& th2 = th_bounds.second;
            auto th1_w = th1.width()*0.5f;
            auto th1_h = th1.height()*0.5f;
            auto th2_w = th2.width()*0.5f;
            auto th2_h = th2.height()*0.5f;

            auto th1_upscaled_bounds = rect{th1.left-th1_w, th1.top-th1_h, th1.right+th1_w, th1.bottom+th1_h};
            auto th2_upscaled_bounds = rect{th2.left-th2_w, th2.top-th2_h, th2.right+th2_w, th2.bottom+th2_h};
            if (th1_upscaled_bounds.includes(p))
            {
               _state = state::scrolling_thumb1;
               scroll_thumb1();
               return true;
            }
            if (th2_upscaled_bounds.includes(p))
            {
               _state = state::scrolling_thumb2;
               scroll_thumb2();
               return true;
            }
            return false;
         }
      }

      ctx.view.refresh(ctx);
      return true;
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
      auto get_single_bound = [this] (context const& ctx, auto const& thumb, double value)
      {
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
      return std::make_pair(
         get_single_bound(ctx, thumb().first.get(), value_first()),
         get_single_bound(ctx, thumb().second.get(), value_second())
      );
   }

   inline auto value_from_point = [](context const& ctx, point p, auto const& thumb, bool _is_horiz)
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

      bool overlaps_thumb1 = tmb_bounds.first.includes(track_info.current);
      bool overlaps_thumb2 = tmb_bounds.second.includes(track_info.current);

      auto begin_tracking_1 = [&]()
      {
         auto cp = center_point(tmb_bounds.first);
         track_info.offset.x = track_info.current.x - cp.x;
         track_info.offset.y = track_info.current.y - cp.y;
         _state = state::moving_thumb1;
      };

      auto begin_tracking_2 = [&]()
      {
         auto cp = center_point(tmb_bounds.second);
         track_info.offset.x = track_info.current.x - cp.x;
         track_info.offset.y = track_info.current.y - cp.y;
         _state = state::moving_thumb2;
      };

      if (track_info.modifiers == mod_alt)
      {
         _state = _state == state::idle_1 ? state::idle_2 : state::idle_1;
         ctx.view.refresh(ctx); // visually update which thumb is prioritized even if we don't move it
      }

      // the current state determines which thumb is checked first
      switch (_state)
      {
         case state::idle_2:
         {
            if (overlaps_thumb2)
            {
               begin_tracking_2();
               return;
            }
            else if (overlaps_thumb1)
            {
               begin_tracking_1();
               return;
            }
            return;
         }

         default:
         {
            if (overlaps_thumb1)
            {
               begin_tracking_1();
               return;
            }
            else if (overlaps_thumb2)
            {
               begin_tracking_2();
               return;
            }
         }
      }
   }

   void range_slider_base::move_first(context const& ctx, tracker_info& track_info)
   {
      auto new_value = value_from_point(ctx, track_info.current, thumb().first.get(), _is_horiz);
      new_value = handle_possible_collision_first(ctx, new_value);
      if (_value.first != new_value)
      {
         edit_value_first(new_value);
         ctx.view.refresh(ctx);
      }
   }

   void range_slider_base::move_second(context const& ctx, tracker_info& track_info)
   {
      auto new_value = value_from_point(ctx, track_info.current, thumb().second.get(), _is_horiz);
      new_value = handle_possible_collision_second(ctx, new_value);
      if (_value.second != new_value)
      {
         edit_value_second(new_value);
         ctx.view.refresh(ctx);
      }
   }

   void range_slider_base::move_both_from_first(context const& ctx, tracker_info& track_info)
   {
      auto new_value = value_from_point(ctx, track_info.current, thumb().first.get(), _is_horiz);
      auto deltax = new_value - _value.first;
      if (1 < _value.second + deltax)
      {
         deltax = 1 - _value.second;
      }
      edit_value({_value.first + deltax, _value.second + deltax});
      ctx.view.refresh(ctx);
   }

   void range_slider_base::move_both_from_second(context const& ctx, tracker_info& track_info)
   {
      auto new_value = value_from_point(ctx, track_info.current, thumb().second.get(), _is_horiz);
      auto deltax = new_value - _value.second;
      if (_value.first + deltax < 0)
      {
         deltax = -_value.first;
      }
      edit_value({_value.first + deltax, _value.second + deltax});
      ctx.view.refresh(ctx);
   }

   void range_slider_base::keep_tracking(context const& ctx, tracker_info& track_info)
   {
      if (track_info.current != track_info.previous)
      {
         // first check if both should be moved
         if (track_info.modifiers == mod_shift)
         {
            switch (_state)
            {
               case state::moving_thumb1:
               {
                  move_both_from_first(ctx, track_info);
                  return;
               }

               case state::moving_thumb2:
               {
                  move_both_from_second(ctx, track_info);
                  return;
               }

               default:
                  return;
            }
         }

         // otherwise just move one
         switch (_state)
         {
            case state::moving_thumb1:
            {
               move_first(ctx, track_info);
               return;
            }

            case state::moving_thumb2:
            {
               move_second(ctx, track_info);
               return;
            }

            default:
               return;
         }
      }
   }

   void range_slider_base::end_tracking(context const& ctx, tracker_info& track_info)
   {
      // first check if both should be moved
      if (track_info.modifiers == mod_shift)
      {
         switch (_state)
         {
            case state::moving_thumb1:
            {
               move_both_from_first(ctx, track_info);
               _state = state::idle_1;
               return;
            }

            case state::moving_thumb2:
            {
               move_both_from_second(ctx, track_info);
               _state = state::idle_2;
               return;
            }

            default:
               return;
         }
      }

      // otherwise just move one
      switch (_state)
      {
         case state::moving_thumb1:
         {
            move_first(ctx, track_info);
            _state = state::idle_1;
            return;
         }

         case state::moving_thumb2:
         {
            move_second(ctx, track_info);
            _state = state::idle_2;
            return;
         }

         default:
            return;
      }
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

   float basic_range_slider_offset_base::handle_possible_collision_first(context const& ctx, float val) const
   {
      auto th_bounds = thumb_bounds(ctx);
      auto tr_bounds = track_bounds(ctx);
      auto th_w = (th_bounds.first.width() + th_bounds.second.width())*(0.5-offset_from_center)/tr_bounds.width();
      return std::min<float>(val, value_second() - th_w);
   }

   float basic_range_slider_offset_base::handle_possible_collision_second(context const& ctx, float val) const
   {
      auto th_bounds = thumb_bounds(ctx);
      auto tr_bounds = track_bounds(ctx);
      auto th_w = (th_bounds.first.width() + th_bounds.second.width())*(0.5-offset_from_center)/tr_bounds.width();
      return std::max<float>(val, value_first() + th_w);
   }
}