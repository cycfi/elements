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
      if (auto* rcvr = find_subject<receiver<int>*>(this))
      {
         rcvr->value((_state ? 2 : 0) + _hilite);
         ctx.view.refresh(ctx);
      }
      return false;
   }

   void basic_button::drag(context const& ctx, mouse_button btn)
   {
      if (state(ctx.bounds.includes(btn.pos)))
         ctx.view.refresh(ctx);
   }

   bool basic_button::wants_control() const
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
         if (auto* rcvr = find_subject<receiver<int>*>(this))
            rcvr->value((_state? 2 : 0) + _hilite);
         return true;
      }
      return false;
   }

   void basic_button::value(bool new_state)
   {
      if (_state != new_state)
         state(new_state);
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

   bool basic_choice::is_selected() const
   {
      return value();
   }

   void basic_choice::select(bool state)
   {
      if (state != is_selected())
         value(state);
   }

   element* basic_choice::click(context const& ctx, mouse_button btn)
   {
      bool was_selected = is_selected();
      auto r = basic_latching_button<>::click(ctx, btn);
      if (!was_selected && value())
      {
         auto [c, cctx] = find_composite(ctx);
         if (c)
         {
            for (std::size_t i = 0; i != c->size(); ++i)
            {
               if (auto e = find_subject<basic_choice*>(&c->at(i)))
               {
                  if (e == this)
                  {
                     // Set the radio button
                     e->select(true);
                     // The base class::click should have called on_click already
                  }
                  else
                  {
                     if (e->is_selected())
                     {
                        // Reset the radio button
                        e->select(false);
                        if (e->on_click)
                           e->on_click(false);
                     }
                  }
               }
            }
         }
         cctx->view.refresh(*cctx);
      }
      return r;
   }
}}
