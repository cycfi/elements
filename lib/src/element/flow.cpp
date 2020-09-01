/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/flow.hpp>
#include <elements/support/context.hpp>
#include <elements/view.hpp>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Flow Element
   ////////////////////////////////////////////////////////////////////////////
   view_limits flow_element::limits(basic_context const& ctx) const
   {
      view_limits limits_ = { { 0, 0 }, { full_extent, full_extent } };
      if (!_flowable.needs_reflow())
      {
         limits_.min.y = base_type::limits(ctx).min.y;

         for (std::size_t i = 0; i != _flowable.size();  ++i)
         {
            auto el = _flowable.at(i).limits(ctx);
            clamp_min(limits_.min.x, el.min.x);
         }
      }
      return limits_;
   }

   void flow_element::layout(context const& ctx)
   {
      clear();
      _flowable.break_lines(*this, ctx, ctx.bounds.width());
      base_type::layout(ctx);

      if (_flowable.needs_reflow())
      {
         ctx.view.post([&view = ctx.view]{ view.layout(); });
         _flowable.reflow_done();
      }
   }

   void flowable_container::break_lines(
      std::vector<element_ptr>& rows
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

   element_ptr flowable_container::make_row(size_t first, size_t last)
   {
      using htile = range_composite<htile_element>;
      return std::make_shared<htile>(*this, first, last);
   }
}}
