/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/flow.hpp>
#include <photon/support/context.hpp>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Flow Widget
   ////////////////////////////////////////////////////////////////////////////////////////////////
   widget_limits flow_widget::limits(basic_context const& ctx) const
   {
      double max_y = 0;
      for (std::size_t ix = 0; ix != size();  ++ix)
         clamp_min(max_y, at(ix).limits(ctx).min.y);
      return { { 0, 0 }, { full_extent, float(max_y) } };
   }

   void flow_widget::layout(context const& ctx)
   {
      _vtile.clear();

      double      max_x = ctx.bounds.width();
      double      curr_x = 0;
      std::size_t first = 0;
      std::size_t last = 0;

      for (std::size_t ix = 0; ix != size();  ++ix)
      {
         auto&    elem = at(ix);
         double   elem_nat_x = elem.limits(ctx).min.x;
         curr_x = curr_x + elem_nat_x;

         if (curr_x > max_x)
         {
            curr_x = elem_nat_x;
            auto htile_ = std::make_shared<htile>(*this, first, last);
            _vtile.push_back(htile_);
            first = last;
         }

         last++;
      }

      _vtile.layout(ctx);
   }

   rect flow_widget::bounds_of(context const& ctx, std::size_t index) const
   {
      for (std::size_t ix = 0; ix != _vtile.size();  ++ix)
      {
         auto  htile_ = std::dynamic_pointer_cast<htile>(_vtile[ix]);
         auto  size = htile_->size();
         if (size < index)
            return htile_->bounds_of(ctx, index);
         index -= size;
      }

      return {};
   }
}
