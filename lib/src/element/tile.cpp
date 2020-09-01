/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman
   Copyright (c) 2020 Michał Urbański

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/tile.hpp>
#include <elements/support/context.hpp>

#include <algorithm>
#include <numeric>
#include <limits>

namespace cycfi { namespace elements
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
   }

   ////////////////////////////////////////////////////////////////////////////
   // Vertical Tiles
   ////////////////////////////////////////////////////////////////////////////
   view_limits vtile_element::limits(basic_context const& ctx) const
   {
      view_limits limits{ { 0.0, 0.0 }, { full_extent, 0.0 } };
      for (std::size_t i = 0; i != size();  ++i)
      {
         auto el = at(i).limits(ctx);

         limits.min.y += el.min.y;
         limits.max.y += el.max.y;
         clamp_min(limits.min.x, el.min.x);
         clamp_max(limits.max.x, el.max.x);
      }

      clamp_min(limits.max.x, limits.min.x);
      clamp_max(limits.max.y, full_extent);
      return limits;
   }

   void vtile_element::layout(context const& ctx)
   {
      auto const sz = size();

      // Collect min, max, and stretch information from each element.
      // Initially set the allocation sizes of each element to its minimum.
      std::vector<layout_info> info(sz);
      for (std::size_t i = 0; i != sz; ++i)
      {
         auto& elem = at(i);
         auto limits = elem.limits(ctx);
         info[i].stretch = elem.stretch().y;
         info[i].min = limits.min.y;
         info[i].max = limits.max.y;
         info[i].alloc = limits.min.y;
         info[i].index = i;
      }

      auto const left = ctx.bounds.left;
      auto const right = ctx.bounds.right;
      auto const top = ctx.bounds.top;
      auto const height = ctx.bounds.height();
      // Compute the best fit for all elements
      allocate(height, info);

      // Now we have the final layout. We can now layout the individual
      // elements.
      _tiles.resize(sz);
      auto curr = 0.0f;
      for (std::size_t i = 0; i != sz; ++i)
      {
         _tiles[i] = curr + info[i].alloc;
         auto const prev = curr;
         curr += info[i].alloc;

         auto& elem = at(i);
         rect ebounds = { left, prev+top, right, curr+top };
         elem.layout(context{ ctx, &elem, ebounds });
      }
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
      return rect{ left, (index? _tiles[index-1] : 0)+top, right, _tiles[index]+top };
   }

   ////////////////////////////////////////////////////////////////////////////
   // Horizontal Tiles
   ////////////////////////////////////////////////////////////////////////////
   view_limits htile_element::limits(basic_context const& ctx) const
   {
      view_limits limits{ { 0.0, 0.0 }, { 0.0, full_extent } };
      for (std::size_t i = 0; i != size();  ++i)
      {
         auto el = at(i).limits(ctx);

         limits.min.x += el.min.x;
         limits.max.x += el.max.x;
         clamp_min(limits.min.y, el.min.y);
         clamp_max(limits.max.y, el.max.y);
      }

      clamp_min(limits.max.y, limits.min.y);
      clamp_max(limits.max.x, full_extent);
      return limits;
   }

   void htile_element::layout(context const& ctx)
   {
      auto const sz = size();

      // Collect min, max, and stretch information from each element.
      // Initially set the allocation sizes of each element to its minimum.
      std::vector<layout_info> info(sz);
      for (std::size_t i = 0; i != sz; ++i)
      {
         auto& elem = at(i);
         auto limits = elem.limits(ctx);
         info[i].stretch = elem.stretch().x;
         info[i].min = limits.min.x;
         info[i].max = limits.max.x;
         info[i].alloc = limits.min.x;
         info[i].index = i;
      }

      auto const top = ctx.bounds.top;
      auto const bottom = ctx.bounds.bottom;
      auto const left = ctx.bounds.left;
      auto const width = ctx.bounds.width();
      // Compute the best fit for all elements
      allocate(width, info);

      // Now we have the final layout. We can now layout the individual
      // elements.
      _tiles.resize(sz);
      auto curr = 0.0f;
      for (std::size_t i = 0; i != sz; ++i)
      {
         _tiles[i] = curr + info[i].alloc;
         auto const prev = curr;
         curr += info[i].alloc;

         auto& elem = at(i);
         rect ebounds = { prev+left, top, curr+left, bottom };
         elem.layout(context{ ctx, &elem, ebounds });
      }
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
      return rect{ (index? _tiles[index-1] : 0)+left, top, _tiles[index]+left, bottom };
   }
}}
