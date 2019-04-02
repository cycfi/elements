/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <photon/element/tile.hpp>
#include <photon/support/context.hpp>

namespace cycfi { namespace photon
{
   ////////////////////////////////////////////////////////////////////////////
   // Vertical Tiles
   ////////////////////////////////////////////////////////////////////////////
   view_limits vtile_element::limits(basic_context const& ctx) const
   {
      view_limits limits{ { 0.0, 0.0 }, { full_extent, 0.0 } };
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

   void vtile_element::layout(context const& ctx)
   {
      _left = ctx.bounds.left;
      _right = ctx.bounds.right;

      _tiles.resize(size()+1);

      double max_span_y = 0.0;
      for (std::size_t ix = 0; ix != size();  ++ix)
         max_span_y += at(ix).span().y;

      double   height   = ctx.bounds.height();
      double   curr     = ctx.bounds.top;
      auto     i        = _tiles.begin();

      for (std::size_t ix = 0; ix != size(); ++ix)
      {
         auto& elem = at(ix);
         auto  limits = elem.limits(ctx);
         auto  elem_height = std::max<float>(height * at(ix).span().y / max_span_y, limits.min.y);

         *i++ = curr;
         auto prev = curr;
         curr += elem_height;

         if (curr > ctx.bounds.bottom)
            curr = ctx.bounds.bottom;

         rect ebounds = { _left, float(prev), _right, float(curr) };
         elem.layout(context{ ctx, &elem, ebounds });
      }
      *i = curr;
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

   void htile_element::layout(context const& ctx)
   {
      _top = ctx.bounds.top;
      _bottom = ctx.bounds.bottom;

      _tiles.resize(size()+1);

      double max_span_x = 0.0;
      for (std::size_t ix = 0; ix != size();  ++ix)
         max_span_x += at(ix).span().x;

      double   width    = ctx.bounds.width();
      double   curr     = ctx.bounds.left;
      auto     i        = _tiles.begin();

      for (std::size_t ix = 0; ix != size(); ++ix)
      {
         auto& elem = at(ix);
         auto  limits = elem.limits(ctx);
         auto  elem_width = std::max<float>(width * at(ix).span().x / max_span_x, limits.min.x);

         *i++ = curr;
         auto prev = curr;
         curr += elem_width;

         if (curr > ctx.bounds.right)
            curr = ctx.bounds.right;

         rect ebounds = { float(prev), _top, float(curr), _bottom };
         elem.layout(context{ ctx, &elem, ebounds });
      }
      *i = curr;
   }

   rect htile_element::bounds_of(context const& ctx, std::size_t index) const
   {
      return rect{ _tiles[index], _top, _tiles[index + 1], _bottom };
   }
}}
