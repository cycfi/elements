/*=============================================================================
   Copyright (c) 2016-2017 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <photon/element/layer.hpp>
#include <photon/view.hpp>
#include <photon/support/context.hpp>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////
   // Layer
   ////////////////////////////////////////////////////////////////////////////
   view_limits layer_element::limits(basic_context const& ctx) const
   {
      view_limits limits{ { 0.0, 0.0 }, { full_extent, full_extent } };
      for (std::size_t ix = 0; ix != size();  ++ix)
      {
         auto el = at(ix).limits(ctx);

         clamp_min(limits.min.x, el.min.x);
         clamp_min(limits.min.y, el.min.y);
         clamp_max(limits.max.x, el.max.x);
         clamp_max(limits.max.y, el.max.y);

         limits.max.x = std::max(limits.max.x, limits.min.x);
         limits.max.y = std::max(limits.max.y, limits.min.y);
      }

      return limits;
   }

   void layer_element::layout(context const& ctx)
   {
      bounds = ctx.bounds;
      for (std::size_t ix = 0; ix != size(); ++ix)
      {
         auto& e = at(ix);
         e.layout(context{ ctx, &e, bounds_of(ctx, ix) });
      }
   }

   layer_element::hit_info layer_element::hit_element(context const& ctx, point p) const
   {
      // we test from the highest index (topmost element)
      for (int ix = int(size())-1; ix >= 0; --ix)
      {
         auto& e = at(ix);
         if (e.is_control())
         {
            rect bounds = bounds_of(ctx, ix);
            if (bounds.includes(p))
            {
               context ectx{ ctx, &e, bounds };
               if (e.hit_test(ectx, p))
                  return hit_info{ &e, bounds, int(ix) };
            }
         }
      }
      return hit_info{ 0, rect{}, -1 };
   }

   rect layer_element::bounds_of(context const& ctx, std::size_t index) const
   {
      float width = ctx.bounds.width();
      float height = ctx.bounds.height();
      auto  limits = at(index).limits(ctx);

      clamp_min(width, limits.min.x);
      clamp_max(width, limits.max.x);
      clamp_min(height, limits.min.y);
      clamp_max(height, limits.max.y);

      return { bounds.left, bounds.top, bounds.left + width, bounds.top + height };
   }

   bool layer_element::focus(focus_request r)
   {
      focus_top();
      return composite_base::focus(r);
   }

   void layer_element::focus_top()
   {
      if (!composite_base::focus())
      {
         for (int ix = int(size())-1; ix >= 0; --ix)
         {
            auto& e = at(ix);
            if (e.is_control() && e.focus(focus_request::wants_focus))
            {
               e.focus(focus_request::begin_focus);
               composite_base::focus(ix);
               break;
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   // Deck
   ////////////////////////////////////////////////////////////////////////////
   void deck_element::draw(context const& ctx)
   {
      rect bounds = bounds_of(ctx, _selected_index);
      if (intersects(bounds, ctx.view.dirty()))
      {
         auto& elem = at(_selected_index);
         context ectx{ ctx, &elem, bounds };
         elem.draw(ectx);
      }
   }

   void deck_element::refresh(context const& ctx, element& element)
   {
      if (&element == this)
      {
         ctx.view.refresh(ctx);
      }
      else
      {
         rect bounds = bounds_of(ctx, _selected_index);
         auto& elem = at(_selected_index);
         context ectx{ ctx, &elem, bounds };
         elem.refresh(ectx, element);
      }
   }

   layer_element::hit_info deck_element::hit_element(context const& ctx, point p) const
   {
      auto& e = at(_selected_index);
      if (e.is_control())
      {
         rect bounds = bounds_of(ctx, _selected_index);
         if (bounds.includes(p))
         {
            context ectx{ ctx, &e, bounds };
            if (e.hit_test(ectx, p))
               return hit_info{ &e, bounds, int(_selected_index) };
         }
      }
      return hit_info{ 0, rect{}, -1 };
   }

   bool deck_element::focus(focus_request r)
   {
      if (!composite_base::focus())
      {
         auto& e = at(_selected_index);
         if (e.is_control() && e.focus(focus_request::wants_focus))
            composite_base::focus(_selected_index);
      }
      return composite_base::focus(r);
   }

   void deck_element::select(std::size_t index)
   {
      if (index < size())
         _selected_index = index;
   }
}
