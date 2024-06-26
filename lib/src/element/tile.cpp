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

     template<bool IS_X_AXIS, class TILE_ELEMENT>
     [[gnu::always_inline]] inline auto compute_limits(basic_context const& ctx, const TILE_ELEMENT &tile) -> view_limits
     {
      view_limits limits{{0.0, 0.0},
                         {IS_X_AXIS ? 0.0 : full_extent,
                          IS_X_AXIS ? full_extent : 0.0}};
      for (std::size_t i = 0; i != tile.size();  ++i)
      {
         auto el = tile.at(i).limits(ctx);

         limits.min.coord(IS_X_AXIS) += el.min.coord(IS_X_AXIS);
         limits.max.coord(IS_X_AXIS) += el.max.coord(IS_X_AXIS);
         clamp_min(limits.min.coord(! IS_X_AXIS), el.min.coord(! IS_X_AXIS));
         clamp_max(limits.max.coord(! IS_X_AXIS), el.max.coord(! IS_X_AXIS));
      }

      clamp_min(limits.max.coord(! IS_X_AXIS), limits.min.coord(! IS_X_AXIS));
      clamp_max(limits.max.coord(IS_X_AXIS), full_extent);
      return limits;
     }

     template<bool IS_X_AXIS, class TILE_ELEMENT>
     [[gnu::always_inline]] inline auto compute_layout(context const& ctx, TILE_ELEMENT &tile, std::vector<float> &tilePos) -> void
     {
       auto const sz = tile.size();

       // Collect min, max, and stretch information from each element.
       // Initially set the allocation sizes of each element to its minimum.
       std::vector<layout_info> info(sz);
       for (std::size_t i = 0; i != sz; ++i)
       {
         auto& elem = tile.at(i);
         auto limits = elem.limits(ctx);
         info[i].stretch = elem.stretch().coord(IS_X_AXIS);
         info[i].min = limits.min.coord(IS_X_AXIS);
         info[i].max = limits.max.coord(IS_X_AXIS);
         info[i].alloc = limits.min.coord(IS_X_AXIS);
         info[i].index = i;
       }

       auto const otherMin = ctx.bounds.axisMin(not IS_X_AXIS);
       auto const otherMax = ctx.bounds.axisMax(not IS_X_AXIS);
       auto const myMin = ctx.bounds.axisMin(IS_X_AXIS);
       auto const myDelta = ctx.bounds.axisDelta(IS_X_AXIS);
       // Compute the best fit for all elements
       allocate(myDelta, info);

       // Now we have the final layout. We can now layout the individual
       // elements.
       tilePos.resize(sz);
       auto curr = 0.0f;
       for (std::size_t i = 0; i != sz; ++i)
       {
         tilePos[i] = curr + info[i].alloc;
         auto const prev = curr;
         curr += info[i].alloc;

         auto& elem = tile.at(i);
         auto ebounds = IS_X_AXIS
           ? rect{prev+myMin, otherMin, curr+myMin, otherMax}
           : rect{otherMin, prev+myMin, otherMax, curr+myMin};
         elem.layout(context{ctx, &elem, ebounds});
       }
     }
   }

   ////////////////////////////////////////////////////////////////////////////
   // Vertical Tiles
   ////////////////////////////////////////////////////////////////////////////
   view_limits vtile_element::limits(basic_context const& ctx) const
   {
     return compute_limits<false>(ctx, *this);
   }

   void vtile_element::layout(context const& ctx)
   {
     compute_layout<false>(ctx, *this, _tiles);
   }

   void vtile_element::draw(context const& ctx)
   {
      if (_tiles.empty())
         layout(ctx);
      composite_base::draw(ctx);
   }

   rect vtile_element::bounds_of(context const& ctx, std::size_t index) const
   {
      if (index >= _tiles.size())
         return {};
      auto const left = ctx.bounds.left;
      auto const right = ctx.bounds.right;
      auto const top = ctx.bounds.top;
      return rect{left, (index? _tiles[index-1] : 0)+top, right, _tiles[index]+top};
   }

   ////////////////////////////////////////////////////////////////////////////
   // Horizontal Tiles
   ////////////////////////////////////////////////////////////////////////////
   view_limits htile_element::limits(basic_context const& ctx) const
   {
     return compute_limits<true>(ctx, *this);
   }

   void htile_element::layout(context const& ctx)
   {
     compute_layout<true>(ctx, *this, _tiles);
   }

   void htile_element::draw(context const& ctx)
   {
      if (_tiles.empty())
         layout(ctx);
      composite_base::draw(ctx);
   }

   rect htile_element::bounds_of(context const& ctx, std::size_t index) const
   {
      if (index >= _tiles.size())
         return {};
      auto const top = ctx.bounds.top;
      auto const bottom = ctx.bounds.bottom;
      auto const left = ctx.bounds.left;
      return rect{(index? _tiles[index-1] : 0)+left, top, _tiles[index]+left, bottom};
   }
}
