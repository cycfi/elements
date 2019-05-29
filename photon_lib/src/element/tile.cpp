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

   namespace
   {
      // Compute the best fit for all elements
      void allocate(
         double size, double max_span, double total
       , std::vector<layout_info>& info)
      {
         double extra = size - total;
         while (extra > 0.5) // loop while there's room to grow
         {
            double remove_span = 0.0;
            for (auto& info : info)
            {
               if (info.alloc < info.max)       // This element can grow
               {
                  auto add = extra * info.span / max_span;
                  info.alloc += add;
                  if (info.alloc >= info.max)   // We exceeded its max
                  {
                     info.alloc = info.max;
                     remove_span -= info.span;
                  }
                  total += add;
               }
            }
            extra = size - total;
            max_span -= remove_span;
         }
      }
   }

   void vtile_element::layout(context const& ctx)
   {
      _left = ctx.bounds.left;
      _right = ctx.bounds.right;
      _tiles.resize(size()+1);

      double const height = ctx.bounds.height();

      // Collect min, max, and span information from each element. Also,
      // accumulate the maximum span (max_span) for later. Initially set the
      // allocation sizes of each element to its minimum.
      double max_span = 0.0;
      float total = 0.0;
      std::vector<layout_info> info(size());
      for (std::size_t i = 0; i != size(); ++i)
      {
         auto& elem = at(i);
         auto limits = elem.limits(ctx);
         max_span += (info[i].span = elem.span().y);
         total += (info[i].alloc = info[i].min = limits.min.y);
         info[i].max = limits.max.y;
      }

      // Compute the best fit for all elements
      allocate(height, max_span, total, info);

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

      double const width = ctx.bounds.width();

      // Collect min, max, and span information from each element. Also,
      // accumulate the maximum span (max_span) for later. Initially set the
      // allocation sizes of each element to its minimum.
      double max_span = 0.0;
      double total = 0.0;
      std::vector<layout_info> info(size());
      for (std::size_t i = 0; i != size(); ++i)
      {
         auto& elem = at(i);
         auto limits = elem.limits(ctx);
         max_span += (info[i].span = elem.span().x);
         total += (info[i].alloc = info[i].min = limits.min.x);
         info[i].max = limits.max.x;
      }

      // Compute the best fit for all elements
      allocate(width, max_span, total, info);

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
