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

   rect widget::limits() const
   {
      return full_limits;
   }

   widget* widget::hit_test(layout_info const& l, point const& p)
   {
      return (l.bounds.includes(p)) ? this : 0;
   }

   void widget::draw(layout_info const& l)
   {
   }

   void widget::layout(rect const& bounds)
   {
   }

   widget* widget::click(layout_info const& l, mouse_button btn)
   {
      return 0;
   }

   void widget::drag(layout_info const& l, mouse_button btn)
   {
   }

   bool widget::key(layout_info const& l, key_info const& k)
   {
      return false;
   }

   bool widget::cursor(layout_info const& l, point const& p)
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

   rect proxy::limits() const
   {
      return _subject->limits();
   }

   widget* proxy::hit_test(layout_info const& l, point const& p)
   {
      l.widget = _subject.get();
      subject_bounds(l.bounds);
      return _subject->hit_test(l, p);
   }

   void proxy::draw(layout_info const& l)
   {
      l.widget = _subject.get();
      subject_bounds(l.bounds);
      _subject->draw(l);
   }

   void proxy::layout(rect const& bounds_)
   {
      rect bounds = bounds_;
      subject_bounds(bounds);
      _subject->layout(bounds);
   }

   void proxy::subject_bounds(rect& b)
   {
   }

   widget* proxy::click(layout_info const& l, mouse_button btn)
   {
      l.widget = _subject.get();
      subject_bounds(l.bounds);
      return _subject->click(l, btn);
   }

   void proxy::drag(layout_info const& l, mouse_button btn)
   {
      l.widget = _subject.get();
      subject_bounds(l.bounds);
      _subject->drag(l, btn);
   }

   bool proxy::key(layout_info const& l, key_info const& k)
   {
      l.widget = _subject.get();
      subject_bounds(l.bounds);
      return _subject->key(l, k);
   }

   bool proxy::cursor(layout_info const& l, point const& p)
   {
      l.widget = _subject.get();
      subject_bounds(l.bounds);
      return _subject->cursor(l, p);
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

   widget* composite::hit_test(layout_info const& l, point const& p)
   {
      if (!_elements.empty())
      {
         hit_info info = hit_element(p);
         if (info.element)
         {
            layout_info elem_layout{ l.app, l.window, info.element, &l, info.bounds };
            return info.element->hit_test(elem_layout, p);
         }
      }
      return 0;
   }

   void composite::draw(layout_info const& l)
   {
      auto w_bounds = window_bounds(l.window);
      for (std::size_t i = 0; i < _elements.size(); ++i)
      {
         rect bounds = bounds_of(i);
         if (intersects(bounds, w_bounds))
         {
            auto elem = _elements[i];
            layout_info elem_layout{ l.app, l.window, elem.get(), &l, bounds };
            elem->draw(elem_layout);
         }
      }
   }

   widget* composite::click(layout_info const& l, mouse_button btn)
   {
      point p = l.cursor_pos();

      if (!_elements.empty())
      {
         hit_info info = hit_element(p);

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
            layout_info elem_layout{ l.app, l.window, info.element, &l, info.bounds };
            if (info.element->click(elem_layout, btn))
               return info.element;
         }
      }
      return 0;
   }

   void composite::drag(layout_info const& l, mouse_button btn)
   {
      hit_info info = hit_element(l.cursor_pos());
      if (info.element)
      {
         layout_info elem_layout{ l.app, l.window, info.element, &l, info.bounds };
         info.element->drag(elem_layout, btn);
      }
   }

   bool composite::key(layout_info const& l, key_info const& k)
   {
      if (_focus != -1)
      {
         rect bounds = bounds_of(_focus);
         widget* focus_ptr = _elements[_focus].get();
         layout_info elem_layout{ l.app, l.window, focus_ptr, &l, bounds };
         return focus_ptr->key(elem_layout, k);
      };

      return false;
   }

   bool composite::cursor(layout_info const& l, point const& p)
   {
      if (!_elements.empty())
      {
         hit_info info = hit_element(p);
         if (info.element && photon::intersects(info.bounds, window_bounds(l.window)))
         {
            layout_info elem_layout{ l.app, l.window, info.element, &l, info.bounds };
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

   composite::hit_info composite::hit_element(point const& p) const
   {
      for (std::size_t i = 0; i < _elements.size(); ++i)
      {
         rect bounds = bounds_of(i);
         if (bounds.includes(p))
            return hit_info{ _elements[i].get(), bounds, int(i) };
      }
      return hit_info{ 0, rect{}, -1 };
   }
}
