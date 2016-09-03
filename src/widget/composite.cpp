/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/composite.hpp>
#include <photon/support/context.hpp>
#include <photon/view.hpp>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // composite_base class implementation
   ////////////////////////////////////////////////////////////////////////////////////////////////
   namespace
   {
      rect view_bounds(view const& v)
      {
         auto size = v.size();
         return rect{ 0, 0, size.x, size.y };
      }
   }

   widget* composite_base::hit_test(context const& ctx, point p)
   {
      if (!empty())
      {
         hit_info info = hit_element(ctx, p);
         return info.element;
      }
      return 0;
   }

   void composite_base::draw(context const& ctx)
   {
      for (std::size_t i = 0; i < size(); ++i)
      {
         rect bounds = bounds_of(ctx, i);
         if (intersects(bounds, ctx.view.dirty()))
         {
            auto elem = (*this)[i];
            context ectx{ ctx, elem.get(), bounds };
            elem->draw(ectx);
         }
      }
   }

   widget* composite_base::click(context const& ctx, mouse_button btn)
   {
      point p = btn.pos;

      if (!empty())
      {
         hit_info info = (btn.is_pressed)? hit_element(ctx, p) : _click_info;

         if (info.element && focus(focus_request::wants_focus))
         {
            if (_focus != info.index)
            {
               // end the previous focus
               if (_focus != -1)
                  (*this)[_focus]->focus(focus_request::end_focus);

               // start a new focus
               _focus = info.index;
               if (_focus != -1)
                  (*this)[_focus]->focus(focus_request::begin_focus);
            }
         }

         if (info.element)
         {
            _drag_tracking = info.index;
            context ectx{ ctx, info.element, info.bounds };
            if (info.element->click(ectx, btn))
            {
               if (btn.is_pressed)
                  _click_info = info;
               return info.element;
            }
         }
         else
         {
            _click_info.element = 0;
         }
      }
      _drag_tracking = -1;
      return 0;
   }

   void composite_base::drag(context const& ctx, mouse_button btn)
   {
      if (_drag_tracking != -1)
      {
         rect bounds = bounds_of(ctx, _drag_tracking);
         widget* element = (*this)[_drag_tracking].get();
         context ectx{ ctx, element, bounds };
         element->drag(ectx, btn);
      }
   }

//   bool composite_base::key(context const& ctx, key_info const& k)
//   {
//      if (_focus != -1)
//      {
//         rect bounds = bounds_of(ctx, _focus);
//         widget* focus_ptr = (*this)[_focus].get();
//         context ectx{ ctx, focus_ptr, bounds };
//         return focus_ptr->key(ectx, k);
//      };
//
//      return false;
//   }

   bool composite_base::text(context const& ctx, text_info const& info)
   {
      if (_focus != -1)
      {
         rect bounds = bounds_of(ctx, _focus);
         widget* focus_ptr = (*this)[_focus].get();
         context ectx{ ctx, focus_ptr, bounds };
         return focus_ptr->text(ectx, info);
      };

      return false;
   }

   bool composite_base::cursor(context const& ctx, point p, cursor_tracking status)
   {
      if (status == cursor_tracking::leaving && _cursor_info.element)
      {
         context ectx{ ctx, _cursor_info.element, _cursor_info.bounds };
         _cursor_info.element->cursor(ectx, p, status);
         return true;
      }

      if (!empty())
      {
         hit_info info = hit_element(ctx, p);
         if (info.element && photon::intersects(info.bounds, view_bounds(ctx.view)))
         {
            context ectx{ ctx, info.element, info.bounds };
            bool r = info.element->cursor(ectx, p, status);
            if (r)
               _cursor_info = info;
            else
               _cursor_info = hit_info{};
            return r;
         }
      }
      return false;
   }

   bool composite_base::scroll(context const& ctx, point p)
   {
      if (!empty())
      {
         hit_info info = hit_element(ctx, ctx.cursor_pos()); // $$$ fixme $$$
         if (info.element && photon::intersects(info.bounds, view_bounds(ctx.view)))
         {
            context ectx{ ctx, info.element, info.bounds };
            return info.element->scroll(ectx, p);
         }
      }
      return false;
   }

   bool composite_base::focus(focus_request r)
   {
      switch (r)
      {
         case focus_request::wants_focus:
            for (auto element : *this)
               if (element->focus(focus_request::wants_focus))
                  return true;
            return false;

         case focus_request::begin_focus:
            if (_focus != -1)
               (*this)[_focus]->focus(focus_request::begin_focus);
            return true;

         case focus_request::end_focus:
            if (_focus != -1)
               (*this)[_focus]->focus(focus_request::end_focus);
            return true;
      }

      return false;
   }

   widget const* composite_base::focus() const
   {
      return (empty() || (_focus == -1))? 0 : (*this)[_focus].get();
   }

   widget* composite_base::focus()
   {
      return (empty() || (_focus == -1))? 0 : (*this)[_focus].get();
   }

   void composite_base::focus(std::size_t index)
   {
      if (index < size())
         _focus = int(index);
   }

   composite_base::hit_info composite_base::hit_element(context const& ctx, point p) const
   {
      for (std::size_t i = 0; i < size(); ++i)
      {
         widget_ptr e = (*this)[i];
         if (e->is_control())
         {
            rect bounds = bounds_of(ctx, i);
            if (bounds.includes(p))
            {
               context ectx{ ctx, e.get(), bounds };
               if (e->hit_test(ectx, p))
                  return hit_info{ e.get(), bounds, int(i) };
            }
         }
      }
      return hit_info{ 0, rect{}, -1 };
   }

   bool composite_base::is_control() const
   {
      for (auto const& e : *this)
         if (e->is_control())
            return true;
      return false;
   }

   void composite_base::idle(basic_context const& ctx)
   {
      for (auto const& e : *this)
         e->idle(ctx);
   }
}
