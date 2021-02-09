/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/thumbwheel.hpp>
#include <elements/element/traversal.hpp>
#include <elements/view.hpp>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // thumbwheel_base
   ////////////////////////////////////////////////////////////////////////////
   thumbwheel_base::thumbwheel_base(point init)
    : _value{ init }
   {
      clamp(_value.x, 0.0, 1.0);
      clamp(_value.y, 0.0, 1.0);
   }

   void thumbwheel_base::prepare_subject(context& ctx)
   {
      proxy_base::prepare_subject(ctx);
      if (auto* rcvr = find_subject<receiver<double>*>(this))
         rcvr->value(_value.y);
      else if (auto* rcvr = find_subject<receiver<point>*>(this))
         rcvr->value(_value);
   }

   void thumbwheel_base::value(point val)
   {
      _value.x = clamp(val.x, 0.0, 1.0);
      _value.y = clamp(val.y, 0.0, 1.0);
      if (auto* rcvr = find_subject<receiver<double>*>(this))
         rcvr->value(_value.y);
      else if (auto* rcvr = find_subject<receiver<point>*>(this))
         rcvr->value(_value);
   }

   namespace
   {
      inline void edit_value(thumbwheel_base* this_, point val)
      {
         this_->value(val);
         if (this_->on_change)
         {
            auto new_val = this_->value();
            this_->on_change(new_val);
         }
      }
   }

   void thumbwheel_base::edit(view& view_, point val)
   {
      edit_value(this, val);
      receiver<point>::notify_edit(view_);
   }

   point thumbwheel_base::compute_value(context const& /*ctx*/, tracker_info& track_info)
   {
      point delta{
         track_info.current.x - track_info.previous.x,
         track_info.current.y - track_info.previous.y
      };

      double factor = 1.0 / get_theme().dial_linear_range;
      if (track_info.modifiers & mod_shift)
         factor /= 5.0;

      float x = _value.x + factor * delta.x;
      float y = _value.y + factor * -delta.y;
      return { clamp(x, 0.0, 1.0), clamp(y, 0.0, 1.0) };
   }

   void thumbwheel_base::keep_tracking(context const& ctx, tracker_info& track_info)
   {
      if (track_info.current != track_info.previous)
      {
         auto new_value = compute_value(ctx, track_info);
         if (_value != new_value)
         {
            edit_value(this, new_value);
            ctx.view.refresh(ctx);
         }
      }
   }

   bool thumbwheel_base::scroll(context const& ctx, point dir, point p)
   {
      auto sdir = scroll_direction();
      track_scroll(ctx, dir, p);
      edit_value(this,
         {
            _value.x + (sdir.x * dir.x * 0.005f),
            _value.y - (sdir.y * dir.y * 0.005f)
         }
      );
      ctx.view.refresh(ctx);
      return true;
   }
}}
