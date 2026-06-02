/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/button.hpp>

namespace cycfi::elements
{
   bool basic_button::click(context const& ctx, mouse_button btn)
   {
      if (!ctx.enabled || !is_enabled())
         return false;

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

   element* basic_button::hit_test(context const& ctx, point p, bool leaf, bool /*control*/)
   {
      return proxy_base::hit_test(ctx, p, leaf, false); // accept non-control subjects
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

   void basic_button::enable(bool state)
   {
      _state.enabled = state;
      update_receiver();
   }

   bool basic_button::is_enabled() const
   {
      return _state.enabled;
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

   /**
    * \brief
    *    Set the value of the button
    *
    * \param val
    *    A boolean value representing the value of the button `true` = ON,
    *    `false` = OPF.
    */
   void basic_button::value(bool val)
   {
      if (_state.value != val)
         state(val);
   }

   /**
    *  \brief
    *    Initiates editing the state of the button and sends notifications.
    *
    *    If `on_click` callback is set, it is called with the new state value
    *    `val`, and the `notify_edit(view_)` function is called which sends
    *    notifications about the state change.
    *
    *  \param view_
    *    A reference to the view.
    *
    *  \param val
    *    The new state value for the button: `true` if the button is ON, and
    *    `false` if OFF.
    */
   void basic_button::edit(view& view_, bool val)
   {
      if (on_click)
         on_click(val);
      receiver<bool>::notify_edit(view_);
   }

   bool basic_toggle_button::click(context const& ctx, mouse_button btn)
   {
      if (!ctx.enabled || !this->is_enabled())
         return false;

      if (btn.state != mouse_button::left || !ctx.bounds.includes(btn.pos))
      {
         this->tracking(false);
         ctx.view.refresh(ctx);
         return false;
      }

      if (btn.down)
      {
         this->tracking(true);
         if (this->state(!this->value()))    // toggle the state
         {
            ctx.view.refresh(ctx);           // we need to save the current state, the state
            _current_state = this->value();  // can change in the drag function and so we'll
         }                                   // need it later when the button is finally released
      }
      else
      {
         this->tracking(false);
         this->state(_current_state);
         if (this->on_click)
            this->on_click(this->value());
         ctx.view.refresh(ctx);
      }
      return true;
   }

   void basic_toggle_button::drag(context const& ctx, mouse_button btn)
   {
      this->hilite(ctx.bounds.includes(btn.pos));
      if (this->state(!_current_state ^ ctx.bounds.includes(btn.pos)))
         ctx.view.refresh(ctx);
   }

   bool basic_latching_button::click(context const& ctx, mouse_button btn)
   {
      if (btn.down && this->value())
         return false;

      if (btn.state != mouse_button::left || !ctx.bounds.includes(btn.pos))
      {
         this->tracking(false);
         ctx.view.refresh(ctx);
         return false;
      }

      if (btn.down)
      {
         this->tracking(true);
         this->on_tracking(ctx, this->begin_tracking);
      }
      else
      {
         this->tracking(false);
         this->on_tracking(ctx, this->end_tracking);
         if (this->on_click)
            this->on_click(true);
         ctx.view.refresh(ctx);
      }
      if (btn.down && this->state(ctx.bounds.includes(btn.pos)))
         ctx.view.refresh(ctx);
      return true;
   }

   bool basic_choice::click(context const& ctx, mouse_button btn)
   {
      if (btn.state == mouse_button::left)
      {
         if (btn.down)
         {
            return basic_latching_button::click(ctx, btn);
         }
         else
         {
            auto r = basic_latching_button::click(ctx, btn);
            if (this->value())
            {
               auto [c, cctx] = find_composite(ctx);
               if (c)
               {
                  for (std::size_t i = 0; i != c->size(); ++i)
                  {
                     if (auto e = find_element<selectable*>(&c->at(i)))
                     {
                        if (e == this)
                        {
                           // Set the button
                           e->select(true);
                        }
                        else
                        {
                           if (e->is_selected())
                           {
                              // Reset the button
                              e->select(false);
                           }
                        }
                     }
                  }
               }
               cctx->view.refresh(*cctx);
            }
            return r;
         }
      }
      return false;
   }
}
