/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/composite.hpp>
#include <elements/element/port.hpp>
#include <elements/element/traversal.hpp>
#include <elements/support/context.hpp>
#include <elements/view.hpp>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // composite_base class implementation
   ////////////////////////////////////////////////////////////////////////////
   element* composite_base::hit_test(context const& ctx, point p, bool leaf, bool control)
   {
      if (!empty())
      {
         hit_info info = hit_element(ctx, p, control);
         return leaf? info.leaf_element_ptr : info.element_ptr;
      }
      return nullptr;
   }

   void composite_base::draw(context const& ctx)
   {
      for_each_visible(ctx,
         [&ctx](element& e, std::size_t /*ix*/, rect const& bounds)
         {
            context ectx{ctx, &e, bounds};
            e.draw(ectx);
            return false;
         }
      );
   }

   void composite_base::for_each_visible(
      context const& ctx
    , for_each_callback f
    , bool reverse
   ) const
   {
      // The default `for_each_visible` implementation follows a linear
      // iteration over each item and verifies the visibility of its bounds.
      // While suitable for a small number of elements within the composite,
      // it becomes inefficient for a large composites with millions of
      // elements.

      auto  port_bounds = get_port_bounds(ctx);
      if (!intersects(ctx.bounds, port_bounds))
         return;

      if (reverse)
      {
         for (int ix = int(size())-1; ix >= 0; --ix)
         {
            rect bounds = bounds_of(ctx, ix);
            if (intersects(bounds, port_bounds))
            {
               if (f(at(ix), ix, bounds))
                  break;
            }
         }
      }
      else
      {
         for (std::size_t ix = 0; ix < size(); ++ix)
         {
            rect bounds = bounds_of(ctx, ix);
            if (intersects(bounds, port_bounds))
            {
               if (f(at(ix), ix, bounds))
                  break;
            }
         }
      }
   }

   void composite_base::refresh(context const& ctx, element& e, int outward)
   {
      if (&e == this)
      {
         ctx.view.refresh(ctx, outward);
      }
      else
      {
         for_each_visible(ctx,
            [&](element& ce, std::size_t /*ix*/, rect const& bounds)
            {
               context ectx{ctx, &ce, bounds};
               ce.refresh(ectx, e, outward);
               return &e == &ce; // Break the loop if we found the element
            }
         );
      }
   }

   void composite_base::in_context_do(context const& ctx, element& e, context_function f)
   {
      if (&e == this)
      {
         f(ctx);
      }
      else
      {
         for_each_visible(ctx,
            [&](element& ce, std::size_t /*ix*/, rect const& bounds)
            {
               context ectx{ctx, &ce, bounds};
               if (&e == &ce)
               {
                  f(ctx);
                  return true; // break the for loop
               }
               else
               {
                  ce.in_context_do(ectx, e, f);
               }
               return false;
            }
         );
      }
   }

   bool composite_base::click(context const& ctx, mouse_button btn)
   {
      if (!empty())
      {
         if (btn.down) // button down
         {
            hit_info info = hit_element(ctx, btn.pos, true);
            if (info.element_ptr && info.leaf_element_ptr)
            {
               bool switched_focus = true;
               if (_focus != info.index)
               {
                  auto idx = info.leaf_element_ptr->wants_focus() ? info.index : -1;
                  switched_focus = new_focus(ctx, idx, restore_previous);
               }

               if (switched_focus)
               {
                  if (info.element_ptr->wants_control())
                  {
                     context ectx{ctx, info.element_ptr, info.bounds};
                     if (info.element_ptr->click(ectx, btn))
                     {
                        if (btn.down)
                           _click_tracking = info.index;
                        return true;
                     }
                  }
                  else
                  {
                     // Clicking elsewhere should relinquish focus
                     relinquish_focus(*this, ctx);
                  }
               }
            }
         }
         else if (_click_tracking != -1) // button up
         {
            rect  bounds = bounds_of(ctx, _click_tracking);
            auto& e = at(_click_tracking);
            context ectx{ctx, &e, bounds};
            if (e.click(ectx, btn))
            {
               _click_tracking = -1;
               return true;
            }
         }
      }
      _click_tracking = -1;
      return false;
   }

   void composite_base::drag(context const& ctx, mouse_button btn)
   {
      if (_click_tracking != -1)
      {
         rect  bounds = bounds_of(ctx, _click_tracking);
         auto& e = at(_click_tracking);
         context ectx{ctx, &e, bounds};
         e.drag(ectx, btn);
      }
   }

   bool composite_base::new_focus(context const& ctx, int index, focus_request req)
   {
      // end the previous focus
      if (_focus != -1 && _focus < int(size()))
      {
         if (at(_focus).end_focus())
            ctx.view.refresh(ctx);
         else
            return false;
      }

      // start a new focus
      _focus = index;
      if (_focus != -1)
      {
         at(_focus).begin_focus(req);
         scrollable::find(ctx).scroll_into_view(bounds_of(ctx, _focus));
         ctx.view.refresh(ctx);
      }
      return true;
   }

   bool composite_base::key(context const& ctx, key_info k)
   {
      auto&& try_key = [&](auto ix) -> bool
      {
         rect bounds = bounds_of(ctx, ix);

         // Only visible elements are given a chance to process key
         if (intersects(bounds, ctx.view_bounds()))
         {
            auto& e = at(ix);
            context ectx{ctx, &e, bounds};
            return e.key(ectx, k);
         }
         return false;
      };

      auto&& try_focus = [&](auto ix, focus_request req) -> bool
      {
         if (at(ix).wants_focus())
            return new_focus(ctx, ix, req);
         return false;
      };

      if (_focus != -1)
      {
         if (try_key(_focus))
            return true;
      }

      if ((k.action == key_action::press || k.action == key_action::repeat)
         && k.key == key_code::tab && size())
      {
         auto next_focus = _focus;
         bool reverse = (k.modifiers & mod_shift) ^ reverse_index();
         if (next_focus == -1 && reverse)
            next_focus = size();

         if (!reverse)
         {
            while (++next_focus != static_cast<int>(size()))
               if (try_focus(next_focus, from_top))
                  return true;
            return false;
         }
         else
         {
            while (--next_focus >= 0)
               if (at(next_focus).wants_focus())
                  if (try_focus(next_focus, from_bottom))
                     return true;
            return false;
         }
      }
      return false;
   }

   bool composite_base::text(context const& ctx, text_info info)
   {
      if (_focus != -1 && _focus < int(size()))
      {
         rect  bounds = bounds_of(ctx, _focus);
         auto& focus_ = at(_focus);
         context ectx{ctx, &focus_, bounds};
         return focus_.text(ectx, info);
      };

      return false;
   }

   bool composite_base::cursor(context const& ctx, point p, cursor_tracking status)
   {
      if (_cursor_tracking >= int(size())) // just to be sure!
         _cursor_tracking = -1;

      // Send cursor leaving to all currently hovering elements if cursor is
      // leaving the composite.
      if (status == cursor_tracking::leaving)
      {
         for (auto ix : _cursor_hovering)
         {
            if (ix < int(size()))
            {
               auto& e = at(ix);
               context ectx{ctx, &e, bounds_of(ctx, ix)};
               e.cursor(ectx, p, cursor_tracking::leaving);
            }
         }
         return false;
      }

      // Send cursor leaving to all currently hovering elements if p is
      // outside the elements's bounds or if the element is no longer hit.
      for (auto i = _cursor_hovering.begin(); i != _cursor_hovering.end();)
      {
         if (*i < int(size()))
         {
            auto& e = at(*i);
            rect  b = bounds_of(ctx, *i);
            context ectx{ctx, &e, b};
            if (!b.includes(p) || !e.hit_test(ectx, p, false, true))
            {
               e.cursor(ectx, p, cursor_tracking::leaving);
               i = _cursor_hovering.erase(i);
               continue;
            }
         }
         ++i;
      }

      // Send cursor entering to newly hit element or hovering to current
      // tracking element
      hit_info info = hit_element(ctx, p, true);
      if (info.element_ptr)
      {
         _cursor_tracking = info.index;
         status = cursor_tracking::hovering;
         if (_cursor_hovering.find(info.index) == _cursor_hovering.end())
         {
            status = cursor_tracking::entering;
            _cursor_hovering.insert(_cursor_tracking);
         }
         auto& e = at(_cursor_tracking);
         context ectx{ctx, &e, bounds_of(ctx, _cursor_tracking)};
         return e.cursor(ectx, p, status);
      }

      return false;
   }

   bool composite_base::scroll(context const& ctx, point dir, point p)
   {
      if (!empty())
      {
         hit_info info = hit_element(ctx, p, true);
         if (auto ptr = info.element_ptr; ptr && artist::intersects(info.bounds, ctx.view_bounds()))
         {
            context ectx{ctx, ptr, info.bounds};
            return ptr->scroll(ectx, dir, p);
         }
      }
      return false;
   }

   bool composite_base::wants_focus() const
   {
      for (std::size_t ix = 0; ix != size(); ++ix)
         if (at(ix).wants_focus())
            return true;
      return false;
   }

   void composite_base::begin_focus(focus_request req)
   {
      if (_focus == -1 && req == restore_previous)
      {
         if (_saved_focus == -1)
            return;
         _focus = _saved_focus;
      }

      if (_focus == -1)
      {
         if (req == from_top)
         {
            for (std::size_t ix = 0; ix != size(); ++ix)
               if (at(ix).wants_focus())
               {
                  _focus = ix;
                  break;
               }
         }
         else if (req == from_bottom)
         {
            for (int ix = size()-1; ix >= 0; --ix)
               if (at(ix).wants_focus())
               {
                  _focus = ix;
                  break;
               }
         }
      }
      if (_focus != -1 && _focus < int(size()))
         at(_focus).begin_focus(req);
   }

   bool composite_base::end_focus()
   {
      bool yield = true;
      if (_focus != -1 && _focus < int(size()))
         yield = at(_focus).end_focus();
      if (yield)
      {
         _saved_focus = _focus;
         _focus = -1;
      }
      return yield;
   }

   void relinquish_focus(composite_base& c, context const& ctx)
   {
      if (c.focus_index() != -1)
      {
         c.end_focus();
         c._saved_focus = -1;
         auto [p, cctx] = find_composite(ctx);
         if (p)
            relinquish_focus(*p, *cctx);
         else
            ctx.view.relinquish_focus();
      }
   }

   element const* composite_base::focus() const
   {
      if (_focus >= int(size()))
         return nullptr;
      return (empty() || (_focus == -1))? 0 : &at(_focus);
   }

   element* composite_base::focus()
   {
      if (_focus >= int(size()))
         return nullptr;
      return (empty() || (_focus == -1))? 0 : &at(_focus);
   }

   void composite_base::focus(std::size_t index)
   {
      if (index < size())
         _focus = int(index);
   }

   void composite_base::track_drop(context const& ctx, drop_info const& d_info, cursor_tracking status)
   {
      if (_cursor_tracking >= int(size())) // just to be sure!
         _cursor_tracking = -1;

      // Send track_drop leaving to all currently hovering elements if cursor is
      // leaving the composite.
      if (status == cursor_tracking::leaving)
      {
         for (auto ix : _cursor_hovering)
         {
            if (ix < int(size()))
            {
               auto& e = at(ix);
               context ectx{ctx, &e, bounds_of(ctx, ix)};
               e.track_drop(ectx, d_info, cursor_tracking::leaving);
            }
         }
         return;
      }

      // Cursor location
      auto p = d_info.where;

      // Send track_drop leaving to all currently hovering elements if p is
      // outside the elements's bounds or if the element is no longer hit.
      for (auto i = _cursor_hovering.begin(); i != _cursor_hovering.end();)
      {
         if (*i < int(size()))
         {
            auto& e = at(*i);
            rect  b = bounds_of(ctx, *i);
            context ectx{ctx, &e, b};
            if (!b.includes(p) || !e.hit_test(ectx, p, false, true))
            {
               e.track_drop(ectx, d_info, cursor_tracking::leaving);
               i = _cursor_hovering.erase(i);
               continue;
            }
         }
         ++i;
      }

      // Send track_drop entering to newly hit element or hovering to current
      // tracking element
      hit_info info = hit_element(ctx, d_info.where, true);
      if (info.element_ptr)
      {
         _cursor_tracking = info.index;
         status = cursor_tracking::hovering;
         if (_cursor_hovering.find(info.index) == _cursor_hovering.end())
         {
            status = cursor_tracking::entering;
            _cursor_hovering.insert(_cursor_tracking);
         }
         auto& e = at(_cursor_tracking);
         context ectx{ctx, &e, bounds_of(ctx, _cursor_tracking)};
         return e.track_drop(ectx, d_info, status);
      }
      else
      {
         _cursor_tracking = -1;
      }
   }

   bool composite_base::drop(context const& ctx, drop_info const& d_info)
   {
      if (_cursor_tracking != -1)
      {
         rect  bounds = bounds_of(ctx, _cursor_tracking);
         auto& e = at(_cursor_tracking);
         context ectx{ctx, &e, bounds};
         if (e.drop(ectx, d_info))
            return true;
      }
      return false;
   }

   composite_base::hit_info composite_base::hit_element(context const& ctx, point p, bool control) const
   {
      hit_info info = hit_info{{}, {}, rect{}, -1};
      for_each_visible(ctx,
         [&](element& e, std::size_t ix, rect const& bounds)
         {
            if (!control || e.wants_control())
            {
               if (bounds.includes(p))
               {
                  context ectx{ctx, &e, bounds};
                  if (auto leaf = e.hit_test(ectx, p, true, control))
                  {
                     info = hit_info{&e, leaf, bounds, int(ix)};
                     return true;
                  }
               }
            }
            return false;
         },
         reverse_index()
      );
      return info;
   }

   bool composite_base::wants_control() const
   {
      for (std::size_t ix = 0; ix < size(); ++ix)
         if (at(ix).wants_control())
            return true;
      return false;
   }

   void composite_base::reset()
   {
      _focus = -1;
      _click_tracking = -1;
      _cursor_tracking = -1;
      _cursor_hovering.clear();
   }
}}
