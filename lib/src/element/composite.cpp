/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/composite.hpp>
#include <elements/element/port.hpp>
#include <elements/support/context.hpp>
#include <elements/view.hpp>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // composite_base class implementation
   ////////////////////////////////////////////////////////////////////////////
   element* composite_base::hit_test(context const& ctx, point p, bool leaf)
   {
      if (!empty())
      {
         hit_info info = hit_element(ctx, p, false);
         return leaf? info.leaf_element_ptr : info.element_ptr;
      }
      return nullptr;
   }

   void composite_base::draw(context const& ctx)
   {
      for (std::size_t ix = 0; ix < size(); ++ix)
      {
         rect bounds = bounds_of(ctx, ix);
         if (intersects(bounds, ctx.view_bounds()))
         {
            auto& e = at(ix);
            context ectx{ctx, &e, bounds};
            e.draw(ectx);
         }
      }
   }

   void composite_base::refresh(context const& ctx, element& element, int outward)
   {
      if (&element == this)
      {
         ctx.view.refresh(ctx, outward);
      }
      else
      {
         for (std::size_t ix = 0; ix < size(); ++ix)
         {
            rect bounds = bounds_of(ctx, ix);
            auto& e = at(ix);
            context ectx{ctx, &e, bounds};
            e.refresh(ectx, element, outward);
         }
      }
   }

   bool composite_base::click(context const& ctx, mouse_button btn)
   {
      if (!empty())
      {
         if (btn.down) // button down
         {
            hit_info info = hit_element(ctx, btn.pos, true);
            if (info.element_ptr)
            {
               if (info.element_ptr->wants_focus() && _focus != info.index)
                  new_focus(ctx, info.index, restore_previous);

               context ectx{ctx, info.element_ptr, info.bounds};
               if (info.element_ptr->click(ectx, btn))
               {
                  if (btn.down)
                     _click_tracking = info.index;
                  return true;
               }
            }
         }
         else if (_click_tracking != -1) // button up
         {
            rect  bounds = bounds_of(ctx, _click_tracking);
            auto& e = at(_click_tracking);
            context ectx{ctx, &e, bounds};
            if (e.click(ectx, btn))
               return true;
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

   void composite_base::new_focus(context const& ctx, int index, focus_request req)
   {
      // end the previous focus
      if (_focus != -1)
      {
         at(_focus).end_focus();
         ctx.view.refresh(ctx);
      }

      // start a new focus
      _focus = index;
      if (_focus != -1)
      {
         at(_focus).begin_focus(req);
         scrollable::find(ctx).scroll_into_view(bounds_of(ctx, _focus));
         ctx.view.refresh(ctx);
      }
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
         {
            new_focus(ctx, ix, req);
            return true;
         }
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

      // If we reached here, then there's either no focus, or the
      // focus did not handle the key press.
      if (reverse_index())
      {
         for (int ix = int(size())-1; ix >= 0; --ix)
         {
            if (try_key(ix))
               return true;
         }
      }
      else
      {
         for (std::size_t ix = 0; ix < size(); ++ix)
         {
            if (try_key(ix))
               return true;
         }
      }

      return false;
   }

   bool composite_base::text(context const& ctx, text_info info)
   {
      if (_focus != -1)
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
            if (!b.includes(p) || !e.hit_test(ectx, p, false))
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
         _focus = _saved_focus;

      if (_focus == -1)
      {
         if (req == from_top || req == restore_previous)
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
      if (_focus != -1)
         at(_focus).begin_focus(req);
   }

   void composite_base::end_focus()
   {
      if (_focus != -1)
         at(_focus).end_focus();
      _saved_focus = _focus;
      _focus = -1;
   }

   element const* composite_base::focus() const
   {
      return (empty() || (_focus == -1))? 0 : &at(_focus);
   }

   element* composite_base::focus()
   {
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
               context ectx{ ctx, &e, bounds_of(ctx, ix) };
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
            context ectx{ ctx, &e, b };
            if (!b.includes(p) || !e.hit_test(ectx, p))
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
         context ectx{ ctx, &e, bounds_of(ctx, _cursor_tracking) };
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
         context ectx{ ctx, &e, bounds };
         if (e.drop(ectx, d_info))
            return true;

         _cursor_tracking = -1;
         return true;
      }
      return false;
   }

   composite_base::hit_info composite_base::hit_element(context const& ctx, point p, bool control) const
   {
      auto&& test_element =
         [&](int ix, hit_info& info) -> bool
         {
            auto& e = at(ix);
            if (!control || e.wants_control())
            {
               rect bounds = bounds_of(ctx, ix);
               if (bounds.includes(p))
               {
                  context ectx{ctx, &e, bounds};
                  if (auto leaf = e.hit_test(ectx, p, true))
                  {
                     info = hit_info{&e, leaf, bounds, int(ix)};
                     return true;
                  }
               }
            }
            return false;
         };

      hit_info info = hit_info{{}, {}, rect{}, -1};
      if (reverse_index())
      {
         for (int ix = int(size())-1; ix >= 0; --ix)
            if (test_element(ix, info))
               break;
      }
      else
      {
         for (std::size_t ix = 0; ix < size(); ++ix)
            if (test_element(ix, info))
               break;
      }
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
      _saved_focus = -1;
      _click_tracking = -1;
      _cursor_tracking = -1;
      _cursor_hovering.clear();
   }
}}
