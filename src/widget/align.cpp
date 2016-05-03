/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/align.hpp>
#include <algorithm>

namespace photon
{
//   ////////////////////////////////////////////////////////////////////////////////////////////////
//   // halign
//   ////////////////////////////////////////////////////////////////////////////////////////////////
//   rect halign_widget::limits(basic_context const& ctx) const
//   {
//      rect e_limits = subject()->limits(ctx);
//      return rect{ e_limits.left, e_limits.top, full_extent, e_limits.bottom };
//   }
//
//   void halign_widget::prepare_subject(context& ctx)
//   {
//      rect     e_limits          = subject()->limits(ctx);
//      double   elem_width        = e_limits.left;
//      double   available_width   = ctx.bounds.width();
//
//      if (available_width > elem_width)
//         elem_width = std::min(available_width, e_limits.right);
//
//      ctx.bounds.left += (available_width - elem_width) * _align;
//      ctx.bounds.width(elem_width);
//   }
//
//   ////////////////////////////////////////////////////////////////////////////////////////////////
//   // valign
//   ////////////////////////////////////////////////////////////////////////////////////////////////
//   rect valign_widget::limits(basic_context const& ctx) const
//   {
//      rect e_limits = subject()->limits(ctx);
//      return rect{ e_limits.left, e_limits.top, full_extent, e_limits.bottom };
//   }
//
//   void valign_widget::prepare_subject(context& ctx)
//   {
//      rect     e_limits          = subject()->limits(ctx);
//      double   elem_height       = e_limits.top;
//      double   available_height  = ctx.bounds.height();
//
//      if (available_height > elem_height)
//         elem_height = std::min(available_height, e_limits.right);
//
//      ctx.bounds.top += (available_height - elem_height) * _align;
//      ctx.bounds.height(elem_height);
//   }
}
