/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/button.hpp>
#include <photon/theme.hpp>
#include <photon/support.hpp>
#include <photon/app.hpp>
#include <photon/window.hpp>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Momentary Button
   widget* button_widget::click(context const& ctx, mouse_button btn)
   {
      if (state(btn.is_pressed && ctx.bounds.includes(ctx.cursor_pos())))
         ctx.window.draw();
      return this;
   }

   void button_widget::drag(context const& ctx, mouse_button btn)
   {
      if (state(ctx.bounds.includes(ctx.cursor_pos())))
         ctx.window.draw();
   }

   bool button_widget::is_control() const
   {
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Button Body
   void botton_body_widget::draw(context const& ctx)
   {
      ctx.theme().draw_button(ctx.bounds, body_color);
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Toggle Button
   widget* toggle_button_widget::click(context const& ctx, mouse_button btn)
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

   void toggle_button_widget::drag(context const& ctx, mouse_button btn)
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
   widget* latching_button_widget::click(context const& ctx, mouse_button btn)
   {
      if (btn.is_pressed)
         return button_widget::click(ctx, btn);
      return this;
   }
}
