/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/layer.hpp>

namespace photon
{
   rect layer_widget::limits() const
   {
      rect limits{ 0.0, 0.0, full_extent, full_extent };
      for (auto const& elem : elements())
      {
         rect  el = elem->limits();

         min_limit(limits.left, el.left);
         min_limit(limits.top, el.top);
         max_limit(limits.right, el.right);
         max_limit(limits.bottom, el.bottom);
      }

      return limits;
   }

   void layer_widget::layout(rect const& b)
   {
      bounds = b;
      for (std::size_t i = 0; i != elements().size(); ++i)
         elements()[i]->layout(bounds_of(i));
   }

   layer_widget::hit_info layer_widget::hit_element(point const& p) const
   {
      // we test from the highest index (topmost element)
      for (int i = elements().size()-1; i >= 0; --i)
      {
         rect bounds = bounds_of(i);
         if (bounds.includes(p))
            return hit_info{ elements()[i].get(), bounds, int(i) };
      }
      return hit_info{ 0, rect{}, -1 };
   }


   rect layer_widget::bounds_of(std::size_t index) const
   {
      double   width = bounds.width();
      double   height = bounds.height();
      rect     limits = elements()[index]->limits();

      min_limit(width, limits.left);
      max_limit(width, limits.right);
      min_limit(height, limits.top);
      max_limit(height, limits.bottom);

      return { bounds.left, bounds.top, bounds.left + width, bounds.top + height };
   }
}
