/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <photon/element/tile.hpp>
#include <photon/support/context.hpp>

namespace cycfi { namespace photon
{
   namespace
   {
      struct layout_info
      {
         float min, max, span, alloc;
      };
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
      _left = ctx.bounds.left;
      _right = ctx.bounds.right;
      _tiles.resize(size()+1);

      double height = ctx.bounds.height();

      // - Collect min, max, and span information from each element
      // - Also, accumulate the maximum span (max_span) for later
      double max_span = 0.0;
      std::vector<layout_info> info(size());
      for (std::size_t i = 0; i != size(); ++i)
      {
         auto& elem = at(i);
         auto limits = elem.limits(ctx);
         max_span += (info[i].span = elem.span().y);
         info[i].min = limits.min.y;
         info[i].max = limits.max.y;
      }

      // - Calculate the ideal allocation sizes for each element
      // - Get the total height computed by the ideal allocation sizes
      // - Also, get the flex_count: the number of elements that can
      //   still flex (size re-adjusted), for later.
      float total = 0.0;
      int flex_count = 0;
      for (auto& info : info)
      {
         auto elem_height = height * info.span / max_span;
         clamp(elem_height, info.min, info.max);
         if (elem_height > info.min)
            ++flex_count;
         total += (info.alloc = elem_height);
      }

      // If the total (ideal) height exceeds the supplied height,
      // Adjust the heights of elements that can flex.
      if (total > height && flex_count > 0)
      {
         float adjust = (total - height) / flex_count;
         for (auto& info : info)
         {
            if (info.alloc > info.min)
            {
               info.alloc -= adjust;
               if (info.alloc < info.min)
               {
                  auto diff = info.min - info.alloc;
                  info.alloc = info.min;
                  --flex_count;
                  total -= info.min;
                  if (total <= height)
                     break;
                  if (flex_count > 0)
                     adjust = (total - height) / flex_count;
               }
            }
         }
      }

      // Now we have the final layout. We can now layout the individual
      // elements.
      double curr = ctx.bounds.top;
      auto iter = _tiles.begin();
      std::size_t i = 0;
      for (auto const& info : info)
      {
         *iter++ = curr;
         auto prev = curr;
         curr += info.alloc;

         auto& elem = at(i++);
         rect ebounds = { _left, float(prev), _right, float(curr) };
         elem.layout(context{ ctx, &elem, ebounds });
      }
      *iter = curr;
   }

   rect vtile_element::bounds_of(context const& ctx, std::size_t index) const
   {
      return rect{ _left, _tiles[index], _right, _tiles[index+1] };
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
      _top = ctx.bounds.top;
      _bottom = ctx.bounds.bottom;
      _tiles.resize(size()+1);

      double width = ctx.bounds.width();

      // - Collect min, max, and span information from each element
      // - Also, accumulate the maximum span (max_span) for later
      double max_span = 0.0;
      std::vector<layout_info> info(size());
      for (std::size_t i = 0; i != size(); ++i)
      {
         auto& elem = at(i);
         auto limits = elem.limits(ctx);
         max_span += (info[i].span = elem.span().x);
         info[i].min = limits.min.x;
         info[i].max = limits.max.x;
      }

      // - Calculate the ideal allocation sizes for each element
      // - Get the total width computed by the ideal allocation sizes
      // - Also, get the flex_count: the number of elements that can
      //   still flex (size re-adjusted), for later.
      float total = 0.0;
      int flex_count = 0;
      for (auto& info : info)
      {
         auto elem_width = width * info.span / max_span;
         clamp(elem_width, info.min, info.max);
         if (elem_width > info.min)
            ++flex_count;
         total += (info.alloc = elem_width);
      }

      // If the total (ideal) width exceeds the supplied height,
      // Adjust the widths of elements that can flex.
      if (total > width && flex_count > 0)
      {
         float adjust = (total - width) / flex_count;
         for (auto& info : info)
         {
            if (info.alloc > info.min)
            {
               info.alloc -= adjust;
               if (info.alloc < info.min)
               {
                  auto diff = info.min - info.alloc;
                  info.alloc = info.min;
                  --flex_count;
                  total -= info.min;
                  if (total <= width)
                     break;
                  if (flex_count > 0)
                     adjust = (total - width) / flex_count;
               }
            }
         }
      }

      // Now we have the final layout. We can now layout the individual
      // elements.
      double curr = ctx.bounds.left;
      auto iter = _tiles.begin();
      std::size_t i = 0;
      for (auto const& info : info)
      {
         *iter++ = curr;
         auto prev = curr;
         curr += info.alloc;

         auto& elem = at(i++);
         rect ebounds = { float(prev), _top, float(curr), _bottom };
         elem.layout(context{ ctx, &elem, ebounds });
      }
      *iter = curr;
   }

   rect htile_element::bounds_of(context const& ctx, std::size_t index) const
   {
      return rect{ _tiles[index], _top, _tiles[index + 1], _bottom };
   }
}}
