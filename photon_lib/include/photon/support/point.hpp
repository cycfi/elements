/*=============================================================================
   Copyright (c) 2016-2018 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(PHOTON_GUI_LIB_POINT_APRIL_10_2016)
#define PHOTON_GUI_LIB_POINT_APRIL_10_2016

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////
   // Points
   ////////////////////////////////////////////////////////////////////////////
   struct point
   {
      constexpr   point();
      constexpr   point(float x, float y);
                  point(point const&) = default;
      point&      operator=(point const &) = default;

      bool        operator==(point const& other) const;
      bool        operator!=(point const& other) const;

      point       move(float dx, float dy) const;
      point       move_to(float x, float y) const;

      float       x;
      float       y;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Sizes
   ////////////////////////////////////////////////////////////////////////////
   struct size : point
   {
      using point::point;

      point       move(float dx, float dy) const = delete;
      point       move_to(float x, float y) const = delete;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Inlines
   ////////////////////////////////////////////////////////////////////////////
   inline constexpr point::point()
    : x(0.0), y(0.0)
   {}

   inline constexpr point::point(float x, float y)
    : x(x), y(y)
   {}

   inline bool point::operator==(point const& other) const
   {
      return (other.x == x) && (other.y == y);
   }

   inline bool point::operator!=(point const& other) const
   {
      return !(*this == other);
   }

   inline point point::move(float dx, float dy) const
   {
      point r = *this;
      r.x += dx;
      r.y += dy;
      return r;
   }

   inline point point::move_to(float x_, float y_) const
   {
      point r = *this;
      r.x = x_;
      r.y = y_;
      return r;
   }
}
#endif
