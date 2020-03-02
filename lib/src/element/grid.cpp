/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/grid.hpp>
#include <elements/support/context.hpp>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Vertical Grids
   ////////////////////////////////////////////////////////////////////////////
   view_limits vgrid_element::limits(basic_context const& ctx) const
   {
      view_limits limits{ { 0.0, 0.0 }, { full_extent, 0.0 } };
      auto prev = 0;
      for (std::size_t i = 0; i != size();  ++i)
      {
         auto el = at(i).limits(ctx);
         auto height = _grid[i] - prev;
         prev = _grid[i];

         limits.min.y += height;
         limits.max.y += height;
         clamp_min(limits.min.x, el.min.x);
         clamp_max(limits.max.x, el.max.x);
      }

      clamp_min(limits.max.x, limits.min.x);
      clamp_max(limits.max.y, full_extent);
      return limits;
   }

   void vgrid_element::layout(context const& ctx)
   {
      auto left = ctx.bounds.left;
      auto right = ctx.bounds.right;
      auto top = ctx.bounds.top;

      auto prev = 0;
      for (std::size_t i = 0; i != size(); ++i)
      {
         auto height = _grid[i] - prev;
         auto& elem = at(i);
         rect ebounds = { left, prev+top, right, prev+top+height };
         elem.layout(context{ ctx, &elem, ebounds });
         prev = _grid[i];
      }
   }

   rect vgrid_element::bounds_of(context const& ctx, std::size_t index) const
   {
      if (index >= size())
         return {};
      auto left = ctx.bounds.left;
      auto right = ctx.bounds.right;
      auto top = ctx.bounds.top;
      return rect{ left, (index ? _grid[index-1] : 0)+top, right, _grid[index]+top };
   }

   ////////////////////////////////////////////////////////////////////////////
   // Horizontal Grids
   ////////////////////////////////////////////////////////////////////////////
   view_limits hgrid_element::limits(basic_context const& ctx) const
   {
      view_limits limits{ { 0.0, 0.0 }, { 0.0, full_extent } };
      auto prev = 0;
      for (std::size_t i = 0; i != size();  ++i)
      {
         auto el = at(i).limits(ctx);
         auto width = _grid[i] - prev;
         prev = _grid[i];

         limits.min.x += width;
         limits.max.x += width;
         clamp_min(limits.min.y, el.min.y);
         clamp_max(limits.max.y, el.max.y);
      }

      clamp_min(limits.max.y, limits.min.y);
      clamp_max(limits.max.x, full_extent);
      return limits;
   }

   void hgrid_element::layout(context const& ctx)
   {
      auto top = ctx.bounds.top;
      auto bottom = ctx.bounds.bottom;
      auto left = ctx.bounds.left;

      auto prev = 0;
      for (std::size_t i = 0; i != size(); ++i)
      {
         auto width = _grid[i] - prev;
         auto& elem = at(i++);
         rect ebounds = { prev+left, top, prev+left+width, bottom };
         elem.layout(context{ ctx, &elem, ebounds });
         prev = _grid[i];
      }
   }

   rect hgrid_element::bounds_of(context const& ctx, std::size_t index) const
   {
      if (index >= size())
         return {};
      auto top = ctx.bounds.top;
      auto bottom = ctx.bounds.bottom;
      auto left = ctx.bounds.left;
      return rect{ (index? _grid[index-1] : 0)+left, top, _grid[index]+left, bottom };
   }
}}
