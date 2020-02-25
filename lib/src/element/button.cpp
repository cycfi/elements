/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/button.hpp>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Basic Button
   ////////////////////////////////////////////////////////////////////////////
   element* basic_button::click(context const& ctx, mouse_button btn)
   {
      if (!ctx.bounds.includes(btn.pos))
         return nullptr;

      if (btn.down)
      {
         on_tracking(ctx, begin_tracking);
      }
      else
      {
         on_tracking(ctx, end_tracking);
         if (on_click)
            on_click(true);
      }

      if (state(btn.down && ctx.bounds.includes(btn.pos)))
         ctx.view.refresh(ctx);
      return this;
   }

   bool basic_button::cursor(context const& ctx, point /* p */, cursor_tracking status)
   {
      _hilite = status != cursor_tracking::leaving;
      subject().value((_state? 2 : 0) + _hilite);
      ctx.view.refresh(ctx);
      return false;
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
         subject().value((_state? 2 : 0) + _hilite);
         return true;
      }
      return false;
   }

   void basic_button::value(bool new_state)
   {
      if (_state != new_state)
         state(new_state);
   }

   void basic_button::value(int new_state)
   {
      value(bool(new_state));
   }

   ////////////////////////////////////////////////////////////////////////////
   // Layered Button
   ////////////////////////////////////////////////////////////////////////////
   element* layered_button::hit_test(context const& ctx, point p)
   {
      if (ctx.bounds.includes(p))
         return this;
      return 0;
   }

   element* layered_button::click(context const& ctx, mouse_button btn)
   {
      if (!ctx.bounds.includes(btn.pos))
         return 0;

      if (btn.down)
      {
         on_tracking(ctx, begin_tracking);
      }
      else
      {
         on_tracking(ctx, end_tracking);
         if (on_click)
            on_click(true);
      }

      if (state(btn.down && ctx.bounds.includes(btn.pos)))
         ctx.view.refresh(ctx);
      return this;
   }

   void layered_button::drag(context const& ctx, mouse_button btn)
   {
      if (state(ctx.bounds.includes(btn.pos)))
         ctx.view.refresh(ctx);
   }

   bool layered_button::is_control() const
   {
      return true;
   }

   bool layered_button::value() const
   {
      return _state;
   }

   bool layered_button::state(bool new_state)
   {
      if (new_state != _state)
      {
         _state = new_state;
         deck_element::select(_state);
         return true;
      }
      return false;
   }

   void layered_button::value(bool new_state)
   {
      if (_state != new_state)
         state(new_state);
   }

   void layered_button::value(int new_state)
   {
      value(bool(new_state));
   }
}}
