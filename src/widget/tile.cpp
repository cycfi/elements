/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/tile.hpp>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Vertical Tiles
   ////////////////////////////////////////////////////////////////////////////////////////////////
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
      double height  = b.height();
      double extra   = limits.bottom - height;
      double m_size  = limits.bottom - limits.top;
      double curr    = b.top;
      auto  i        = _tiles.begin();

      for (auto const& elem : elements())
      {
         rect  limits = elem->limits();

         *i++ = curr;
         auto prev = curr;
         curr += limits.bottom;

         if ((extra != 0) && (m_size != 0))
            curr -= extra * (limits.bottom - limits.top) / m_size;

         elem->layout({ _left, prev, _right, curr });
      }
      *i = curr;
   }

   rect vtile_widget::bounds_of(std::size_t index) const
   {
      return rect{ _left, _tiles[index], _right, _tiles[index+1] };
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Horizontal Tiles
   ////////////////////////////////////////////////////////////////////////////////////////////////
   rect htile_widget::limits() const
   {
      rect limits{ 0.0, 0.0, 0.0, full_extent };
      for (auto const& elem : elements())
      {
         rect  el = elem->limits();

         limits.left += el.left;
         limits.right += el.right;
         min_limit(limits.top, el.top);
         max_limit(limits.bottom, el.bottom);
      }

      return limits;
   }

   void htile_widget::layout(rect const& b)
   {
      _top = b.top;
      _bottom = b.bottom;

      _tiles.resize(elements().size()+1);

      rect  limits   = this->limits();
      double width   = b.width();
      double extra   = limits.right - width;
      double m_size  = limits.right - limits.left;
      double curr    = b.left;
      auto  i        = _tiles.begin();

      for (auto const& elem : elements())
      {
         rect  limits = elem->limits();

         *i++ = curr;
         auto prev = curr;
         curr += limits.right;

         if ((extra != 0) && (m_size != 0))
            curr -= extra * (limits.right - limits.left) / m_size;

         elem->layout({ prev, _top, curr, _bottom });
      }
      *i = curr;
   }

   rect htile_widget::bounds_of(std::size_t index) const
   {
      return rect{ _tiles[index], _top, _tiles[index + 1], _bottom };
   }
}
