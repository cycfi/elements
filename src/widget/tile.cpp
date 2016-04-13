/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/tile.hpp>

namespace photon
{
   rect vtile_widget::limits() const
   {
      rect limits{ 0.0, 0.0, full_extent, 0.0 };
      for (auto const& elem : elements())
      {
         rect  el = elem->limits();

         limits.top += el.top;
         limits.bottom += el.bottom;
         min_limit(limits.left, el.left);
         max_limit(limits.right, el.right);
      }

      return limits;
   }

   void vtile_widget::layout(rect const& b)
   {
      _left = b.left;
      _right = b.right;

      _tiles.resize(elements().size()+1);

      rect  limits   = this->limits();
      float height   = b.height();
      float extra    = limits.bottom - height;
      float m_size   = limits.bottom - limits.top;
      float curr     = b.top;
      auto  i        = _tiles.begin();

      for (auto const& elem : elements())
      {
         rect  limits = elem->limits();

         *i++ = curr;
         curr += limits.bottom;

         if ((extra != 0) && (m_size != 0))
            curr -= extra * (limits.bottom - limits.top) / m_size;
      }
      *i = curr;
   }

   rect vtile_widget::bounds_of(std::size_t index) const
   {
      return rect{ _left, _tiles[index], _right, _tiles[index+1] };
   }
}
