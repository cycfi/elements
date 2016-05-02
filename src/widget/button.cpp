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
   void button_widget::draw(context const& ctx)
   {
      auto c = _color;
      if (_state)
         c.alpha /= 0.5;

      ctx.theme().draw_button(ctx.bounds, c);

      if (_state)
         subject()->draw({ ctx, rect{ ctx.bounds }.move(1, 1) });
      else
         subject()->draw(ctx);
   }

   widget* button_widget::click(context const& ctx, mouse_button btn)
   {
      _state = btn.is_pressed && ctx.bounds.includes(ctx.cursor_pos());
      ctx.window.draw();
      return this;
   }

   void button_widget::drag(context const& ctx, mouse_button btn)
   {
      _state = ctx.bounds.includes(ctx.cursor_pos());
      ctx.window.draw();
   }

   bool button_widget::is_control() const
   {
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Toggle Button
   widget* toggle_button_widget::click(context const& ctx, mouse_button btn)
   {
      if (!ctx.bounds.includes(ctx.cursor_pos()))
         return this;

      if (btn.is_pressed)
      {
         state(!state());
         ctx.window.draw();
         state(!state());
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
      state(curr_state ^ ctx.bounds.includes(ctx.cursor_pos()));
      ctx.window.draw();
      state(curr_state);
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
