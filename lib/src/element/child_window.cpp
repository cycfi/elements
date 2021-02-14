/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/child_window.hpp>
#include <elements/element/floating.hpp>
#include <elements/view.hpp>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // child_window_element
   ////////////////////////////////////////////////////////////////////////////
   bool child_window_element::click(context const& ctx, mouse_button btn)
   {
      if (btn.down)
      {
         auto this_ = shared_from_this();
         // If the child window is not already at the front,
         if (ctx.view.layers().back() != this_)
         {
            // Move child window to the front
            ctx.view.move_to_front(this_);

            // Simulate a view click for continuation
            ctx.view.post(
               [btn, &view = ctx.view]()
               {
                  view.click(btn);
               }
            );
            return true;
         }
      }
      return floating_element::click(ctx, btn);
   }

   ////////////////////////////////////////////////////////////////////////////
   // movable_base
   ////////////////////////////////////////////////////////////////////////////
   bool movable_base::click(context const& ctx, mouse_button btn)
   {
      bool tracking_subject = proxy_base::click(ctx, btn);
      if (!tracking_subject)
      {
         bool r = tracker::click(ctx, btn);
         auto state = get_state();
         if (state)
         {
            state->_offs_top = btn.pos.y - ctx.bounds.top;
            state->_offs_bottom = ctx.bounds.bottom - btn.pos.y;
         }
         return r;
      }
      return tracking_subject;
   }

   void movable_base::drag(context const& ctx, mouse_button btn)
   {
      auto state = get_state();
      if (!state)
      {
         proxy_base::drag(ctx, btn);
      }
      else
      {
         // Clamp the mouse position so we don't move the child window
         // outside the view which will prevent it from being moved when the
         // movable control (e.g. title bar) falls out of view.

         auto b = ctx.view_bounds();         // MacOS style: we want the movable
         b.top += state->_offs_top;          // part to be fully visible when moving
         b.bottom -= state->_offs_bottom;    // to the top or bottom of the main view.

         clamp(btn.pos.x, b.left, b.right);
         clamp(btn.pos.y, b.top, b.bottom);

         tracker::drag(ctx, btn);
      }
   }

   void movable_base::keep_tracking(context const& ctx, tracker_info& track_info)
   {
      if (track_info.current != track_info.previous)
      {
         auto fl = find_parent<floating_element*>(ctx);
         if (fl)
         {
            auto p = track_info.movement();
            fl->bounds(fl->bounds().move(p.x, p.y));
            ctx.view.refresh();
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   // closable_element
   ////////////////////////////////////////////////////////////////////////////
   void close_floating_element(context& ctx, floating_element* fl)
   {
      ctx.view.remove(fl->shared_from_this());
   }
}}
