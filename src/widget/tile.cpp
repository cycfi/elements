/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/tile.hpp>
#include <photon/support/context.hpp>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Vertical Tiles
   ////////////////////////////////////////////////////////////////////////////////////////////////
   widget_limits vtile_widget::limits(basic_context const& ctx) const
   {
      widget_limits limits{ { 0.0, 0.0 }, { full_extent, 0.0 } };
      for (std::size_t ix = 0; ix != size();  ++ix)
      {
         auto el = get(ix)->limits(ctx);

         limits.min.y += el.min.y;
         limits.max.y += el.max.y;
         clamp_min(limits.min.x, el.min.x);
         clamp_max(limits.max.x, el.max.x);
      }

      clamp_min(limits.max.x, limits.min.x);
      return limits;
   }

   void vtile_widget::layout(context const& ctx)
   {
      _left = ctx.bounds.left;
      _right = ctx.bounds.right;

      _tiles.resize(size()+1);

      auto     limits   = this->limits(ctx);
      double   height   = ctx.bounds.height();
      double   extra    = limits.max.y - height;
      double   m_size   = limits.max.y - limits.min.y;
      double   curr     = ctx.bounds.top;
      auto     i        = _tiles.begin();

      for (std::size_t ix = 0; ix != size();  ++ix)
      {
         auto  elem = get(ix);
         auto  limits = elem->limits(ctx);

         *i++ = curr;
         auto prev = curr;
         curr += limits.max.y;

         if ((extra != 0) && (m_size != 0))
            curr -= extra * (limits.max.y - limits.min.y) / m_size;

         rect ebounds = { _left, float(prev), _right, float(curr) };
         elem->layout(context{ ctx, elem, ebounds });
      }
      *i = curr;
   }

   rect vtile_widget::bounds_of(context const& ctx, std::size_t index) const
   {
      return rect{ _left, _tiles[index], _right, _tiles[index+1] };
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Horizontal Tiles
   ////////////////////////////////////////////////////////////////////////////////////////////////
   widget_limits htile_widget::limits(basic_context const& ctx) const
   {
      widget_limits limits{ { 0.0, 0.0 }, { 0.0, full_extent } };
      for (std::size_t ix = 0; ix != size();  ++ix)
      {
         auto el = get(ix)->limits(ctx);

         limits.min.x += el.min.x;
         limits.max.x += el.max.x;
         clamp_min(limits.min.y, el.min.y);
         clamp_max(limits.max.y, el.max.y);
      }

      clamp_min(limits.max.y, limits.min.y);
      return limits;
   }

   void htile_widget::layout(context const& ctx)
   {
      _top = ctx.bounds.top;
      _bottom = ctx.bounds.bottom;

      _tiles.resize(size()+1);

      auto     limits   = this->limits(ctx);
      double   width    = ctx.bounds.width();
      double   extra    = limits.max.x - width;
      double   m_size   = limits.max.x - limits.min.x;
      double   curr     = ctx.bounds.left;
      auto     i        = _tiles.begin();

      for (std::size_t ix = 0; ix != size();  ++ix)
      {
         auto  elem = get(ix);
         auto  limits = elem->limits(ctx);

         *i++ = curr;
         auto prev = curr;
         curr += limits.max.x;

         if ((extra != 0) && (m_size != 0))
            curr -= extra * (limits.max.x - limits.min.x) / m_size;

         rect ebounds = { float(prev), _top, float(curr), _bottom };
         elem->layout(context{ ctx, elem, ebounds });
      }
      *i = curr;
   }

   rect htile_widget::bounds_of(context const& ctx, std::size_t index) const
   {
      return rect{ _tiles[index], _top, _tiles[index + 1], _bottom };
   }
}
