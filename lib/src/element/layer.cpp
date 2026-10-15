/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/layer.hpp>
#include <elements/view.hpp>
#include <elements/support/context.hpp>

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Layer
   ////////////////////////////////////////////////////////////////////////////
   view_limits layer_element::limits(basic_context const& ctx) const
   {
      view_limits limits{{0.0, 0.0}, {full_extent, full_extent}};
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
         e.layout(context{ctx, &e, bounds_of(ctx, ix)});
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
               context ectx{ctx, &e, bounds};
               if (auto leaf = e.hit_test(ectx, p, true, control))
                  return hit_info{&e, leaf, bounds, int(ix)};
            }
         }
      }
      return hit_info{{}, {}, rect{}, -1};
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

      return {left, top, left+width, top+height};
   }

   ////////////////////////////////////////////////////////////////////////////
   // Deck
   ////////////////////////////////////////////////////////////////////////////
   void deck_element::draw(context const& ctx)
   {
      if (_selected_index >= size())
         return;
      rect bounds = bounds_of(ctx, _selected_index);
      if (intersects(bounds, ctx.visible_bounds()))
      {
         auto& elem = at(_selected_index);
         context ectx{ctx, &elem, bounds};
         elem.draw(ectx);
      }
   }

   void deck_element::refresh(context const& ctx, element& e, int outward)
   {
      if (&e == this)
      {
         ctx.view.refresh(ctx, outward);
      }
      else if (_selected_index < size())
      {
         rect bounds = bounds_of(ctx, _selected_index);
         auto& elem = at(_selected_index);
         context ectx{ctx, &elem, bounds};
         elem.refresh(ectx, e, outward);
      }
   }

   void deck_element::in_context_do(context const& ctx, element& e, context_function f)
   {
      if (&e == this)
      {
         f(ctx);
      }
      else if (_selected_index < size())
      {
         rect bounds = bounds_of(ctx, _selected_index);
         auto& elem = at(_selected_index);
         context ectx{ctx, &elem, bounds};
         elem.in_context_do(ectx, e, f);
      }
   }

   layer_element::hit_info deck_element::hit_element(context const& ctx, point p, bool control) const
   {
      if (_selected_index >= size())
         return hit_info{{}, {}, rect{}, -1};
      auto& e = at(_selected_index);
      if (!control || e.wants_control())
      {
         rect bounds = bounds_of(ctx, _selected_index);
         if (bounds.includes(p))
         {
            context ectx{ctx, &e, bounds};
            if (auto leaf = e.hit_test(ectx, p, true, control))
               return hit_info{&e, leaf, bounds, int(_selected_index)};
         }
      }
      return hit_info{{}, {}, rect{}, -1};
   }

   bool deck_element::key(context const& ctx, key_info k)
   {
      // Only the selected page takes keys, and Tab moves the focus into the
      // selected page alone: the hidden pages are never focused.
      if (_selected_index >= size())
         return false;
      auto& e = at(_selected_index);
      rect bounds = bounds_of(ctx, _selected_index);
      if (!intersects(bounds, ctx.view_bounds()))
         return false;

      context ectx{ctx, &e, bounds};
      if (focus_index() == int(_selected_index))
         return e.key(ectx, k);

      if ((k.action == key_action::press || k.action == key_action::repeat)
         && k.key == key_code::tab && e.wants_focus())
      {
         bool backward = k.modifiers & mod_shift;
         focus(_selected_index);
         e.begin_focus(backward? from_bottom : from_top);
         ctx.view.refresh(ctx);
         return true;
      }
      return false;
   }

   bool deck_element::text(context const& ctx, text_info info)
   {
      if (_selected_index >= size() || focus_index() != int(_selected_index))
         return false;
      auto& e = at(_selected_index);
      context ectx{ctx, &e, bounds_of(ctx, _selected_index)};
      return e.text(ectx, info);
   }

   bool deck_element::wants_focus() const
   {
      return _selected_index < size() && at(_selected_index).wants_focus();
   }

   void deck_element::begin_focus(focus_request req)
   {
      if (_selected_index >= size())
         return;
      auto& e = at(_selected_index);
      if (e.wants_focus())
      {
         focus(_selected_index);
         e.begin_focus(req);
      }
   }

   bool deck_element::hotkey(context const& ctx, key_info k)
   {
      // Every page is asked, for when it is selected; the selected page
      // alone takes the key.
      if (is_hotkey_test(k))
         return layer_element::hotkey(ctx, k);
      if (!ctx.enabled || _selected_index >= size())
         return false;
      auto& e = at(_selected_index);
      context ectx{ctx, &e, bounds_of(ctx, _selected_index)};
      return e.hotkey(ectx, k);
   }

   void deck_element::select(std::size_t index)
   {
      if (index >= size() || index == _selected_index)
         return;

      // The page losing the selection gives up the focus; if the deck had
      // the focus, the newly selected page takes it.
      bool had_focus = focus_index() != -1;
      if (had_focus)
         composite_base::end_focus();
      _selected_index = index;
      if (had_focus)
         begin_focus(from_top);
   }

   std::string layer_element::class_name() const
   {
      return "layer";
   }

   std::string deck_element::class_name() const
   {
      return "deck";
   }
}
