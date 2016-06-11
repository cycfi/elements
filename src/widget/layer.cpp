/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/layer.hpp>
#include <photon/window.hpp>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Layer
   ////////////////////////////////////////////////////////////////////////////////////////////////
   rect layer_widget::limits(basic_context const& ctx) const
   {
      rect limits{ 0.0, 0.0, full_extent, full_extent };
      for (auto const& elem : *this)
      {
         rect  el = elem->limits(ctx);

         clamp_min(limits.left, el.left);
         clamp_min(limits.top, el.top);
         clamp_max(limits.right, el.right);
         clamp_max(limits.bottom, el.bottom);

         limits.right = std::max(limits.right, limits.left);
         limits.bottom = std::max(limits.bottom, limits.top);
      }

      return limits;
   }

   void layer_widget::layout(context const& ctx)
   {
      bounds = ctx.bounds;
      for (std::size_t i = 0; i != size(); ++i)
      {
         (*this)[i]->layout(context{ ctx, (*this)[i].get(), bounds_of(ctx, i) });
      }
   }

   layer_widget::hit_info layer_widget::hit_element(context const& ctx, point p) const
   {
      // we test from the highest index (topmost element)
      for (int i = int(size())-1; i >= 0; --i)
      {
         widget_ptr e = (*this)[i];
         if (e->is_control())
         {
            rect bounds = bounds_of(ctx, i);
            if (bounds.includes(p))
            {
               context ectx{ ctx, e.get(), bounds };
               if (e->hit_test(ectx, p))
                  return hit_info{ e.get(), bounds, int(i) };
            }
         }
      }
      return hit_info{ 0, rect{}, -1 };
   }

   rect layer_widget::bounds_of(context const& ctx, std::size_t index) const
   {
      float width = ctx.bounds.width();
      float height = ctx.bounds.height();
      rect  limits = (*this)[index]->limits(ctx);

      clamp_min(width, limits.left);
      clamp_max(width, limits.right);
      clamp_min(height, limits.top);
      clamp_max(height, limits.bottom);

      return { bounds.left, bounds.top, bounds.left + width, bounds.top + height };
   }

   bool layer_widget::focus(focus_request r)
   {
      focus_top();
      return composite_base::focus(r);
   }

   void layer_widget::focus_top()
   {
      if (!composite_base::focus())
      {
         for (int i = int(size())-1; i >= 0; --i)
         {
            widget_ptr e = (*this)[i];
            if (e->is_control() && e->focus(focus_request::wants_focus))
               composite_base::focus(i);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Deck
   ////////////////////////////////////////////////////////////////////////////////////////////////
   namespace
   {
      rect window_bounds(window const& w)
      {
         auto size = w.size();
         return rect{ 0, 0, size.x, size.y };
      }
   }

   void deck_widget::draw(context const& ctx)
   {
      auto w_bounds = window_bounds(ctx.window);
      rect bounds = bounds_of(ctx, _selected_index);
      if (intersects(bounds, w_bounds))
      {
         auto elem = (*this)[_selected_index];
         context ectx{ ctx, elem.get(), bounds };
         elem->draw(ectx);
      }
   }

   layer_widget::hit_info deck_widget::hit_element(context const& ctx, point p) const
   {
      widget_ptr e = (*this)[_selected_index];
      if (e->is_control())
      {
         rect bounds = bounds_of(ctx, _selected_index);
         if (bounds.includes(p))
         {
            context ectx{ ctx, e.get(), bounds };
            if (e->hit_test(ectx, p))
               return hit_info{ e.get(), bounds, int(_selected_index) };
         }
      }
      return hit_info{ 0, rect{}, -1 };
   }

   bool deck_widget::focus(focus_request r)
   {
      if (!composite_base::focus())
      {
         widget_ptr e = (*this)[_selected_index];
         if (e->is_control() && e->focus(focus_request::wants_focus))
            composite_base::focus(_selected_index);
      }
      return composite_base::focus(r);
   }

   void deck_widget::select(std::size_t index)
   {
      if (index < size())
         _selected_index = index;
   }
}
