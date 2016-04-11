/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget.hpp>

namespace photon
{
   size_limits widget::limits() const
   {
      return full_limits;
   }

   widget* widget::hit_test(layout_info& l, point const& p)
   {
      return (l.bounds.includes(p)) ? this : 0;
   }

   void widget::draw(layout_info& l)
   {
   }

   void widget::layout(rect const& bounds)
   {
   }

   widget* widget::click(layout_info& l, point const& p)
   {
      return 0;
   }

   bool widget::key(layout_info& l, key_info const& k)
   {
   	return false;
   }

   bool widget::cursor(layout_info& l, point const& p)
   {
   	return false;
   }
}

