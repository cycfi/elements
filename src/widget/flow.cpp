/*=======================================================================================
   Copyright (c) 2016 Joel de Guzman

   Distributed under the The MIT License (https://opensource.org/licenses/MIT)
=======================================================================================*/
#include <photon/widget/flow.hpp>
#include <photon/support/context.hpp>

namespace photon
{
   //////////////////////////////////////////////////////////////////////////////////////
   // Flow Widget
   //////////////////////////////////////////////////////////////////////////////////////
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
      _flowable.break_lines(*this, ctx, ctx.bounds.width());
      base_type::layout(ctx);
      _laid_out = true;
   }

   void flowable_container::break_lines(
      std::vector<widget_ptr>& rows
    , basic_context const& ctx
    , float width
   )
   {
      double      curr_x = 0;
      std::size_t first = 0;
      std::size_t last = 0;

      for (std::size_t ix = 0; ix != size();  ++ix)
      {
         double   elem_nat_x = width_of(ix, ctx);
         curr_x = curr_x + elem_nat_x;

         if (curr_x > width)
         {
            curr_x = elem_nat_x;
            rows.push_back(make_row(first, last));
            first = last;
         }

         last++;
      }

      if (first != last)
         rows.push_back(make_row(first, last));
   }

   float flowable_container::width_of(size_t index, basic_context const& ctx) const
   {
      return at(index).limits(ctx).min.x;
   }

   widget_ptr flowable_container::make_row(size_t first, size_t last)
   {
      using htile = range_composite<htile_widget>;
      return std::make_shared<htile>(*this, first, last);
   }
}
