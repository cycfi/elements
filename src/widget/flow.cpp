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
      widget_limits limits_ = { { 0, 0 }, { full_extent, full_extent } };
      if (_laid_out)
         limits_.min.y = base_type::limits(ctx).min.y;
      return limits_;
   }

   void flow_widget::layout(context const& ctx)
   {
      using htile = range_composite<htile_widget>;
      clear();

      double      max_x = ctx.bounds.width();
      double      curr_x = 0;
      std::size_t first = 0;
      std::size_t last = 0;

      for (std::size_t ix = 0; ix != _container.size();  ++ix)
      {
         auto&    elem = _container.at(ix);
         double   elem_nat_x = elem.limits(ctx).min.x;
         curr_x = curr_x + elem_nat_x;

         if (curr_x > max_x)
         {
            curr_x = elem_nat_x;
            push_back(std::make_shared<htile>(_container, first, last));
            first = last;
         }

         last++;
      }

      if (first != last)
         push_back(std::make_shared<htile>(_container, first, last));

      base_type::layout(ctx);
      _laid_out = true;
   }
}
