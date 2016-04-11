/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/margin.hpp>

namespace photon
{
   rect margin::limits() const
   {
      auto r = subject->limits();

      r.left += margin_.left + margin_.right;
      r.right += margin_.left + margin_.right;
      r.top += margin_.top + margin_.bottom;
      r.bottom += margin_.top + margin_.bottom;

      return r;
   }

   void margin::draw(layout_info const& l)
   {
      auto bounds = l.bounds;
      bounds.top += margin_.top;
      bounds.left += margin_.left;
      bounds.bottom -= margin_.bottom;
      bounds.right -= margin_.right;

      subject->draw(layout_info{ l.app, l.window, &l, bounds });
   }
}
