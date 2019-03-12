/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <photon/element/button.hpp>
#include <photon/view.hpp>

namespace cycfi { namespace photon
{
   ////////////////////////////////////////////////////////////////////////////
   // Momentary Button
   ////////////////////////////////////////////////////////////////////////////   element* basic_latching_button::click(context const& ctx, mouse_button btn)
   element* basic_button::hit_test(context const& ctx, point p)
   {
      if (ctx.bounds.includes(p))
         return this;
      return 0;
   }

   element* basic_button::click(context const& ctx, mouse_button btn)
   {
      if (!ctx.bounds.includes(btn.pos))
         return 0;
      if (!btn.down && on_click)
         on_click(true);
      if (state(btn.down && ctx.bounds.includes(btn.pos)))
         ctx.view.refresh(ctx);
      return this;
   }

   void basic_button::drag(context const& ctx, mouse_button btn)
   {
      if (state(ctx.bounds.includes(btn.pos)))
         ctx.view.refresh(ctx);
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
         deck_element::select(_state);
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

   void basic_button::value(int new_state)
   {
      value(bool(new_state));
   }

   ////////////////////////////////////////////////////////////////////////////
   // Toggle Button
   ////////////////////////////////////////////////////////////////////////////
   element* basic_toggle_button::click(context const& ctx, mouse_button btn)
   {
      if (!ctx.bounds.includes(btn.pos))
      {
         ctx.view.refresh(ctx);
         return 0;
      }

      if (btn.down)
      {
         if (state(!value()))             // toggle the state
         {
            ctx.view.refresh(ctx);        // we need to save the current state, the state
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
         ctx.view.refresh(ctx);
   }

   ////////////////////////////////////////////////////////////////////////////
   // Latching Button
   ////////////////////////////////////////////////////////////////////////////
   element* basic_latching_button::click(context const& ctx, mouse_button btn)
   {
      if (!ctx.bounds.includes(btn.pos))
         return 0;
      if (btn.down)
         return basic_button::click(ctx, btn);
      else if (on_click)
         on_click(true);
      return this;
   }
}}
