/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <photon/element/button.hpp>

#include <iostream>

namespace cycfi { namespace photon
{
   ////////////////////////////////////////////////////////////////////////////
   // Basic Button
   ////////////////////////////////////////////////////////////////////////////
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

   void basic_button::draw(context const& ctx)
   {
      if (!ctx.view.current_button().down)
      {
         bool hit = ctx.bounds.includes(ctx.view.cursor_pos());
         subject().value(hit? 1 : _state? 2 : 0);
         if (hit)
            ctx.view.refresh(ctx);
      }
      proxy_base::draw(ctx);
   }

   bool basic_button::cursor(context const& ctx, point p, cursor_tracking status)
   {
      if (ctx.bounds.includes(p) || status == cursor_tracking::leaving)
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
         subject().value(_state? 2 : 0);
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
   // Layered Button
   ////////////////////////////////////////////////////////////////////////////   element* basic_latching_button::click(context const& ctx, mouse_button btn)
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
      if (!btn.down && on_click)
         on_click(true);
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
      state(new_state);
      if (on_click && _state != new_state)
         on_click(_state);
   }

   void layered_button::value(int new_state)
   {
      value(bool(new_state));
   }
}}
