/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/composite.hpp>
#include <elements/support/context.hpp>
#include <elements/view.hpp>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // composite_base class implementation
   ////////////////////////////////////////////////////////////////////////////
   namespace
   {
      rect view_bounds(view const& v)
      {
         auto size = v.size();
         return rect{ 0, 0, size.x, size.y };
      }
   }

   element* composite_base::hit_test(context const& ctx, point p)
   {
      if (!empty())
      {
         hit_info info = hit_element(ctx, p);
         auto ptr = info.element.lock();
         return ptr.get();
      }
      return nullptr;
   }

   void composite_base::draw(context const& ctx)
   {
      for (std::size_t ix = 0; ix < size(); ++ix)
      {
         rect bounds = bounds_of(ctx, ix);
         if (intersects(bounds, ctx.view.dirty()))
         {
            auto& e = at(ix);
            context ectx{ ctx, &e, bounds };
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
            context ectx{ ctx, &e, bounds };
            e.refresh(ectx, element, outward);
         }
      }
   }

   element* composite_base::click(context const& ctx, mouse_button btn)
   {
      point p = btn.pos;

      if (!empty())
      {
         hit_info info = (btn.down)? hit_element(ctx, p) : _click_info;

         if (!info.element.expired() && wants_focus())
         {
            if (_focus != info.index)
               new_focus(ctx, info.index);
         }

         if (auto ptr = info.element.lock())
         {
            _drag_tracking = info.index;
            context ectx{ ctx, ptr.get(), info.bounds };
            if (ptr->click(ectx, btn))
            {
               if (btn.down)
                  _click_info = info;
               return ptr.get();
            }
         }
         else
         {
            _click_info.element.reset();
         }
      }
      _drag_tracking = -1;
      return 0;
   }

   void composite_base::drag(context const& ctx, mouse_button btn)
   {
      if (_drag_tracking != -1)
      {
         rect  bounds = bounds_of(ctx, _drag_tracking);
         auto& e = at(_drag_tracking);
         context ectx{ ctx, &e, bounds };
         e.drag(ectx, btn);
      }
   }

   void composite_base::new_focus(context const& ctx, int index)
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
         at(_focus).begin_focus();
         ctx.view.refresh(ctx);
      }
   }

   bool composite_base::key(context const& ctx, key_info k)
   {
      auto&& try_key = [&](auto ix) -> bool
      {
         rect bounds = bounds_of(ctx, ix);
         auto& e = at(ix);
         context ectx{ ctx, &e, bounds };
         return e.key(ectx, k);
      };

      auto&& try_focus = [&](auto ix) -> bool
      {
         if (at(ix).wants_focus())
         {
            new_focus(ctx, ix);
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
         if ((next_focus == -1) || !reverse)
         {
            while (++next_focus != static_cast<int>(size()))
               if (try_focus(next_focus))
                  return true;
            return false;
         }
         else
         {
            while (--next_focus >= 0)
               if (at(next_focus).wants_focus())
                  if (try_focus(next_focus))
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
         context ectx{ ctx, &focus_, bounds };
         return focus_.text(ectx, info);
      };

      return false;
   }

   namespace
   {
      void cursor_leaving(context const& ctx, point p, composite_base::hit_info& _cursor_info)
      {
         if (auto ptr = _cursor_info.element.lock())
         {
            context ectx{ ctx, ptr.get(), _cursor_info.bounds };
            ptr->cursor(ectx, p, cursor_tracking::leaving);
         }
         _cursor_info = composite_base::hit_info{};
      }
   }

   bool composite_base::cursor(context const& ctx, point p, cursor_tracking status)
   {
      if (status == cursor_tracking::leaving && !_cursor_info.element.expired())
      {
         cursor_leaving(ctx, p, _cursor_info);
         return true;
      }

      if (!empty())
      {
         hit_info info = hit_element(ctx, p);
         if (auto ptr = info.element.lock())
         {
            auto cptr = _cursor_info.element.lock();

            // If we're previously tracking an element, send it a 'leaving' message
            if (cptr && cptr != ptr)
               cursor_leaving(ctx, p, _cursor_info);

            if (elements::intersects(info.bounds, view_bounds(ctx.view)))
            {
               context ectx{ ctx, ptr.get(), info.bounds };
               bool r = ptr->cursor(ectx, p, status);
               _cursor_info = info;
               return r;
            }
         }
         else if (!_cursor_info.element.expired())
         {
            cursor_leaving(ctx, p, _cursor_info);
         }
      }

      return false;
   }

   bool composite_base::scroll(context const& ctx, point dir, point p)
   {
      if (!empty())
      {
         hit_info info = hit_element(ctx, p);
         if (auto ptr = info.element.lock(); ptr && elements::intersects(info.bounds, view_bounds(ctx.view)))
         {
            context ectx{ ctx, ptr.get(), info.bounds };
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

   void composite_base::begin_focus()
   {
      if (_focus == -1)
         _focus = _saved_focus;
      if (_focus == -1)
      {
         for (std::size_t ix = 0; ix != size(); ++ix)
            if (at(ix).wants_focus())
            {
               _focus = ix;
               break;
            }
      }
      if (_focus != -1)
         at(_focus).begin_focus();
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

   composite_base::hit_info composite_base::hit_element(context const& ctx, point p) const
   {
      for (std::size_t ix = 0; ix < size(); ++ix)
      {
         auto& e = at(ix);
         if (e.wants_control())
         {
            rect bounds = bounds_of(ctx, ix);
            if (bounds.includes(p))
            {
               context ectx{ ctx, &e, bounds };
               if (e.hit_test(ectx, p))
                  return hit_info{ e.shared_from_this(), bounds, int(ix) };
            }
         }
      }
      return hit_info{ {}, rect{}, -1 };
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
      _drag_tracking = -1;
      _click_info = hit_info{};
      _cursor_info = hit_info{};
   }
}}
