/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/widget.hpp>
#include <photon/window.hpp>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // widget class implementation
   ////////////////////////////////////////////////////////////////////////////////////////////////

   rect widget::limits(theme const& th) const
   {
      return full_limits;
   }

   widget* widget::hit_test(context const& ctx, point const& p)
   {
      return (ctx.bounds.includes(p)) ? this : 0;
   }

   void widget::draw(context const& ctx)
   {
   }

   void widget::layout(theme const& th, rect const& bounds)
   {
   }

   widget* widget::click(context const& ctx, mouse_button btn)
   {
      return 0;
   }

   void widget::drag(context const& ctx, mouse_button btn)
   {
   }

   bool widget::key(context const& ctx, key_info const& k)
   {
      return false;
   }

   bool widget::cursor(context const& ctx, point const& p)
   {
      return false;
   }

   bool widget::focus(focus_request r)
   {
      return r == focus_request::end_focus;
   }

   widget const* widget::focus() const
   {
      return this;
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // proxy class implementation
   ////////////////////////////////////////////////////////////////////////////////////////////////

   rect proxy::limits(theme const& th) const
   {
      return _subject->limits(th);
   }

   widget* proxy::hit_test(context const& ctx, point const& p)
   {
      ctx.widget = _subject.get();
      subject_bounds(ctx.theme(), ctx.bounds);
      return _subject->hit_test(ctx, p);
   }

   void proxy::draw(context const& ctx)
   {
      ctx.widget = _subject.get();
      subject_bounds(ctx.theme(), ctx.bounds);
      _subject->draw(ctx);
   }

   void proxy::layout(theme const& th, rect const& bounds_)
   {
      rect bounds = bounds_;
      subject_bounds(th, bounds);
      _subject->layout(th, bounds);
   }

   void proxy::subject_bounds(theme const& th, rect& b)
   {
   }

   widget* proxy::click(context const& ctx, mouse_button btn)
   {
      ctx.widget = _subject.get();
      subject_bounds(ctx.theme(), ctx.bounds);
      return _subject->click(ctx, btn);
   }

   void proxy::drag(context const& ctx, mouse_button btn)
   {
      ctx.widget = _subject.get();
      subject_bounds(ctx.theme(), ctx.bounds);
      _subject->drag(ctx, btn);
   }

   bool proxy::key(context const& ctx, key_info const& k)
   {
      ctx.widget = _subject.get();
      subject_bounds(ctx.theme(), ctx.bounds);
      return _subject->key(ctx, k);
   }

   bool proxy::cursor(context const& ctx, point const& p)
   {
      ctx.widget = _subject.get();
      subject_bounds(ctx.theme(), ctx.bounds);
      return _subject->cursor(ctx, p);
   }

   bool proxy::focus(focus_request r)
   {
      return _subject->focus(r);
   }

   widget const* proxy::focus() const
   {
      return _subject->focus();
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // composite class implementation
   ////////////////////////////////////////////////////////////////////////////////////////////////
   namespace
   {
      rect window_bounds(window const& w)
      {
         auto size = w.size();
         return rect{ 0, 0, size.x, size.y };
      }
   }

   widget* composite::hit_test(context const& ctx, point const& p)
   {
      if (!_elements.empty())
      {
         hit_info info = hit_element(ctx.theme(), p);
         if (info.element)
         {
            context elem_layout{ ctx.app, ctx.window, info.element, &ctx, info.bounds };
            return info.element->hit_test(elem_layout, p);
         }
      }
      return 0;
   }

   void composite::draw(context const& ctx)
   {
      auto w_bounds = window_bounds(ctx.window);
      for (std::size_t i = 0; i < _elements.size(); ++i)
      {
         rect bounds = bounds_of(ctx.theme(), i);
         if (intersects(bounds, w_bounds))
         {
            auto elem = _elements[i];
            context elem_layout{ ctx.app, ctx.window, elem.get(), &ctx, bounds };
            elem->draw(elem_layout);
         }
      }
   }

   widget* composite::click(context const& ctx, mouse_button btn)
   {
      point p = ctx.cursor_pos();

      if (!_elements.empty())
      {
         hit_info info = hit_element(ctx.theme(), p);

         if (info.element && focus(focus_request::wants_focus))
         {
            if (_focus != info.index)
            {
               // end the previous focus
               if (_focus != -1)
                  _elements[_focus]->focus(focus_request::end_focus);

               // start a new focus
               _focus = info.index;
               if (_focus != -1)
                  _elements[_focus]->focus(focus_request::begin_focus);
            }
         }

         if (info.element)
         {
            context elem_layout{ ctx.app, ctx.window, info.element, &ctx, info.bounds };
            if (info.element->click(elem_layout, btn))
               return info.element;
         }
      }
      return 0;
   }

   void composite::drag(context const& ctx, mouse_button btn)
   {
      hit_info info = hit_element(ctx.theme(), ctx.cursor_pos());
      if (info.element)
      {
         context elem_layout{ ctx.app, ctx.window, info.element, &ctx, info.bounds };
         info.element->drag(elem_layout, btn);
      }
   }

   bool composite::key(context const& ctx, key_info const& k)
   {
      if (_focus != -1)
      {
         rect bounds = bounds_of(ctx.theme(), _focus);
         widget* focus_ptr = _elements[_focus].get();
         context elem_layout{ ctx.app, ctx.window, focus_ptr, &ctx, bounds };
         return focus_ptr->key(elem_layout, k);
      };

      return false;
   }

   bool composite::cursor(context const& ctx, point const& p)
   {
      if (!_elements.empty())
      {
         hit_info info = hit_element(ctx.theme(), p);
         if (info.element && photon::intersects(info.bounds, window_bounds(ctx.window)))
         {
            context elem_layout{ ctx.app, ctx.window, info.element, &ctx, info.bounds };
            return info.element->cursor(elem_layout, p);
         }
      }
      return false;
   }

   bool composite::focus(focus_request r)
   {
      switch (r) {

         case focus_request::wants_focus:
            for (auto element : _elements)
               if (element->focus(focus_request::wants_focus))
                  return true;
            return false;

         case focus_request::begin_focus:
            if (_focus != -1)
               _elements[_focus]->focus(focus_request::begin_focus);
            return true;

         case focus_request::end_focus:
            if (_focus != -1)
               _elements[_focus]->focus(focus_request::end_focus);
            return true;
      }

      return false;
   }

   widget const* composite::focus() const
   {
      return _elements.empty()? 0 : _elements[_focus].get();
   }

   composite::hit_info composite::hit_element(theme const& th, point const& p) const
   {
      for (std::size_t i = 0; i < _elements.size(); ++i)
      {
         rect bounds = bounds_of(th, i);
         if (bounds.includes(p))
            return hit_info{ _elements[i].get(), bounds, int(i) };
      }
      return hit_info{ 0, rect{}, -1 };
   }
}
