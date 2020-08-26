/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <infra/support.hpp>
#include <elements/support/rect.hpp>
#include <algorithm>

namespace cycfi { namespace elements
{
   bool intersects(rect a, rect b)
   {
      if (a.left >= b.right || b.left >= a.right)
         return false;
      if (a.top >= b.bottom || b.top >= a.bottom)
         return false;
      return true;
   }

   rect max(rect a, rect b)
   {
      return {
         std::min(a.left, b.left),
         std::min(a.top, b.top),
         std::max(a.right, b.right),
         std::max(a.bottom, b.bottom)
      };
   }

   rect min(rect a, rect b)
   {
      return {
         std::max(a.left, b.left),
         std::max(a.top, b.top),
         std::min(a.right, b.right),
         std::min(a.bottom, b.bottom)
      };
   }

   rect center(rect r, rect encl)
   {
      float dx = (encl.width() - r.width()) / 2.0;
      float dy = (encl.height() - r.height()) / 2.0;
      r = r.move_to(encl.left, encl.top);
      r = r.move(dx, dy);
      return r;
   }

   rect center_h(rect r, rect encl)
   {
      float dx = (encl.width() - r.width()) / 2.0;
      r = r.move_to(encl.left, r.top);
      r = r.move(dx, 0.0);
      return r;
   }

   rect center_v(rect r, rect encl)
   {
      float dy = (encl.height() - r.height()) / 2.0;
      r = r.move_to(r.left, encl.top);
      r = r.move(0.0, dy);
      return r;
   }

   rect align(rect r, rect encl, float x_align, float y_align)
   {
      r = r.move_to(
         encl.left + ((encl.width() - r.width()) * x_align),
         encl.top + ((encl.height() - r.height()) * y_align)
      );
      return r;
   }

   rect align_h(rect r, rect encl, float x_align)
   {
      r = r.move_to(
         encl.left + ((encl.width() - r.width()) * x_align), r.top
      );
      return r;
   }

   rect align_v(rect r, rect encl, float y_align)
   {
      r = r.move_to(
         r.left, encl.top + ((encl.height() - r.height()) * y_align)
      );
      return r;
   }

   rect clip(rect r, rect encl)
   {
      clamp_min(r.left, encl.left);
      clamp_min(r.top, encl.top);
      clamp_max(r.right, encl.right);
      clamp_max(r.bottom, encl.bottom);
      return r;
   }
}}
