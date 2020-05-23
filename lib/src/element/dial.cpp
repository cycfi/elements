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

   double dial_base::value_from_point(context const& ctx, point p)
   {
      using namespace radial_consts;

      point center = center_point(ctx.bounds);
      double angle = -std::atan2(p.x-center.x, p.y-center.y);
      if (angle < 0.0)
         angle += _2pi;

      float val = (angle-start_angle) / range;
      if (std::abs(val - value()) < 0.6)
         return clamp(val, 0.0, 1.0);
      return value();
   }

   void dial_base::begin_tracking(context const& /* ctx */, tracker_info& /* track_info */)
   {
   }

   void dial_base::keep_tracking(context const& ctx, tracker_info& track_info)
   {
      if (track_info.current != track_info.previous)
      {
         double new_value = value_from_point(ctx, track_info.current);
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
      track_scroll(ctx, dir, p);
      edit_value(this, value() + dir.y * 0.005);
      ctx.view.refresh(ctx);
      return true;
   }

   void draw_indicator(canvas& cnv, circle cp, float val, color c)
   {
      constexpr float w_factor = 0.1;  // relative width of the indicator
      constexpr float h_factor = 0.2;  // relative height of the indicator
      using namespace radial_consts;

      auto state = cnv.new_state();
      auto center = cp.center();
      cnv.translate({ center.x, center.y });
      cnv.rotate(offset + (val * range));

      float r = cp.radius * 0.85;
      float ind_w = r * w_factor;
      float ind_h = r * h_factor;
      rect  ind_r = { -ind_w, -ind_h, ind_w, ind_h };
      ind_r = ind_r.move(0, r*0.6);

      draw_indicator(cnv, ind_r, c);
   }

   void draw_radial_marks(canvas& cnv, circle cp, float size, color c)
   {
      using namespace radial_consts;
      auto state = cnv.new_state();
      auto center = cp.center();
      constexpr auto num_divs = 50;
      float div = range / num_divs;
      auto const& theme = get_theme();

      cnv.translate({ center.x, center.y });
      cnv.stroke_style(theme.ticks_color);
      for (int i = 0; i != num_divs+1; ++i)
      {
         float from = cp.radius;
         if (i % (num_divs / 10))
         {
            // Minor ticks
            from -= size / 4;
            cnv.line_width(theme.minor_ticks_width);
            cnv.stroke_style(c.level(theme.minor_ticks_level));
         }
         else
         {
            // Major ticks
            cnv.line_width(theme.major_ticks_width);
            cnv.stroke_style(c.level(theme.major_ticks_level));
         }

         float angle = offset + (M_PI / 2) + (i * div);
         float sin_ = std::sin(angle);
         float cos_ = std::cos(angle);
         float to = cp.radius - (size / 2);

         cnv.move_to({ from * cos_, from * sin_ });
         cnv.line_to({ to * cos_, to * sin_ });
         cnv.stroke();
      }
   }

   void draw_radial_labels(
      canvas& cnv
    , circle cp
    , float /* size */
    , float font_size
    , std::string const labels[]
    , std::size_t num_labels
   )
   {
      if (num_labels < 2)
         return; // Nothing to do

      using namespace radial_consts;
      auto state = cnv.new_state();
      auto center = cp.center();
      float div = range / (num_labels-1);
      auto const& theme = get_theme();

      cnv.translate({ center.x, center.y });
      cnv.text_align(cnv.middle | cnv.center);
      cnv.fill_style(theme.label_font_color);

      cnv.font(
         theme.label_font,
         theme.label_font_size * font_size
      );

      for (std::size_t i = 0; i != num_labels; ++i)
      {
         float angle = offset + (M_PI / 2) + (i * div);
         float sin_ = std::sin(angle);
         float cos_ = std::cos(angle);

         cnv.fill_text({ cp.radius * cos_, cp.radius * sin_ }, labels[i].c_str());
      }
   }
}}
