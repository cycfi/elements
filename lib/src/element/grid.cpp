/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/grid.hpp>
#include <elements/support/context.hpp>
#include <elements/element/traversal.hpp>
#include <infra/assert.hpp>

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Vertical Grids
   ////////////////////////////////////////////////////////////////////////////
   view_limits vgrid_element::limits(basic_context const& ctx) const
   {
      _num_spans = 0;
      for (std::size_t i = 0; i != size(); ++i)
         _num_spans += at(i).span();

      view_limits limits{{0.0, 0.0}, {full_extent, 0.0}};
      std::size_t gi = 0;
      float prev = 0;
      float desired_total_min = 0;

      for (std::size_t i = 0; i != size();  ++i)
      {
         auto& elem = at(i);
         gi += elem.span()-1;
         auto y = get_grid_coord(gi++);
         auto height = y - prev;
         auto factor = 1.0/height;
         prev = y;

         auto el = elem.limits(ctx);
         auto elem_desired_total_min = el.min.y * factor;
         if (desired_total_min < elem_desired_total_min)
            desired_total_min = elem_desired_total_min;

         limits.max.y += el.max.y;
         clamp_min(limits.min.x, el.min.x);
         clamp_max(limits.max.x, el.max.x);
      }

      limits.min.y = desired_total_min;
      clamp_min(limits.max.x, limits.min.x);
      clamp_max(limits.max.y, full_extent);
      return limits;
   }

   void vgrid_element::layout(context const& ctx)
   {
      _positions.resize(size()+1);

      auto left = ctx.bounds.left;
      auto right = ctx.bounds.right;
      auto total_height = ctx.bounds.height();
      std::size_t gi = 0;

      float prev = 0;
      for (std::size_t i = 0; i != size(); ++i)
      {
         auto& elem = at(i);
         gi += elem.span()-1;
         auto y = get_grid_coord(gi++) * total_height;
         auto height = y - prev;
         rect ebounds = {left, prev, right, prev+height};
         elem.layout(context{ctx, &elem, ebounds});
         _positions[i] = prev;
         prev = y;
      }
      _positions[size()] = total_height;
   }

   rect vgrid_element::bounds_of(context const& ctx, std::size_t index) const
   {
      if (index >= _positions.size() || index >= size())
         return {};
      auto left = ctx.bounds.left;
      auto top = ctx.bounds.top;
      auto right = ctx.bounds.right;
      return {left, _positions[index]+top, right, _positions[index+1]+top};
   }

   ////////////////////////////////////////////////////////////////////////////
   // Horizontal Grids
   ////////////////////////////////////////////////////////////////////////////
   view_limits hgrid_element::limits(basic_context const& ctx) const
   {
      _num_spans = 0;
      for (std::size_t i = 0; i != size(); ++i)
         _num_spans += at(i).span();

      view_limits limits{{ 0.0, 0.0}, {0.0, full_extent}};
      std::size_t gi = 0;
      float prev = 0;
      float desired_total_min = 0;

      for (std::size_t i = 0; i != size();  ++i)
      {
         auto& elem = at(i);
         gi += elem.span()-1;
         auto x = get_grid_coord(gi++);
         auto width = x - prev;
         auto factor = 1.0/width;
         prev = x;

         auto el = elem.limits(ctx);
         auto elem_desired_total_min = el.min.x * factor;
         if (desired_total_min < elem_desired_total_min)
            desired_total_min = elem_desired_total_min;

         limits.max.x += el.max.x;
         clamp_min(limits.min.y, el.min.y);
         clamp_max(limits.max.y, el.max.y);
      }

      limits.min.x = desired_total_min;
      clamp_min(limits.max.y, limits.min.y);
      clamp_max(limits.max.x, full_extent);
      return limits;
   }

   void hgrid_element::layout(context const& ctx)
   {
      _positions.resize(size()+1);

      auto top = ctx.bounds.top;
      auto bottom = ctx.bounds.bottom;
      auto total_width = ctx.bounds.width();
      std::size_t gi = 0;

      float prev = 0;
      for (std::size_t i = 0; i != size(); ++i)
      {
         auto& elem = at(i);
         gi += elem.span()-1;
         auto x = get_grid_coord(gi++) * total_width;
         auto width = x - prev;
         rect ebounds = {prev, top, prev+width, bottom};
         elem.layout(context{ctx, &elem, ebounds});
         _positions[i] = prev;
         prev = x;
      }
      _positions[size()] = total_width;
   }

   rect hgrid_element::bounds_of(context const& ctx, std::size_t index) const
   {
      if (index >= _positions.size() || index >= size())
         return {};
      auto top = ctx.bounds.top;
      auto left = ctx.bounds.left;
      auto bottom = ctx.bounds.bottom;
      return {_positions[index]+left, top, _positions[index+1]+left, bottom};
   }

   // The margin around the window that allows resizing
   constexpr float resize_margin = 5.0f;

   element*hgrid_adjuster_element::hit_test(context const& ctx, point p, bool leaf, bool control)
   {
      auto const& b = ctx.bounds;
      if (rect{b.left, b.top, b.left+resize_margin, b.bottom}.includes(p))
         return this;
      return tracker::hit_test(ctx, p, leaf, control);
   }

   bool hgrid_adjuster_element::cursor(context const& ctx, point p, cursor_tracking status)
   {
      if (auto* g = find_parent<grid_base*>(ctx); g && !g->is_fixed())
      {
         auto const& b = ctx.bounds;
         if (rect{b.left, b.top, b.left+resize_margin, b.bottom}.includes(p))
         {
            set_cursor(cursor_type::h_resize);
            return true;
         }
      }
      return tracker::cursor(ctx, p, status);
   }

   // bool hgrid_adjuster_element::click(context const& ctx, mouse_button btn)
   // {
   //    return tracker::click(ctx, btn);
   // }

   // void hgrid_adjuster_element::drag(context const& ctx, mouse_button btn)
   // {
   //    tracker::drag(ctx, btn);
   // }

   void hgrid_adjuster_element::begin_tracking(context const& ctx, tracker_info& track_info)
   {
      if (auto* gctx = find_parent_context<grid_base*>(ctx);
         gctx && gctx->element && !static_cast<grid_base*>(gctx->element)->is_fixed())
      {
         auto const& b = ctx.bounds;
         if (rect{b.left, b.top, b.left+resize_margin, b.bottom}.includes(track_info.current))
         {
            auto g = static_cast<grid_base*>(gctx->element);
            if (auto info = g->hit_element(*gctx, track_info.current, true); info.element_ptr)
            {
               CYCFI_ASSERT(info.index > 0, "info.index cannot be zero!");
               if (auto state = get_state())
                  state->_index = info.index-1;
            }
            track_info.offset.x = track_info.current.x - b.left;
         }
      }
   }

   void hgrid_adjuster_element::keep_tracking(context const& ctx, tracker_info& track_info)
   {
      if (track_info.current == track_info.previous)
         return;

      if (auto* gctx = find_parent_context<grid_base*>(ctx);
         gctx && gctx->element && !static_cast<grid_base*>(gctx->element)->is_fixed())
      {
         if (track_info.current != track_info.previous)
         {
            auto g = static_cast<grid_base*>(gctx->element);
            if (auto state = get_state())
            {
               float coord = g->get_grid_coord(state->_index);
               auto full_width = gctx->bounds.width();
               auto pos = track_info.current.x;
               g->set_grid_coord(state->_index, (pos-gctx->bounds.left) / full_width);

               ctx.view.post(
                  [&view_ = ctx.view, g]()
                  {
                     view_.layout(*g);
                     view_.refresh(*g);
                  }
               );
            }

            // double new_value = value_from_point(ctx, track_info.current);
            // if (_value != new_value)
            // {
            //    ctx.view.refresh(ctx);
            // }
         }
      }
   }

   void hgrid_adjuster_element::end_tracking(context const& ctx, tracker_info& track_info)
   {
      if (auto* gctx = find_parent_context<grid_base*>(ctx);
         gctx && gctx->element && !static_cast<grid_base*>(gctx->element)->is_fixed())
      {
      }

      // double new_value = value_from_point(ctx, track_info.current);
      // if (_value != new_value)
      // {
         // ctx.view.refresh(ctx);
      // }
   }
}
