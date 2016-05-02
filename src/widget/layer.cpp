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
      for (auto const& elem : elements())
      {
         rect  el = elem->limits(ctx);

         min_limit(limits.left, el.left);
         min_limit(limits.top, el.top);
         max_limit(limits.right, el.right);
         max_limit(limits.bottom, el.bottom);
      }

      return limits;
   }

   void layer_widget::layout(context const& ctx)
   {
      bounds = ctx.bounds;
      for (std::size_t i = 0; i != elements().size(); ++i)
      {
         elements()[i]->layout(context{ ctx, elements()[i].get(), bounds_of(ctx, i) });
      }
   }

   layer_widget::hit_info layer_widget::hit_element(context const& ctx, point const& p) const
   {
      // we test from the highest index (topmost element)
      for (int i = int(elements().size())-1; i >= 0; --i)
      {
         widget_ptr e = elements()[i];
         if (e->is_control())
         {
            rect bounds = bounds_of(ctx, i);
            if (bounds.includes(p))
               return hit_info{ e.get(), bounds, int(i) };
         }
      }
      return hit_info{ 0, rect{}, -1 };
   }


   rect layer_widget::bounds_of(context const& ctx, std::size_t index) const
   {
      double   width = ctx.bounds.width();
      double   height = ctx.bounds.height();
      rect     limits = elements()[index]->limits(ctx);

      min_limit(width, limits.left);
      max_limit(width, limits.right);
      min_limit(height, limits.top);
      max_limit(height, limits.bottom);

      return { bounds.left, bounds.top, bounds.left + width, bounds.top + height };
   }

   bool layer_widget::focus(focus_request r)
   {
      focus_top();
      return composite::focus(r);
   }

   bool layer_widget::scroll(context const& ctx, point const& p)
   {
      focus_top();
      return composite::scroll(ctx, p);
   }

   void layer_widget::focus_top()
   {
      if (!composite::focus())
      {
         for (int i = int(elements().size())-1; i >= 0; --i)
         {
            widget_ptr e = elements()[i];
            if (e->is_control() && e->focus(focus_request::wants_focus))
               composite::focus(i);
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
      rect bounds = bounds_of(ctx, 0);
      if (intersects(bounds, w_bounds))
      {
         auto elem = elements()[0];
         context ectx{ ctx, elem.get(), bounds };
         elem->draw(ectx);
      }
   }
}
