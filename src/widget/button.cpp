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
      if (!btn.is_pressed && on_click)
         on_click(true);
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

   template <typename Button>
   Button make_button(std::string const& text, color body_color)
   {
      color c = body_color;

      auto btxt = margin({ 15, 8, 15, 8 }, halign(0.5,  heading(text)));
      auto btn_body_off = basic_botton_body(c.level(0.9));
      auto btn_body_on = basic_botton_body(c.opacity(0.5));

      auto btn_img_off = layer(btxt, btn_body_off);
      auto btn_img_on = left_top_margin({1, 1}, layer(btxt, btn_body_on));

      return Button(new_(btn_img_off), new_(btn_img_on));
   }

   basic_button button(std::string const& text, color body_color)
   {
      return make_button<basic_button>(text, body_color);
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Button Body
   float basic_botton_body::corner_radius = 4.0;

   void basic_botton_body::draw(context const& ctx)
   {
      auto&       _canvas = ctx.theme().canvas();
      auto const& b = ctx.bounds;

      paint bg
         = _canvas.linear_gradient(b.top_left(), b.bottom_left()
          , color(255, 255, 255, 32)
          , color(0, 0, 0, 32)
         );

      _canvas.begin_path();
      _canvas.round_rect(b.inset(1, 1), corner_radius-1);
      _canvas.fill_color(body_color);
      _canvas.fill();
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
         if (on_click)
            on_click(state());
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

   basic_toggle_button toggle_button(std::string const& text, color body_color)
   {
      return make_button<basic_toggle_button>(text, body_color);
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Latching Button
   widget* basic_latching_button::click(context const& ctx, mouse_button btn)
   {
      if (btn.is_pressed)
         return basic_button::click(ctx, btn);
      else if (on_click)
         on_click(true);
      return this;
   }

   basic_latching_button latching_button(std::string const& text, color body_color)
   {
      return make_button<basic_latching_button>(text, body_color);
   }
}
