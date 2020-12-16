/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/layer.hpp>
#include <elements/view.hpp>
#include <elements/support/context.hpp>

namespace cycfi { namespace elements
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
      for (std::size_t ix = 0; ix != size(); ++ix)
      {
         auto& e = at(ix);
         e.layout(context{ ctx, &e, bounds_of(ctx, ix) });
      }
   }

   void layer_element::draw(context const& ctx)
   {
      auto width = ctx.bounds.width();
      auto height = ctx.bounds.height();
      if (_previous_size.x != width || _previous_size.y != height)
      {
         _previous_size.x = width;
         _previous_size.y = height;
         layout(ctx);
      }
      composite_base::draw(ctx);
   }

   layer_element::hit_info layer_element::hit_element(context const& ctx, point p, bool control) const
   {
      // we test from the highest index (topmost element)
      for (int ix = int(size())-1; ix >= 0; --ix)
      {
         auto& e = at(ix);
         if (!control || e.wants_control())
         {
            rect bounds = bounds_of(ctx, ix);
            if (bounds.includes(p))
            {
               context ectx{ ctx, &e, bounds };
               if (e.hit_test(ectx, p))
                  return hit_info{ e.shared_from_this(), bounds, int(ix) };
            }
         }
      }
      return hit_info{ {}, rect{}, -1 };
   }

   rect layer_element::bounds_of(context const& ctx, std::size_t index) const
   {
      auto left = ctx.bounds.left;
      auto top = ctx.bounds.top;
      auto width = ctx.bounds.width();
      auto height = ctx.bounds.height();
      auto  limits = at(index).limits(ctx);

      clamp_min(width, limits.min.x);
      clamp_max(width, limits.max.x);
      clamp_min(height, limits.min.y);
      clamp_max(height, limits.max.y);

      return { left, top, left+width, top+height };
   }

   void layer_element::begin_focus()
   {
      focus_top();
      return composite_base::begin_focus();
   }

   void layer_element::focus_top()
   {
      for (int ix = int(size())-1; ix >= 0; --ix)
      {
         auto& e = at(ix);
         if (composite_base::focus() == &e)
            break; // element at at(ix) is already the focus

         if (e.wants_focus())
         {
            e.begin_focus();
            composite_base::focus(ix);
            break;
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   // Deck
   ////////////////////////////////////////////////////////////////////////////
   void deck_element::draw(context const& ctx)
   {
      rect bounds = bounds_of(ctx, _selected_index);
      if (intersects(bounds, ctx.view_bounds()))
      {
         auto& elem = at(_selected_index);
         context ectx{ ctx, &elem, bounds };
         elem.draw(ectx);
      }
   }

   void deck_element::refresh(context const& ctx, element& element, int outward)
   {
      if (&element == this)
      {
         ctx.view.refresh(ctx, outward);
      }
      else
      {
         rect bounds = bounds_of(ctx, _selected_index);
         auto& elem = at(_selected_index);
         context ectx{ ctx, &elem, bounds };
         elem.refresh(ectx, element, outward);
      }
   }

   layer_element::hit_info deck_element::hit_element(context const& ctx, point p, bool control) const
   {
      auto& e = at(_selected_index);
      if (!control || e.wants_control())
      {
         rect bounds = bounds_of(ctx, _selected_index);
         if (bounds.includes(p))
         {
            context ectx{ ctx, &e, bounds };
            if (e.hit_test(ectx, p))
               return hit_info{ e.shared_from_this(), bounds, int(_selected_index) };
         }
      }
      return hit_info{ {}, rect{}, -1 };
   }

   void deck_element::begin_focus()
   {
      if (!composite_base::focus())
      {
         auto& e = at(_selected_index);
         if (e.wants_control() && e.wants_focus())
            composite_base::focus(_selected_index);
      }
      composite_base::begin_focus();
   }

   void deck_element::select(std::size_t index)
   {
      if (index < size())
         _selected_index = index;
   }
}}
