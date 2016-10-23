/*=======================================================================================
   Copyright (c) 2016 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=======================================================================================*/
#include <photon/widget/tile.hpp>
#include <photon/support/context.hpp>

namespace photon
{
   //////////////////////////////////////////////////////////////////////////////////////
   // Vertical Tiles
   //////////////////////////////////////////////////////////////////////////////////////
   widget_limits vtile_widget::limits(basic_context const& ctx) const
   {
      widget_limits limits{ { 0.0, 0.0 }, { full_extent, 0.0 } };
      for (std::size_t ix = 0; ix != size();  ++ix)
      {
         auto el = at(ix).limits(ctx);

         limits.min.y += el.min.y;
         limits.max.y += el.max.y;
         clamp_min(limits.min.x, el.min.x);
         clamp_max(limits.max.x, el.max.x);
      }

      clamp_min(limits.max.x, limits.min.x);
      clamp_max(limits.max.y, full_extent);
      return limits;
   }

   void vtile_widget::layout(context const& ctx)
   {
      _left = ctx.bounds.left;
      _right = ctx.bounds.right;

      _tiles.resize(size()+1);

      double   min_y = 0.0;
      double   max_y = 0.0;
      for (std::size_t ix = 0; ix != size();  ++ix)
      {
         auto el = at(ix).limits(ctx);
         min_y += el.min.y;
         max_y += el.max.y;
      }

      double   height   = ctx.bounds.height();
      double   extra    = max_y - height;
      double   m_size   = max_y - min_y;
      double   curr     = ctx.bounds.top;
      auto     i        = _tiles.begin();

      for (std::size_t ix = 0; ix != size();  ++ix)
      {
         auto& elem = at(ix);
         auto  limits = elem.limits(ctx);

         *i++ = curr;
         auto prev = curr;
         curr += limits.max.y;

         if ((extra != 0) && (m_size != 0))
            curr -= extra * (limits.max.y - limits.min.y) / m_size;

         rect ebounds = { _left, float(prev), _right, float(curr) };
         elem.layout(context{ ctx, &elem, ebounds });
      }
      *i = curr;
   }

   rect vtile_widget::bounds_of(context const& ctx, std::size_t index) const
   {
      return rect{ _left, _tiles[index], _right, _tiles[index+1] };
   }

   //////////////////////////////////////////////////////////////////////////////////////
   // Horizontal Tiles
   //////////////////////////////////////////////////////////////////////////////////////
   widget_limits htile_widget::limits(basic_context const& ctx) const
   {
      widget_limits limits{ { 0.0, 0.0 }, { 0.0, full_extent } };
      for (std::size_t ix = 0; ix != size();  ++ix)
      {
         auto el = at(ix).limits(ctx);

         limits.min.x += el.min.x;
         limits.max.x += el.max.x;
         clamp_min(limits.min.y, el.min.y);
         clamp_max(limits.max.y, el.max.y);
      }

      clamp_min(limits.max.y, limits.min.y);
      clamp_max(limits.max.x, full_extent);
      return limits;
   }

   void htile_widget::layout(context const& ctx)
   {
      _top = ctx.bounds.top;
      _bottom = ctx.bounds.bottom;

      _tiles.resize(size()+1);

      double   min_x = 0.0;
      double   max_x = 0.0;
      for (std::size_t ix = 0; ix != size();  ++ix)
      {
         auto el = at(ix).limits(ctx);
         min_x += el.min.x;
         max_x += el.max.x;
      }

      double   width    = ctx.bounds.width();
      double   extra    = max_x - width;
      double   m_size   = max_x - min_x;
      double   curr     = ctx.bounds.left;
      auto     i        = _tiles.begin();

      for (std::size_t ix = 0; ix != size();  ++ix)
      {
         auto& elem = at(ix);
         auto  limits = elem.limits(ctx);

         *i++ = curr;
         auto prev = curr;
         curr += limits.max.x;

         if ((extra != 0) && (m_size != 0))
            curr -= extra * (limits.max.x - limits.min.x) / m_size;

         rect ebounds = { float(prev), _top, float(curr), _bottom };
         elem.layout(context{ ctx, &elem, ebounds });
      }
      *i = curr;
   }

   rect htile_widget::bounds_of(context const& ctx, std::size_t index) const
   {
      return rect{ _tiles[index], _top, _tiles[index + 1], _bottom };
   }
}
