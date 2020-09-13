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
   bool basic_button::click(context const& ctx, mouse_button btn)
   {
      if (btn.state != mouse_button::left || !ctx.bounds.includes(btn.pos))
      {
         tracking(false);
         ctx.view.refresh(ctx);
         return false;
      }

      if (btn.down)
      {
         tracking(true);
         on_tracking(ctx, begin_tracking);
      }
      else
      {
         tracking(false);
         on_tracking(ctx, end_tracking);
         if (on_click)
            on_click(true);
         ctx.view.refresh(ctx);
      }

      if (state(btn.down && ctx.bounds.includes(btn.pos)))
         ctx.view.refresh(ctx);
      return true;
   }

   bool basic_button::cursor(context const& ctx, point /* p */, cursor_tracking status)
   {
      hilite(status != cursor_tracking::leaving);
      if (update_receiver())
         ctx.view.refresh(ctx);
      return false;
   }

   void basic_button::drag(context const& ctx, mouse_button btn)
   {
      this->hilite(ctx.bounds.includes(btn.pos));
      if (state(ctx.bounds.includes(btn.pos)))
         ctx.view.refresh(ctx);
   }

   bool basic_button::wants_control() const
   {
      return true;
   }

   bool basic_button::state(bool val)
   {
      if (val != _state.value)
      {
         _state.value = val;
         update_receiver();
         return true;
      }
      return false;
   }

   void basic_button::tracking(bool val)
   {
      if (val != _state.tracking)
      {
         _state.tracking = val;
         update_receiver();
      }
   }
   void basic_button::hilite(bool val)
   {
      if (val != _state.hilite)
      {
         _state.hilite = val;
         update_receiver();
      }
   }

   bool basic_button::update_receiver()
   {
      if (auto* rcvr = find_subject<receiver<int>*>(this))
      {
         rcvr->value((_state.value? 2 : 0) + _state.hilite);
         return true;
      }
      else if (auto* rcvr = find_subject<receiver<button_state>*>(this))
      {
         rcvr->value(_state);
         return true;
      }
      return false;
   }

   void basic_button::value(bool val)
   {
      if (_state.value != val)
         state(val);
   }

   void basic_button::send(bool val)
   {
      if (on_click)
         on_click(val);
   }

   void basic_button::on_send(callback_function f)
   {
      on_click = f;
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

   bool layered_button::click(context const& ctx, mouse_button btn)
   {
      if (btn.state != mouse_button::left || !ctx.bounds.includes(btn.pos))
         return false;

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
      return true;
   }

   void layered_button::drag(context const& ctx, mouse_button btn)
   {
      if (state(ctx.bounds.includes(btn.pos)))
         ctx.view.refresh(ctx);
   }

   bool layered_button::wants_control() const
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

   void layered_button::send(bool val)
   {
      if (on_click)
         on_click(val);
   }

   void layered_button::on_send(callback_function f)
   {
      on_click = f;
   }

   void basic_choice_base::do_click(context const& ctx)
   {
      auto [c, cctx] = find_composite(ctx);
      if (c)
      {
         for (std::size_t i = 0; i != c->size(); ++i)
         {
            if (auto e = find_element<basic_choice_base*>(&c->at(i)))
            {
               if (e == this)
               {
                  // Set the button
                  e->select(true);
                  // The base class::click should have called on_click already
               }
               else
               {
                  if (e->is_selected())
                  {
                     // Reset the button
                     e->select(false);
                     e->get_sender().send(false);
                  }
               }
            }
         }
      }
      cctx->view.refresh(*cctx);
   }
}}
