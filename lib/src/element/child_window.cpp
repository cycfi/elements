/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/child_window.hpp>
#include <elements/element/floating.hpp>
#include <elements/view.hpp>

namespace cycfi::elements
{
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

   element* movable_base::hit_test(context const& ctx, point p, bool /*leaf*/, bool /*control*/)
   {
      if (ctx.enabled && is_enabled() && ctx.bounds.includes(p))
         return this;
      return nullptr;
   }

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

   bool resizable_base::cursor(context const& ctx, point p, cursor_tracking /* status */)
   {
      auto outer = ctx.bounds.inset(-5);
      auto inner = ctx.bounds.inset(5);
      if (ctx.enabled && is_enabled() && outer.includes(p) && !inner.includes(p))
      {
         auto const& b = ctx.bounds;
         bool h_resize = false;
         bool v_resize = false;
         if (p.x > b.left - 5 && p.x < b.left + 5)
            h_resize = true;
         else if (p.x > b.right - 5 && p.x < b.right + 5)
            h_resize = true;

         if (p.y > b.top - 5 && p.y < b.top + 5)
            v_resize = true;
         else if (p.y > b.bottom - 5 && p.y < b.bottom + 5)
            v_resize = true;

         if (h_resize != v_resize)
            set_cursor(h_resize? cursor_type::h_resize : cursor_type::v_resize);
         return true;
      }
      return false;
   }

   element* resizable_base::hit_test(context const& ctx, point p, bool leaf, bool control)
   {
      auto outer = ctx.bounds.inset(-5);
      auto inner = ctx.bounds.inset(5);
      if (ctx.enabled && is_enabled() && outer.includes(p) && !inner.includes(p))
         return this;
      return proxy_base::hit_test(ctx, p, leaf, control);
   }

   bool resizable_base::click(context const& ctx, mouse_button btn)
   {
      if (proxy_base::click(ctx, btn))
         return true;

      if (ctx.enabled && is_enabled())
      {
         bool r = tracker::click(ctx, btn);
         auto state = get_state();
         if (state)
         {
            auto outer = ctx.bounds.inset(-5);
            auto inner = ctx.bounds.inset(5);
            if (outer.includes(btn.pos) && !inner.includes(btn.pos))
            {
               auto const& b = ctx.bounds;
               if (btn.pos.x > b.left - 5 && btn.pos.x < b.left + 5)
                  state->_handle = resizable_tracker_info::left;
               else if (btn.pos.x > b.right - 5 && btn.pos.x < b.right + 5)
                  state->_handle = resizable_tracker_info::right;

               if (btn.pos.y > b.top - 5 && btn.pos.y < b.top + 5)
                  state->_handle |= resizable_tracker_info::top;
               else if (btn.pos.y > b.bottom - 5 && btn.pos.y < b.bottom + 5)
                  state->_handle |= resizable_tracker_info::bottom;
            }
         }
         return r;
      }
      return false;
   }

   void resizable_base::drag(context const& ctx, mouse_button btn)
   {
      auto state = get_state();
      if (!state)
      {
         proxy_base::drag(ctx, btn);
      }
      else
      {
         tracker::drag(ctx, btn);
      }
   }

   void resizable_base::keep_tracking(context const& ctx, tracker_info& track_info)
   {
      if (track_info.current != track_info.previous)
      {
         auto fl = find_parent<floating_element*>(ctx);
         if (fl)
         {
            auto p = track_info.current;
            auto b = fl->bounds();
            if (auto state = get_state(); state && state->_handle)
            {
               if (state->_handle & resizable_tracker_info::left)
                  b.left = p.x;
               else if (state->_handle & resizable_tracker_info::right)
                  b.right = p.x;

               if (state->_handle & resizable_tracker_info::top)
                  b.top = p.y;
               else if (state->_handle & resizable_tracker_info::bottom)
                  b.bottom = p.y;

               auto width = b.width();
               auto height = b.height();
               auto ob = fl->bounds();

               auto limits = fl->subject().limits(ctx);

               // Constrain width
               if (width < limits.min.x || width > limits.max.x)
               {
                  if (state->_handle & resizable_tracker_info::left)
                     b.left = ob.left;
                  else if (state->_handle & resizable_tracker_info::right)
                     b.right = ob.right;
               }

               // Constrain height
               if (height < limits.min.y || height > limits.max.y)
               {
                  if (state->_handle & resizable_tracker_info::top)
                     b.top = ob.top;
                  else if (state->_handle & resizable_tracker_info::bottom)
                     b.bottom = ob.bottom;
               }
               if (b != ob)
               {
                  fl->bounds(b);
                  ctx.view.refresh();
               }
            }
         }
      }
   }

   void close_floating_element(context& ctx, floating_element* fl)
   {
      ctx.view.remove(fl->shared_from_this());
   }

   void minimize_floating_element(context& ctx, floating_element* fl)
   {
      ctx.view.remove(fl->shared_from_this());
   }
}
