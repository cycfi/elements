/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/button.hpp>
#include <photon/widget/margin.hpp>
#include <photon/widget/align.hpp>
#include <photon/widget/text.hpp>
#include <photon/theme.hpp>
#include <photon/support.hpp>
#include <photon/app.hpp>
#include <photon/window.hpp>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Momentary Button
   widget* basic_button_widget::click(context const& ctx, mouse_button btn)
   {
      if (state(btn.is_pressed && ctx.bounds.includes(ctx.cursor_pos())))
         ctx.window.draw();
      return this;
   }

   void basic_button_widget::drag(context const& ctx, mouse_button btn)
   {
      if (state(ctx.bounds.includes(ctx.cursor_pos())))
         ctx.window.draw();
   }

   bool basic_button_widget::is_control() const
   {
      return true;
   }

   widget_ptr button(color const& body_color, std::string const& text)
   {
      color c = body_color;

      auto btxt = margin({ 15, 8, 15, 8 }, halign(0.5,  heading(text)));
      auto btn_body_off = basic_botton_body(c);
      c.alpha = 0.5;
      auto btn_body_on = basic_botton_body(c);

      auto btn_img_off = layer(btxt, btn_body_off);
      auto btn_img_on = left_top_margin({1, 1}, layer(btxt, btn_body_on));

      return basic_button(btn_img_off, btn_img_on);
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Button Body
   void basic_botton_body_widget::draw(context const& ctx)
   {
      ctx.theme().draw_button(ctx.bounds, body_color);
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Toggle Button
   widget* basic_toggle_button_widget::click(context const& ctx, mouse_button btn)
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

   void basic_toggle_button_widget::drag(context const& ctx, mouse_button btn)
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
   widget* basic_latching_button_widget::click(context const& ctx, mouse_button btn)
   {
      if (btn.is_pressed)
         return basic_button_widget::click(ctx, btn);
      return this;
   }
}
