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
      clear();
      _break_f(_container, *this, ctx, ctx.bounds.width());
      base_type::layout(ctx);
      _laid_out = true;
   }

   namespace
   {
      void break_into_htiles(
        container& container_
      , std::vector<widget_ptr>& rows
      , basic_context const& ctx
      , float width
     )
     {
         using htile = range_composite<htile_widget>;

         double      curr_x = 0;
         std::size_t first = 0;
         std::size_t last = 0;

         for (std::size_t ix = 0; ix != container_.size();  ++ix)
         {
            auto&    elem = container_.at(ix);
            double   elem_nat_x = elem.limits(ctx).min.x;
            curr_x = curr_x + elem_nat_x;

            if (curr_x > width)
            {
               curr_x = elem_nat_x;
               rows.push_back(std::make_shared<htile>(container_, first, last));
               first = last;
            }

            last++;
         }

         if (first != last)
            rows.push_back(std::make_shared<htile>(container_, first, last));
     }
   }

   flow_widget::break_function flow_widget::default_break_function()
   {
      return break_into_htiles;
   }
}
