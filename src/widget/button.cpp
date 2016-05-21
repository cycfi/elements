/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/button.hpp>
#include <photon/widget/align.hpp>
#include <photon/widget/text.hpp>
#include <photon/window.hpp>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Momentary Button
   widget* basic_button::hit_test(context const& ctx, point p)
   {
      if (ctx.bounds.includes(ctx.cursor_pos()))
         return this;
      return 0;
   }
   widget* basic_button::click(context const& ctx, mouse_button btn)
   {
      if (state(btn.is_pressed && ctx.bounds.includes(ctx.cursor_pos())))
         ctx.window.draw();
      return this;
   }

   void basic_button::drag(context const& ctx, mouse_button btn)
   {
      if (state(ctx.bounds.includes(ctx.cursor_pos())))
         ctx.window.draw();
   }

   bool basic_button::is_control() const
   {
      return true;
   }

   basic_button button(color body_color, std::string const& text)
   {
      color c = body_color;

      auto btxt = margin({ 15, 8, 15, 8 }, halign(0.5,  heading(text)));
      auto btn_body_off = basic_botton_body(c);
      c.alpha = 0.5;
      auto btn_body_on = basic_botton_body(c);

      auto btn_img_off = layer(btxt, btn_body_off);
      auto btn_img_on = left_top_margin({1, 1}, layer(btxt, btn_body_on));

      return basic_button(new_(btn_img_off), new_(btn_img_on));
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Button Body
   float basic_botton_body::corner_radius = 4.0;

   void basic_botton_body::draw(context const& ctx)
   {
      auto&       _canvas = ctx.theme().canvas();
      bool        black = body_color == colors::black;
      auto const& b = ctx.bounds;

      paint bg
         = _canvas.linear_gradient(b.top_left(), b.bottom_left()
          , color(255, 255, 255, black ? 16 : 32)
          , color(0, 0, 0, black ? 16 : 32)
         );

      _canvas.begin_path();
      _canvas.round_rect(b.inset(1, 1), corner_radius-1);

      if (!black)
      {
         _canvas.fill_color(body_color);
         _canvas.fill();
      }
      _canvas.fill_paint(bg);
      _canvas.fill();

      _canvas.begin_path();
      _canvas.round_rect(b.inset(0.5, 0.5), corner_radius-0.5);
      _canvas.stroke_color(color{ 0, 0, 0, 48 });
      _canvas.stroke();
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Toggle Button
   widget* basic_toggle_button::click(context const& ctx, mouse_button btn)
   {
      if (!ctx.bounds.includes(ctx.cursor_pos()))
         return this;

      if (btn.is_pressed)
      {
         if (state(!state()))
         {
            ctx.window.draw();
            state(!state());
         }
      }
      else
      {
         state(!state());
      }
      return this;
   }

   void basic_toggle_button::drag(context const& ctx, mouse_button btn)
   {
      bool curr_state = state();
      if (state(curr_state ^ ctx.bounds.includes(ctx.cursor_pos())))
      {
         ctx.window.draw();
         state(curr_state);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Latching Button
   widget* basic_latching_button::click(context const& ctx, mouse_button btn)
   {
      if (btn.is_pressed)
         return basic_button::click(ctx, btn);
      return this;
   }
}
