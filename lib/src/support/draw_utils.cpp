/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/support/draw_utils.hpp>
#include <elements/support/theme.hpp>

namespace cycfi { namespace elements
{
   void draw_box_vgradient(canvas& cnv, rect bounds, float corner_radius)
   {
      auto gradient = canvas::linear_gradient{
         bounds.top_left(),
         bounds.bottom_left()
      };

      gradient.add_color_stop({ 0.0f, rgba(255, 255, 255, 16) });
      gradient.add_color_stop({ 0.8f, rgba(0, 0, 0, 16) });
      cnv.fill_style(gradient);

      cnv.begin_path();
      cnv.round_rect(bounds, corner_radius);
      cnv.fill();

      cnv.begin_path();
      cnv.move_to(point{ bounds.left+0.5f, bounds.bottom-0.5f });
      cnv.line_to(point{ bounds.right-0.5f, bounds.bottom-0.5f });
      cnv.stroke_style(rgba(0, 0, 0, 32));
      cnv.line_width(1);
      cnv.stroke();
   }

   void draw_panel(canvas& cnv, rect bounds, color c, float corner_radius)
   {
      // Panel fill
      cnv.begin_path();
      cnv.round_rect(bounds, corner_radius);
      cnv.fill_style(c);
      cnv.fill();

      // Simulated blurred shadow. cairo does not have blur yet :-(
      {
         auto save = cnv.new_state();

         cnv.begin_path();
         cnv.rect(bounds.inset(-100, -100));
         cnv.round_rect(bounds.inset(0.5, 0.5), corner_radius);
         cnv.fill_rule(canvas::fill_odd_even);
         cnv.clip();

         rect shr = bounds;
         shr.left -= 2;
         shr.top -= 2;
         shr.right += 6;
         shr.bottom += 6;
         cnv.begin_path();
         cnv.round_rect(shr, corner_radius*2);
         cnv.fill_style(rgba(0, 0, 0, 20));
         cnv.fill();

         shr.left += 1;
         shr.top += 1;
         shr.right -= 2;
         shr.bottom -= 2;
         cnv.begin_path();
         cnv.round_rect(shr, corner_radius*1.5);
         cnv.fill_style(rgba(0, 0, 0, 30));
         cnv.fill();

         shr.left += 1;
         shr.top += 1;
         shr.right -= 2;
         shr.bottom -= 2;
         cnv.begin_path();
         cnv.round_rect(shr, corner_radius);
         cnv.fill_style(rgba(0, 0, 0, 40));
         cnv.fill();
      }
   }

   void draw_button(canvas& cnv, rect bounds, color c, float corner_radius)
   {
      auto gradient = canvas::linear_gradient{
         bounds.top_left(),
         bounds.bottom_left()
      };

      float const box_opacity = get_theme().element_background_opacity;
      gradient.add_color_stop({ 0.0, rgb(255, 255, 255).opacity(box_opacity) });
      gradient.add_color_stop({ 1.0, rgb(0, 0, 0).opacity(box_opacity) });
      cnv.fill_style(gradient);

      cnv.begin_path();
      cnv.round_rect(bounds.inset(1, 1), corner_radius-1);
      cnv.fill_style(c);
      cnv.fill();
      cnv.round_rect(bounds.inset(1, 1), corner_radius-1);
      cnv.fill_style(gradient);
      cnv.fill();

      cnv.begin_path();
      cnv.round_rect(bounds.inset(0.5, 0.5), corner_radius-0.5);
      cnv.stroke_style(rgba(0, 0, 0, 48));
      cnv.stroke();
   }

   void draw_knob(canvas& cnv, circle cp, color c)
   {
      auto state = cnv.new_state();
      float radius = cp.radius * 0.85;
      float inset = cp.radius * 0.15;

      // Draw beveled knob
      {
         auto gradient = canvas::radial_gradient{
            { cp.cx, cp.cy }, radius*0.75f,
            { cp.cx, cp.cy }, radius
         };

         gradient.add_color_stop({ 0.0, c });
         gradient.add_color_stop({ 0.5, c.opacity(0.5) });
         gradient.add_color_stop({ 1.0, c.level(0.5).opacity(0.5) });

         cnv.fill_style(gradient);
         cnv.begin_path();
         cnv.circle(cp.inset(inset));
         cnv.fill();
      }

      // Draw some 3D highlight
      {
         auto hcp = cp.center().move(-radius, -radius);
         auto gradient = canvas::radial_gradient{
            hcp, radius*0.5f,
            hcp, radius*2
         };

		 using cs = canvas::color_stop;
         gradient.add_color_stop(cs{ 0.0f, { 1.0f, 1.0f, 1.0f, 0.4f } });
         gradient.add_color_stop(cs{ 1.0f, { 0.6f, 0.6f, 0.6f, 0.0f } });

         cnv.fill_style(gradient);
         cnv.begin_path();
         cnv.circle(cp.inset(inset));
         cnv.fill();
      }

      // Draw the outline
      {
         cnv.stroke_style(colors::black.opacity(0.1));
         cnv.circle(cp.inset(inset));
         cnv.line_width(radius/30);
         cnv.stroke();
      }

      // Draw knob rim
      {
         cnv.begin_path();
         cnv.circle(cp);
         cnv.circle(cp.inset(inset));
         cnv.fill_rule(canvas::fill_odd_even);
         cnv.clip();

         auto bounds = cp.bounds();
         auto gradient = canvas::linear_gradient{
            bounds.top_left(),
            bounds.bottom_left()
         };

         gradient.add_color_stop({ 1.0, rgba(255, 255, 255, 64) });
         gradient.add_color_stop({ 0.0, rgba(0, 0, 0, 32) });
         cnv.fill_style(gradient);

         cnv.begin_path();
         cnv.rect(bounds);
         cnv.fill_style(gradient);
         cnv.fill();
      }
   }

   void draw_indicator(canvas& cnv, rect bounds, color c)
   {
      cnv.fill_style(c);
      cnv.begin_path();
      cnv.round_rect(bounds, bounds.height()/5);
      cnv.fill();
   }

   void draw_thumb(canvas& cnv, circle cp, color c, color ic)
   {
      auto state = cnv.new_state();
      float radius = cp.radius;

      // Fill the body color
      {
         cnv.fill_style(c);
         cnv.begin_path();
         cnv.circle(cp);
         cnv.fill();
      }

      // Draw some 3D highlight
      {
         auto hcp = cp.center().move(-radius, -radius);
         auto gradient = canvas::radial_gradient{
            hcp, radius*0.5f,
            hcp, radius*2
         };

		 using cs = canvas::color_stop;
		 gradient.add_color_stop(cs{ 0.0f, { 1.0f, 1.0f, 1.0f, 0.4f } });
         gradient.add_color_stop(cs{ 1.0f, { 0.6f, 0.6f, 0.6f, 0.0f } });

         cnv.fill_style(gradient);
         cnv.begin_path();
         cnv.circle(cp);
         cnv.fill();
      }

      // Draw the indicator
      {
         cnv.fill_style(ic);
         cnv.begin_path();
         cnv.circle(cp.inset(cp.radius * 0.55));
         cnv.fill();
      }

      // Add some outer bevel
      {
         auto gradient = canvas::linear_gradient{
            { cp.cx, cp.cy - cp.radius },
            { cp.cx, cp.cy + cp.radius }
         };

         gradient.add_color_stop({ 0.0, colors::white.opacity(0.3) });
         gradient.add_color_stop({ 0.5, colors::black.opacity(0.5) });
         cnv.fill_rule(canvas::fill_odd_even);
         cnv.fill_style(gradient);

         circle cpf = cp;
         cnv.begin_path();
         cnv.circle(cpf);
         cpf.radius *= 0.9;
         cnv.circle(cpf);
         cnv.clip();

         cnv.circle(cp);
         cnv.fill();
      }
   }

   void draw_track(canvas& cnv, rect bounds)
   {
      auto state = cnv.new_state();
      auto w = bounds.width();
      auto h = bounds.height();
      auto r = (w > h)? h/2 : w/2;

      // extend the track a bit for the radius at the ends
      if (w > h)
         bounds = bounds.inset(-r, 0);
      else
         bounds = bounds.inset(0, -r);

      cnv.begin_path();
      cnv.round_rect(bounds, r);
      cnv.clip();

      cnv.fill_style(colors::black);
      cnv.round_rect(bounds, r);
      cnv.fill();

      auto lwidth = r/4;
      cnv.stroke_style(colors::white.opacity(0.3));
      cnv.round_rect(bounds.move(-lwidth, -lwidth), r*0.6);
      cnv.line_width(lwidth*1.5);
      cnv.stroke();
   }
}}
