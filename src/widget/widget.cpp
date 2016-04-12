/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/widget.hpp>

namespace photon
{
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

   widget* widget::click(layout_info const& l, point const& p)
   {
      return 0;
   }

   bool widget::key(layout_info const& l, key_info const& k)
   {
   	return false;
   }

   bool widget::cursor(layout_info const& l, point const& p)
   {
   	return false;
   }

   bool
   widget::focus(focus_request r)
   {
      return r == focus_request::end_focus;
   }

   widget const* widget::focus() const
   {
      return this;
   }

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
      return _subject->draw(l);
   }

   void proxy::layout(rect const& bounds_)
   {
      rect bounds = bounds_;
      subject_bounds(bounds);
      return _subject->layout(bounds);
   }

   void proxy::subject_bounds(rect& b)
   {
   }

   widget* proxy::click(layout_info const& l, point const& p)
   {
      l.widget = _subject.get();
      subject_bounds(l.bounds);
      return _subject->click(l, p);
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
}
