/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/align.hpp>
#include <algorithm>

namespace photon
{
   rect halign_widget::limits() const
   {
      rect e_limits = subject()->limits();
      return rect{ e_limits.left, e_limits.top, full_extent, e_limits.bottom };
   }

   void halign_widget::subject_bounds(rect& b)
   {
      rect     e_limits          = subject()->limits();
      double   elem_width        = e_limits.left;
      double   available_width   = b.width();

      if (available_width > elem_width)
         elem_width = std::min(available_width, e_limits.right);

      b.left += (available_width - elem_width) * _align;
      b.width(elem_width);
   }
}
