/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/button.hpp>
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
      if (!ctx.bounds.includes(ctx.cursor_pos()))
         return 0;
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

   bool basic_button::state() const
   {
      return _state;
   }

   bool basic_button::state(bool new_state)
   {
      if (new_state != _state)
      {
         _state = new_state;
         deck_widget::select(_state);
         return true;
      }
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Toggle Button
   widget* basic_toggle_button::click(context const& ctx, mouse_button btn)
   {
      if (!ctx.bounds.includes(ctx.cursor_pos()))
         return 0;

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

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Latching Button
   widget* basic_latching_button::click(context const& ctx, mouse_button btn)
   {
      if (!ctx.bounds.includes(ctx.cursor_pos()))
         return 0;
      if (btn.is_pressed)
         return basic_button::click(ctx, btn);
      else if (on_click)
         on_click(true);
      return this;
   }
}
