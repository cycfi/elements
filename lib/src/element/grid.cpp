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

   void vgrid_element::layout(context const& ctx)
   {
      auto left = ctx.bounds.left;
      auto right = ctx.bounds.right;
      auto top = ctx.bounds.top;
      auto total_height = ctx.bounds.height();

      auto prev = 0;
      for (std::size_t i = 0; i != size(); ++i)
      {
         auto y = grid()[i] * total_height;
         auto height = y - prev;
         auto& elem = at(i);
         rect ebounds = { left, prev+top, right, prev+top+height };
         elem.layout(context{ ctx, &elem, ebounds });
         prev = y;
      }
   }

   rect vgrid_element::bounds_of(context const& ctx, std::size_t index) const
   {
      if (index >= size())
         return {};
      auto left = ctx.bounds.left;
      auto right = ctx.bounds.right;
      auto top = ctx.bounds.top;
      auto total_height = ctx.bounds.height();
      return rect{
         left
       , (index ? (grid()[index-1] * total_height) : 0) + top
       , right
       , (grid()[index] * total_height) + top
      };
   }

   ////////////////////////////////////////////////////////////////////////////
   // Horizontal Grids
   ////////////////////////////////////////////////////////////////////////////
   view_limits hgrid_element::limits(basic_context const& ctx) const
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

   void hgrid_element::layout(context const& ctx)
   {
      auto top = ctx.bounds.top;
      auto bottom = ctx.bounds.bottom;
      auto left = ctx.bounds.left;
      auto total_width = ctx.bounds.width();

      auto prev = 0;
      for (std::size_t i = 0; i != size(); ++i)
      {
         auto x = grid()[i] * total_width;
         auto width = x - prev;
         auto& elem = at(i);
         rect ebounds = { prev+left, top, prev+left+width, bottom };
         elem.layout(context{ ctx, &elem, ebounds });
         prev = x;
      }
   }

   rect hgrid_element::bounds_of(context const& ctx, std::size_t index) const
   {
      if (index >= size())
         return {};
      auto top = ctx.bounds.top;
      auto bottom = ctx.bounds.bottom;
      auto left = ctx.bounds.left;
      auto total_width = ctx.bounds.width();
      return rect{
         (index? (grid()[index-1] * total_width) : 0) + left
       , top
       , (grid()[index] * total_width) + left
       , bottom
      };
   }
}}
