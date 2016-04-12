/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/margin.hpp>

namespace photon
{
   rect margin_widget::limits() const
   {
      auto r = subject()->limits();

      r.left += _margin.left + _margin.right;
      r.right += _margin.left + _margin.right;
      r.top += _margin.top + _margin.bottom;
      r.bottom += _margin.top + _margin.bottom;

      return r;
   }

   void margin_widget::subject_bounds(rect& b)
   {
      b.top += _margin.top;
      b.left += _margin.left;
      b.bottom -= _margin.bottom;
      b.right -= _margin.right;
   }
}
