/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/flow.hpp>
#include <elements/support/context.hpp>
#include <elements/view.hpp>

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Flow Element
   ////////////////////////////////////////////////////////////////////////////
   bool flow_element::stale() const
   {
      return _flowable.needs_reflow() || _flowable.size() != _flowable_size;
   }

   std::size_t flow_element::size() const
   {
      // A row may refer to elements past the container's end.
      return _flowable.size() != _flowable_size? 0 : base_type::size();
   }

   view_limits flow_element::limits(basic_context const& ctx) const
   {
      view_limits limits_ = {{0, 0}, {full_extent, full_extent}};
      if (!stale())
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
      bool const reflow = stale();
      clear();
      _flowable.break_lines(*this, ctx, ctx.bounds.width());
      _flowable_size = _flowable.size();
      if (reflow)
         _flowable.reflow_done();
      base_type::layout(ctx);

      // Lay out the view again, now that the limits follow the new rows.
      if (reflow)
         ctx.view.post([&view = ctx.view]{ view.layout(); });
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

         if (curr_x > width && first != last)
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

   std::string flow_element::class_name() const
   {
      return "flow";
   }
}
