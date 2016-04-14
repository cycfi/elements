/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/size.hpp>
#include <algorithm>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // hsize
   ////////////////////////////////////////////////////////////////////////////////////////////////
   rect hsize_widget::limits() const
   {
      rect e_limits = subject()->limits();
      return rect{ _width, e_limits.top, _width, e_limits.bottom };
   }

   void hsize_widget::subject_bounds(rect& b)
   {
      b.right = b.left + _width;
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // vsize
   ////////////////////////////////////////////////////////////////////////////////////////////////
   rect vsize_widget::limits() const
   {
      rect e_limits = subject()->limits();
      return rect{ _height, e_limits.top, _height, e_limits.bottom };
   }

   void vsize_widget::subject_bounds(rect& b)
   {
      b.right = b.left + _height;
   }
}
