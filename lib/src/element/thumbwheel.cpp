/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/thumbwheel.hpp>
#include <elements/element/traversal.hpp>
#include <elements/support/theme.hpp>
#include <elements/view.hpp>
#include <cmath>

#include <iostream>

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
            auto val = this_->value();
            this_->on_change(val);
         }
      }
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
         if (_value.x != new_value.x || _value.y != new_value.y)
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

   ////////////////////////////////////////////////////////////////////////////
   // basic_thumbwheel_element
   ////////////////////////////////////////////////////////////////////////////
   void basic_thumbwheel_element::value(double val)
   {
      if (_quantize > 0 && _aligner)
         _aligner(std::round(val / _quantize) * _quantize);
      else
         align(val);
   }

   void basic_thumbwheel_element::make_aligner(context const& ctx)
   {
      if (_quantize > 0)
      {
         _aligner =
            [this, &view = ctx.view, bounds = ctx.bounds](double val)
            {
               view.post(
                  [this, &view, val, bounds]()
                  {
                     do_align(view, bounds, val);
                  }
               );
            };
      }
   }

   void basic_thumbwheel_element::do_align(view& view_, rect const& bounds, double val)
   {
      auto curr = align();
      auto diff = val - curr;
      if (std::abs(diff) < (_quantize / 10))
      {
         align(val);
         if (diff > 0)
            view_.refresh(bounds);
      }
      else
      {
         align(curr + diff/10);
         view_.refresh(bounds);
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   // basic_vthumbwheel_element
   ////////////////////////////////////////////////////////////////////////////
   view_limits basic_vthumbwheel_element::limits(basic_context const& ctx) const
   {
      auto lim = port_base::limits(ctx);
      auto num_elements = (1.0 / quantize()) + 1;
      lim.min.y /= num_elements;
      lim.max.y = lim.min.y;
      return lim;
   }

   void basic_vthumbwheel_element::draw(context const& ctx)
   {
      make_aligner(ctx);
      vport_element::draw(ctx);
   }

   void basic_vthumbwheel_element::align(double val)
   {
      clamp(val, 0.0, 1.0);
      valign(val);
   }

   double basic_vthumbwheel_element::align() const
   {
      return valign();
   }

   ////////////////////////////////////////////////////////////////////////////
   // basic_hthumbwheel_element
   ////////////////////////////////////////////////////////////////////////////
   view_limits basic_hthumbwheel_element::limits(basic_context const& ctx) const
   {
      auto lim = port_base::limits(ctx);
      lim.min.x *= quantize();
      lim.max.x = lim.min.x;
      return lim;
   }

   void basic_hthumbwheel_element::draw(context const& ctx)
   {
      make_aligner(ctx);
      hport_element::draw(ctx);
   }

   void basic_hthumbwheel_element::align(double val)
   {
      clamp(val, 0.0, 1.0);
      halign(val);
   }

   double basic_hthumbwheel_element::align() const
   {
      return halign();
   }
}}
