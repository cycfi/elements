/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman
   Copyright (c) 2020 Michał Urbański

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/tile.hpp>
#include <elements/support/context.hpp>

#include <algorithm>
#include <numeric>
#include <limits>

namespace cycfi::elements
{
   namespace
   {
      struct layout_info
      {
         float min, max, stretch, alloc;
         std::size_t index;
      };

      bool is_almost_zero(float val, int ulp = 1)
      {
         // unless the result is subnormal
         return val < std::numeric_limits<float>::min() ||
         // the machine epsilon has to be scaled to the magnitude of the values used
         // and multiplied by the desired precision in ULPs (units in the last place)
         val <= std::numeric_limits<float>::epsilon() * val * ulp;
      }

      auto range(layout_info info)
      {
         return info.max - info.min;
      }

      auto density(layout_info info)
      {
         auto r = range(info);

         if (r <= 0.0f || is_almost_zero(r))
            return std::numeric_limits<float>::max();
         else
            return info.stretch / r;
      }

      // Distribute space proportionally to each element stretchiness.
      // Each element will get at least stretch / sum_stretch * free_space,
      // but may get more if other elements reach their max size.
      void allocate(float space, std::vector<layout_info>& elements)
      {
         auto const sum_min = std::accumulate(elements.begin(), elements.end(), 0.0f,
            [](double sum, layout_info elem){ return sum + elem.min; });

         if (sum_min >= space)
            return;

         // Sort elements by "density". If any elements would reach their max size,
         // then they will be first. This simplifies the algorithm from O(n * n)
         // to O(n log n) because any remaining free space can be redistributed
         // according to the new sum of stretch proportions without having to redo
         // space allocations for previous elements.
         std::sort(elements.begin(), elements.end(),
            [](layout_info lhs, layout_info rhs){ return density(lhs) > density(rhs); });
         auto sum_stretch = std::accumulate(elements.begin(), elements.end(), 0.0f,
            [](double sum, layout_info elem){ return sum + elem.stretch; });
         auto free_space = space - sum_min;

         for (auto& e : elements)
         {
            if (sum_stretch <= 0.0f || is_almost_zero(sum_stretch))
               break;

            auto const alloc = e.stretch / sum_stretch * free_space;
            auto const r = range(e);
            if (alloc >= r)
            {
               e.alloc += r;
               sum_stretch -= e.stretch;
               free_space -= r;
            }
            else
            {
               e.alloc += alloc;
            }
         }

         // retain initial order needed for further calculations
         std::sort(elements.begin(), elements.end(),
            [](layout_info lhs, layout_info rhs){ return lhs.index < rhs.index; });
      }

     template<axis Axis, class TileElement, axis OtherAxis = other(Axis)>
     CYCFI_FORCE_INLINE auto compute_limits(basic_context const& ctx, const TileElement &tile) -> view_limits
     {
       view_limits limits{{0.0, 0.0},
                          {Axis == axis::x ? 0.0 : full_extent,
                           Axis == axis::x ? full_extent : 0.0}};
       for (std::size_t i = 0; i != tile.size();  ++i)
         {
           auto el = tile.at(i).limits(ctx);

           limits.min[Axis] += el.min[Axis];
           limits.max[Axis] += el.max[Axis];
           clamp_min(limits.min[OtherAxis], el.min[OtherAxis]);
           clamp_max(limits.max[OtherAxis], el.max[OtherAxis]);
         }

       clamp_min(limits.max[OtherAxis], limits.min[OtherAxis]);
       clamp_max(limits.max[Axis], full_extent);
       return limits;
     }

     template<axis Axis, class TileElement, axis OtherAxis = other(Axis)>
     CYCFI_FORCE_INLINE auto compute_layout(context const& ctx, TileElement &tile, std::vector<float> &tile_offsets) -> void
     {
       auto const sz = tile.size();

       // Collect min, max, and stretch information from each element.
       // Initially set the allocation sizes of each element to its minimum.
       std::vector<layout_info> info(sz);
       for (std::size_t i = 0; i != sz; ++i)
       {
         auto& elem = tile.at(i);
         auto limits = elem.limits(ctx);
         info[i].stretch = elem.stretch()[Axis];
         info[i].min = limits.min[Axis];
         info[i].max = limits.max[Axis];
         info[i].alloc = limits.min[Axis];
         info[i].index = i;
       }

       auto const other_axis_min = axis_min(ctx.bounds, OtherAxis);
       auto const other_axis_max = axis_max(ctx.bounds, OtherAxis);
       auto const my_axis_min = axis_min(ctx.bounds, Axis);
       auto const my_axis_delta = axis_delta(ctx.bounds, Axis);
       // Compute the best fit for all elements
       allocate(my_axis_delta, info);

       // Now we have the final layout. We can now layout the individual
       // elements.
       tile_offsets.resize(sz);
       auto curr = 0.0f;
       for (std::size_t i = 0; i != sz; ++i)
       {
         tile_offsets[i] = curr + info[i].alloc;
         auto const prev = curr;
         curr += info[i].alloc;

         auto& elem = tile.at(i);
         auto ebounds = make_rect(Axis, prev+my_axis_min, other_axis_min, curr+my_axis_min, other_axis_max);

         elem.layout(context{ctx, &elem, ebounds});
       }
     }

     template<axis Axis, axis OtherAxis = other(Axis)>
     CYCFI_FORCE_INLINE auto compute_bounds_of(rect const& bounds, std::size_t index, const std::vector<float> &tile_offsets) -> rect
     {
       if (index >= tile_offsets.size())
         return {};
       auto const other_axis_min = axis_min(bounds, OtherAxis);
       auto const other_axis_max = axis_max(bounds, OtherAxis);
       auto const my_axis_min = axis_min(bounds, Axis);
       return make_rect(Axis, (index? tile_offsets[index-1] : 0)+my_axis_min, other_axis_min, tile_offsets[index]+my_axis_min, other_axis_max);
     }
   }

   ////////////////////////////////////////////////////////////////////////////
   // Vertical Tiles
   ////////////////////////////////////////////////////////////////////////////
   view_limits vtile_element::limits(basic_context const& ctx) const
   {
      return compute_limits<axis::y>(ctx, *this);
   }

   void vtile_element::layout(context const& ctx)
   {
     compute_layout<axis::y>(ctx, *this, _tiles);
   }

   void vtile_element::draw(context const& ctx)
   {
      if (_tiles.empty())
         layout(ctx);
      composite_base::draw(ctx);
   }

   rect vtile_element::bounds_of(context const& ctx, std::size_t index) const
   {
      return compute_bounds_of<axis::y>(ctx.bounds, index, _tiles);
   }

   ////////////////////////////////////////////////////////////////////////////
   // Horizontal Tiles
   ////////////////////////////////////////////////////////////////////////////
   view_limits htile_element::limits(basic_context const& ctx) const
   {
      return compute_limits<axis::x>(ctx, *this);
   }

   void htile_element::layout(context const& ctx)
   {
      compute_layout<axis::x>(ctx, *this, _tiles);
   }

   void htile_element::draw(context const& ctx)
   {
      if (_tiles.empty())
         layout(ctx);
      composite_base::draw(ctx);
   }

   rect htile_element::bounds_of(context const& ctx, std::size_t index) const
   {
      return compute_bounds_of<axis::x>(ctx.bounds, index, _tiles);
   }
}
