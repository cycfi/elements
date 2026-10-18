/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/child_window.hpp>
#include <elements/element/floating.hpp>
#include <elements/view.hpp>
#include <algorithm>
#include <cassert>
#include <chrono>
#include <functional>

namespace cycfi::elements
{
   void child_window_element::draw(context const& ctx)
   {
      if (_opacity < 1.0f)
      {
         auto group = ctx.canvas.new_group(_opacity);
         floating_element::draw(ctx);
      }
      else
      {
         floating_element::draw(ctx);
      }
   }

   // Fade out over about 300 ms, a step a frame, then leave the view. A
   // window opened again comes back opaque.
   void child_window_element::close(view& view_)
   {
      using namespace std::chrono_literals;
      if (is_closing())
         return;

      auto self = std::static_pointer_cast<child_window_element>(
         shared_from_this());
      // Each step is kept alive by the timer that runs it, so a view that
      // goes away mid-fade takes the steps with it.
      auto step = std::make_shared<std::function<void()>>();
      *step = [self, &view_, next = std::weak_ptr{step}]()
      {
         self->_opacity -= 16.0f / 300.0f;
         if (self->_opacity <= 0)
         {
            view_.remove(self);
            view_.post([self] { self->_opacity = 1.0f; });
            return;
         }
         view_.refresh(*self);
         view_.post(16ms, [s = next.lock()] { (*s)(); });
      };
      (*step)();
   }

   // A window takes the mouse anywhere inside it: a click on its body, not
   // only on a control in it, raises it, and does not reach what is under
   // it.
   element* child_window_element::hit_test(
      context const& ctx, point p, bool leaf, bool control)
   {
      if (is_closing())
         return nullptr;                  // fading out, no longer there
      if (auto e = floating_element::hit_test(ctx, p, leaf, control))
         return e;
      return bounds().includes(p)? this : nullptr;
   }

   bool child_window_element::click(context const& ctx, mouse_button btn)
   {
      // A press brings the window to the front. The click goes to the
      // window now: it reached this window, so nothing is above it under
      // the cursor, and handling it here keeps the press before the
      // release. The move is posted, since the layers are being
      // traversed. A window that is not a layer of the view has nothing
      // to move.
      if (btn.down)
      {
         auto this_ = shared_from_this();
         if (ctx.view.is_open(this_) && ctx.view.layers().back() != this_)
            ctx.view.move_to_front(this_);
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

   int resizable_base::edges_at(context const& ctx, point p) const
   {
      auto const& b = ctx.bounds;
      if (!b.includes(p))
         return none;
      int e = none;
      if (p.x < b.left + grip)
         e |= left;
      else if (p.x > b.right - grip)
         e |= right;
      if (p.y < b.top + grip)
         e |= top;
      else if (p.y > b.bottom - grip)
         e |= bottom;
      return e;
   }

   element* resizable_base::hit_test(
      context const& ctx, point p, bool leaf, bool control)
   {
      if (edges_at(ctx, p) != none)
         return this;
      return proxy_base::hit_test(ctx, p, leaf, control);
   }

   bool resizable_base::click(context const& ctx, mouse_button btn)
   {
      if (btn.down)
      {
         auto fl = find_parent<floating_element*>(ctx);
         auto e = edges_at(ctx, btn.pos);
         if (fl && e != none)
         {
            _edges = e;
            _start = btn.pos;
            _start_bounds = fl->bounds();
            return true;
         }
      }
      else if (_edges != none)
      {
         _edges = none;
         return true;
      }
      return proxy_base::click(ctx, btn);
   }

   void resizable_base::drag(context const& ctx, mouse_button btn)
   {
      auto fl = find_parent<floating_element*>(ctx);
      if (_edges == none || !fl)
      {
         proxy_base::drag(ctx, btn);
         return;
      }

      // Move the grabbed edges, keeping the others, within the limits of
      // the floating element's content.
      auto lim = fl->subject().limits(ctx);
      auto const dx = btn.pos.x - _start.x;
      auto const dy = btn.pos.y - _start.y;
      auto r = _start_bounds;
      if (_edges & left)
         r.left = std::clamp(r.left + dx
          , r.right - lim.max.x, r.right - lim.min.x);
      if (_edges & right)
         r.right = std::clamp(r.right + dx
          , r.left + lim.min.x, r.left + lim.max.x);
      if (_edges & top)
         r.top = std::clamp(r.top + dy
          , r.bottom - lim.max.y, r.bottom - lim.min.y);
      if (_edges & bottom)
         r.bottom = std::clamp(r.bottom + dy
          , r.top + lim.min.y, r.top + lim.max.y);

      fl->bounds(r);
      ctx.view.layout();               // and the area it left, refreshed
   }

   bool resizable_base::cursor(
      context const& ctx, point p, cursor_tracking status)
   {
      auto e = _edges != none? _edges
         : status == cursor_tracking::leaving? int(none) : edges_at(ctx, p);
      if (e != none)
      {
         // There are no diagonal cursors: a corner takes the side's.
         set_cursor((e & (left | right))? cursor_type::h_resize
            : cursor_type::v_resize);
         return true;
      }
      return proxy_base::cursor(ctx, p, status);
   }

   std::string resizable_base::class_name() const
   {
      return "resizable";
   }

   void close_floating_element(context& ctx, floating_element* fl)
   {
      assert(fl);
      if (auto cw = dynamic_cast<child_window_element*>(fl))
         cw->close(ctx.view);
      else
         ctx.view.remove(fl->shared_from_this());
   }

   std::string child_window_element::class_name() const
   {
      return "child_window";
   }

   std::string movable_base::class_name() const
   {
      return "movable";
   }
}
