/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/dial.hpp>
#include <elements/element/traversal.hpp>
#include <elements/support/theme.hpp>
#include <elements/view.hpp>
#include <cmath>

#include <iostream>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // dial_base
   ////////////////////////////////////////////////////////////////////////////
   dial_base::dial_base(double init_value)
    : _value(init_value)
   {
      clamp(_value, 0.0, 1.0);
   }

   void dial_base::prepare_subject(context& ctx)
   {
      proxy_base::prepare_subject(ctx);
      if (auto* rcvr = find_subject<receiver<double>*>(this))
         rcvr->value(_value);
   }

   void dial_base::value(double val)
   {
      _value = clamp(val, 0.0, 1.0);
      if (auto* rcvr = find_subject<receiver<double>*>(this))
         rcvr->value(_value);
   }

   namespace
   {
      inline void edit_value(dial_base* this_, double val)
      {
         this_->value(val);
         if (this_->on_change)
            this_->on_change(this_->value());
      }
   }

   double dial_base::radial_value(context const& ctx, tracker_info& track_info)
   {
      using namespace radial_consts;

      point p = track_info.current;
      point center = center_point(ctx.bounds);
      double angle = -std::atan2(p.x-center.x, p.y-center.y);
      if (angle < 0.0)
         angle += _2pi;

      float val = (angle-start_angle) / range;
      if (std::abs(val - value()) < 0.6)
         return clamp(val, 0.0, 1.0);
      return value();
   }

   double dial_base::linear_value(context const& /*ctx*/, tracker_info& track_info)
   {
      point delta{
         track_info.current.x - track_info.previous.x,
         track_info.current.y - track_info.previous.y
      };

      double factor = 1.0 / get_theme().dial_linear_range;
      if (track_info.modifiers & mod_shift)
         factor /= 5.0;

      float val = _value + factor * (delta.x - delta.y);
      return clamp(val, 0.0, 1.0);
   }

   double dial_base::compute_value(context const& ctx, tracker_info& track_info)
   {
      return (get_theme().dial_mode == dial_mode_enum::radial)?
         radial_value(ctx, track_info) :
         linear_value(ctx, track_info)
         ;
   }

   void dial_base::begin_tracking(context const& /* ctx */, tracker_info& /* track_info */)
   {
   }

   void dial_base::keep_tracking(context const& ctx, tracker_info& track_info)
   {
      if (track_info.current != track_info.previous)
      {
         double new_value = compute_value(ctx, track_info);
         if (_value != new_value)
         {
            edit_value(this, new_value);
            ctx.view.refresh(ctx);
         }
      }
   }

   void dial_base::end_tracking(context const& /* ctx */, tracker_info& /* track_info */)
   {
   }

   bool dial_base::scroll(context const& ctx, point dir, point p)
   {
      auto sdir = scroll_direction();
      track_scroll(ctx, dir, p);
      edit_value(this, value()
         + (-sdir.y * dir.y * 0.005)
         + (sdir.x * dir.x * 0.005)
      );
      ctx.view.refresh(ctx);
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////
   // thumbwheel_base
   ////////////////////////////////////////////////////////////////////////////
   double thumbwheel_base::compute_value(context const& ctx, tracker_info& track_info)
   {
      return linear_value(ctx, track_info);
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
