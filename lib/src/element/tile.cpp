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
         float min, max, stretch, alloc, density;
         std::size_t index;
      };

      auto range(layout_info info)
      {
         return info.max - info.min;
      }

      // Stretch per unit of growable range. A fixed element (no range) has
      // infinite density, so it sorts first and is settled before any space is
      // handed out.
      float compute_density(layout_info const& info)
      {
         auto const r = info.max - info.min;
         if (r <= 0.0f)
            return std::numeric_limits<float>::max();
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

         // Sort elements by precomputed density, densest first. Any element that
         // would reach its max size sorts ahead of every one that would not, so a
         // single pass settles the layout: once an element saturates we drop its
         // stretch and consumed range from the pools and move on, never having to
         // revisit an earlier element. That makes this O(n log n), not O(n * n).
         // The caller scatters each alloc back to its index, so no re-sort to the
         // original order is needed.
         std::sort(elements.begin(), elements.end(),
            [](layout_info const& lhs, layout_info const& rhs){ return lhs.density > rhs.density; });
         auto sum_stretch = std::accumulate(elements.begin(), elements.end(), 0.0f,
            [](float sum, layout_info const& elem){ return sum + elem.stretch; });
         auto free_space = space - sum_min;

         for (auto& e : elements)
         {
            if (sum_stretch <= 0.0f)
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
      }

      template <axis Axis, class TileElement, axis OtherAxis = other(Axis)>
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

      template <axis Axis, class TileElement, axis OtherAxis = other(Axis)>
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
            info[i].density = compute_density(info[i]);
         }

         auto const other_axis_min = axis_min(ctx.bounds, OtherAxis);
         auto const other_axis_max = axis_max(ctx.bounds, OtherAxis);
         auto const my_axis_min = axis_min(ctx.bounds, Axis);
         auto const my_axis_extent = axis_extent(ctx.bounds, Axis);
         // Compute the best fit for all elements
         allocate(my_axis_extent, info);

         // allocate left info sorted by density. Scatter each element's size
         // back to its own slot (O(n)), reusing tile_offsets as the buffer.
         tile_offsets.resize(sz);
         for (auto const& e : info)
            tile_offsets[e.index] = e.alloc;

         // Now we have the final layout. We can now layout the individual
         // elements, turning the per-tile sizes into cumulative end offsets.
         auto curr = 0.0f;
         for (std::size_t i = 0; i != sz; ++i)
         {
            auto const prev = curr;
            curr += tile_offsets[i];
            tile_offsets[i] = curr;

            auto& elem = tile.at(i);
            auto ebounds = make_rect(Axis, prev+my_axis_min, other_axis_min, curr+my_axis_min, other_axis_max);

            elem.layout(context{ctx, &elem, ebounds});
          }
      }

      template <axis Axis, axis OtherAxis = other(Axis)>
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

   std::string vtile_element::class_name() const
   {
      return "vtile";
   }

   std::string htile_element::class_name() const
   {
      return "htile";
   }
}
