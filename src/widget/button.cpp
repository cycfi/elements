/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/button.hpp>
#include <photon/view.hpp>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Momentary Button
   ////////////////////////////////////////////////////////////////////////////////////////////////   widget* basic_latching_button::click(context const& ctx, mouse_button btn)
   widget* basic_button::hit_test(context const& ctx, point p)
   {
      if (ctx.bounds.includes(p))
         return this;
      return 0;
   }

   widget* basic_button::click(context const& ctx, mouse_button btn)
   {
      if (!ctx.bounds.includes(btn.pos))
         return 0;
      if (!btn.down && on_click)
         on_click(true);
      if (state(btn.down && ctx.bounds.includes(btn.pos)))
         ctx.view.refresh(ctx.bounds);
      return this;
   }

   void basic_button::drag(context const& ctx, mouse_button btn)
   {
      if (state(ctx.bounds.includes(btn.pos)))
         ctx.view.refresh(ctx.bounds);
   }

   bool basic_button::is_control() const
   {
      return true;
   }

   bool basic_button::value() const
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

   void basic_button::value(bool new_state)
   {
      state(new_state);
      if (on_click && _state != new_state)
         on_click(_state);
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Toggle Button
   ////////////////////////////////////////////////////////////////////////////////////////////////   widget* basic_latching_button::click(context const& ctx, mouse_button btn)
   widget* basic_toggle_button::click(context const& ctx, mouse_button btn)
   {
      if (!ctx.bounds.includes(btn.pos))
      {
         ctx.view.refresh(ctx.bounds);
         return 0;
      }

      if (btn.down)
      {
         if (state(!value()))             // toggle the state
         {
            ctx.view.refresh(ctx.bounds); // we need to save the current state, the state
            _current_state = value();     // can change in the drag function and so we'll
         }                                // need it later when the button is finally released
      }
      else
      {
         state(_current_state);
         if (on_click)
            on_click(value());
      }
      return this;
   }

   void basic_toggle_button::drag(context const& ctx, mouse_button btn)
   {
      if (state(!_current_state ^ ctx.bounds.includes(btn.pos)))
         ctx.view.refresh(ctx.bounds);
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Latching Button
   ////////////////////////////////////////////////////////////////////////////////////////////////
   widget* basic_latching_button::click(context const& ctx, mouse_button btn)
   {
      if (!ctx.bounds.includes(btn.pos))
         return 0;
      if (btn.down)
         return basic_button::click(ctx, btn);
      else if (on_click)
         on_click(true);
      return this;
   }
}
