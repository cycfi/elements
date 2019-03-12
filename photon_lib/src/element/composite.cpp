/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <photon/element/composite.hpp>
#include <photon/support/context.hpp>
#include <photon/view.hpp>

namespace cycfi { namespace photon
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
         return info.element;
      }
      return 0;
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

   void composite_base::refresh(context const& ctx, element& element)
   {
      if (&element == this)
      {
         ctx.view.refresh(ctx);
      }
      else
      {
         for (std::size_t ix = 0; ix < size(); ++ix)
         {
            rect bounds = bounds_of(ctx, ix);
            auto& e = at(ix);
            context ectx{ ctx, &e, bounds };
            e.refresh(ectx, element);
         }
      }
   }

   element* composite_base::click(context const& ctx, mouse_button btn)
   {
      point p = btn.pos;

      if (!empty())
      {
         hit_info info = (btn.down)? hit_element(ctx, p) : _click_info;

         if (info.element && focus(focus_request::wants_focus))
         {
            if (_focus != info.index)
            {
               // end the previous focus
               if (_focus != -1)
                  at(_focus).focus(focus_request::end_focus);

               // start a new focus
               _focus = info.index;
               if (_focus != -1)
                  at(_focus).focus(focus_request::begin_focus);
            }
         }

         if (info.element)
         {
            _drag_tracking = info.index;
            context ectx{ ctx, info.element, info.bounds };
            if (info.element->click(ectx, btn))
            {
               if (btn.down)
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
         rect  bounds = bounds_of(ctx, _drag_tracking);
         auto& e = at(_drag_tracking);
         context ectx{ ctx, &e, bounds };
         e.drag(ectx, btn);
      }
   }

   bool composite_base::key(context const& ctx, key_info k)
   {
      if (_focus != -1)
      {
         rect  bounds = bounds_of(ctx, _focus);
         auto& focus_ = at(_focus);
         context ectx{ ctx, &focus_, bounds };
         return focus_.key(ectx, k);
      };

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
         context ectx{ ctx, _cursor_info.element, _cursor_info.bounds };
         _cursor_info.element->cursor(ectx, p, cursor_tracking::leaving);
         _cursor_info = composite_base::hit_info{};
      };
   }

   bool composite_base::cursor(context const& ctx, point p, cursor_tracking status)
   {
      if (status == cursor_tracking::leaving && _cursor_info.element)
      {
         cursor_leaving(ctx, p, _cursor_info);
         return true;
      }

      if (!empty())
      {
         hit_info info = hit_element(ctx, p);
         if (info.element && photon::intersects(info.bounds, view_bounds(ctx.view)))
         {
            // If we're previously tracking an element, send it a 'leaving' message
            if (_cursor_info.element && _cursor_info.element != info.element)
               cursor_leaving(ctx, p, _cursor_info);

            context ectx{ ctx, info.element, info.bounds };
            bool r = info.element->cursor(ectx, p, status);
            if (r)
            {
               _cursor_info = info;
            }
            else
            {
               // If element does not want the cursor, send it a 'leaving' message immediately
               if (info.element && _cursor_info.element != info.element)
                  cursor_leaving(ctx, p, info);
               _cursor_info = hit_info{};
            }
            return r;
         }
      }

      return false;
   }

   bool composite_base::scroll(context const& ctx, point dir, point p)
   {
      if (!empty())
      {
         hit_info info = hit_element(ctx, p);
         if (info.element && photon::intersects(info.bounds, view_bounds(ctx.view)))
         {
            context ectx{ ctx, info.element, info.bounds };
            return info.element->scroll(ectx, dir, p);
         }
      }
      return false;
   }

   bool composite_base::focus(focus_request r)
   {
      switch (r)
      {
         case focus_request::wants_focus:
            for (std::size_t ix = 0; ix != size();  ++ix)
               if (at(ix).focus(focus_request::wants_focus))
                  return true;
            return false;

         case focus_request::begin_focus:
            if (_focus != -1)
               at(_focus).focus(focus_request::begin_focus);
            return true;

         case focus_request::end_focus:
            if (_focus != -1)
               at(_focus).focus(focus_request::end_focus);
            return true;
      }

      return false;
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
         if (e.is_control())
         {
            rect bounds = bounds_of(ctx, ix);
            if (bounds.includes(p))
            {
               context ectx{ ctx, &e, bounds };
               if (e.hit_test(ectx, p))
                  return hit_info{ &e, bounds, int(ix) };
            }
         }
      }
      return hit_info{ 0, rect{}, -1 };
   }

   bool composite_base::is_control() const
   {
      for (std::size_t ix = 0; ix < size(); ++ix)
         if (at(ix).is_control())
            return true;
      return false;
   }

   void composite_base::idle(basic_context const& ctx)
   {
      for (std::size_t ix = 0; ix < size(); ++ix)
         at(ix).idle(ctx);
   }
}}
