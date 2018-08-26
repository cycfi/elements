/*=============================================================================
   Copyright (c) 2016-2018 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(PHOTON_GUI_LIB_RECT_APRIL_10_2016)
#define PHOTON_GUI_LIB_RECT_APRIL_10_2016

#include <photon/support/point.hpp>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////
   // rect
   ////////////////////////////////////////////////////////////////////////////
   struct rect
   {
      constexpr   rect();
      constexpr   rect(float left, float top, float right, float bottom);
      constexpr   rect(point origin, float right, float bottom)
                   : rect(origin.x, origin.y, right, bottom)
                  {}
      constexpr   rect(float left, float top, size size_)
                   : rect(left, top, left + size_.x, top + size_.y)
                  {}
      constexpr   rect(point origin, size size_)
                   : rect(origin.x, origin.y, origin.x + size_.x, origin.y + size_.y)
                  {}

                  rect(rect const &) = default;
      rect&       operator=(rect const&) = default;

      bool        operator==(rect const& other) const;
      bool        operator!=(rect const& other) const;

      bool        is_empty() const;
      bool        includes(point p) const;
      bool        includes(rect other) const;

      float       width() const;
      void        width(float width_);
      float       height() const;
      void        height(float height_);

      point       top_left() const;
      point       bottom_right() const;
      point       top_right() const;
      point       bottom_left() const;

      rect        move(float dx, float dy) const;
      rect        move_to(float x, float y) const;
      rect        inset(float x_inset = 1.0, float y_inset = 1.0) const;

      float       left;
      float       top;
      float       right;
      float       bottom;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Free Functions
   ////////////////////////////////////////////////////////////////////////////
   bool     is_valid(rect r);
   bool     is_same_size(rect a, rect b);
   bool     intersects(rect a, rect b);

   point    center_point(rect r);
   float    area(rect r);
   rect     max(rect a, rect b);
   rect     min(rect a, rect b);

   void     clear(rect& r);
   rect     center(rect r, rect encl);
   rect     center_h(rect r, rect encl);
   rect     center_v(rect r, rect encl);
   rect     align(rect r, rect encl, float x_align, float y_align);
   rect     align_h(rect r, rect encl, float x_align);
   rect     align_v(rect r, rect encl, float y_align);
   rect     clip(rect r, rect encl);

   ////////////////////////////////////////////////////////////////////////////
   // Inlines
   ////////////////////////////////////////////////////////////////////////////
   inline constexpr rect::rect()
    : left(0.0), top(0.0), right(0.0), bottom(0.0)
   {}

   inline constexpr rect::rect(float left, float top, float right, float bottom)
    : left(left), top(top), right(right), bottom(bottom)
   {}

   inline bool rect::operator==(rect const& other) const
   {
      return
         (top == other.top) && (bottom == other.bottom) &&
         (left == other.left) && (right == other.right)
         ;
   }

   inline bool rect::operator!=(rect const& other) const
   {
      return !(*this == other);
   }

   inline bool rect::is_empty() const
   {
      return (left == right) || (top == bottom);
   }

   inline bool rect::includes(point p) const
   {
      return
         (p.x >= left) && (p.x <= right) &&
         (p.y >= top) && (p.y <= bottom)
         ;
   }

   inline bool rect::includes(rect other) const
   {
      return
         (other.left >= left) && (other.left <= right) &&
         (other.top >= top) && (other.top <= bottom) &&
         (other.right >= left) && (other.right <= right) &&
         (other.bottom >= top) && (other.bottom <= bottom)
         ;
   }

   inline float rect::width() const
   {
      return right - left;
   }

   inline void rect::width(float width_)
   {
      right = left + width_;
   }

   inline float rect::height() const
   {
      return (bottom - top);
   }

   inline void rect::height(float height_)
   {
      bottom = top + height_;
   }

   inline point rect::top_left() const
   {
      return { left, top };
   }

   inline point rect::bottom_right() const
   {
      return { right, bottom };
   }

   inline point rect::top_right() const
   {
      return { right, top };
   }

   inline point rect::bottom_left() const
   {
      return { left, bottom };
   }

   inline rect rect::move(float dx, float dy) const
   {
      rect r = *this;
      r.top += dy;
      r.left += dx;
      r.bottom += dy;
      r.right += dx;
      return r;
   }

   inline rect rect::move_to(float x, float y) const
   {
      return move(x-left, y-top);
   }

   inline rect rect::inset(float x_inset, float y_inset) const
   {
      rect r = *this;
      r.top += y_inset;
      r.left += x_inset;
      r.bottom -= y_inset;
      r.right -= x_inset;

      if (!is_valid(r))
         clear(r);

      return r;
   }

   inline bool is_valid(rect r)
   {
      return (r.left <= r.right) && (r.top <= r.bottom);
   }

   inline bool is_same_size(rect a, rect b)
   {
      return (a.width() == b.width()) && (a.height() == b.height());
   }

   inline point center_point(rect r)
   {
      return { r.left + (r.width() / 2.0f), r.top + (r.height() / 2.0f) };
   }

   inline float area(rect r)
   {
      return r.width() * r.height();
   }

   inline void clear(rect& r)
   {
      r.left = 0.0;
      r.top  = 0.0;
      r.right = 0.0;
      r.bottom = 0.0;
   }
}
#endif
