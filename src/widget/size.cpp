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
   // size
   ////////////////////////////////////////////////////////////////////////////////////////////////
   rect size_widget::limits(context const& ctx) const
   {
      return rect{ _size.x, _size.y, _size.x, _size.y };
   }

   void size_widget::prepare_subject(context const& ctx)
   {
      ctx.bounds.right = ctx.bounds.left + _size.x;
      ctx.bounds.bottom = ctx.bounds.top + _size.y;
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // hsize
   ////////////////////////////////////////////////////////////////////////////////////////////////
   rect hsize_widget::limits(context const& ctx) const
   {
      rect e_limits = subject()->limits(ctx);
      return rect{ _width, e_limits.top, _width, e_limits.bottom };
   }

   void hsize_widget::prepare_subject(context const& ctx)
   {
      ctx.bounds.right = ctx.bounds.left + _width;
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // vsize
   ////////////////////////////////////////////////////////////////////////////////////////////////
   rect vsize_widget::limits(context const& ctx) const
   {
      rect e_limits = subject()->limits(ctx);
      return rect{ _height, e_limits.top, _height, e_limits.bottom };
   }

   void vsize_widget::prepare_subject(context const& ctx)
   {
      ctx.bounds.bottom = ctx.bounds.top + _height;
   }
}
