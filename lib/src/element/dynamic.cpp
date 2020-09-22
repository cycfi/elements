/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/dynamic.hpp>
#include <elements/view.hpp>

namespace cycfi { namespace elements
{
   view_limits dynamic::limits(basic_context const& /*ctx*/) const
   {
      if (_composer)
      {
         auto w_limits = _composer->width_limits();
         if (auto size = _composer->size())
         {
            return {
               { w_limits.min, float(_height) }
             , { w_limits.max, float(_height) }
            };
         }
      }
      return {{ 0, 0 }, { 0, 0 }};
   }

   void dynamic::draw(context const& ctx)
   {
      auto& cnv = ctx.canvas;
      auto  state = cnv.new_state();
      auto  clip_extent = cnv.clip_extent();
      auto  top = ctx.bounds.top;

      cnv.rect(ctx.bounds);
      cnv.clip();

      if (!intersects(ctx.bounds, clip_extent))
         return;

      auto it = std::lower_bound(_rows.begin(), _rows.end(),
         clip_extent.top-top,
         [](auto const& row, double pivot)
         {
            return (row.pos + row.height) < pivot;
         }
      );

      // Draw the rows within the visible bounds of the view
      std::size_t new_start = it - _rows.begin();
      for (; it != _rows.end(); ++it)
      {
         auto& row = *it;
         context rctx { ctx, row.elem_ptr.get(), ctx.bounds };
         rctx.bounds.top = top + row.pos;
         rctx.bounds.height(row.height);
         if (intersects(clip_extent, rctx.bounds))
         {
            if (!row.elem_ptr)
            {
               row.elem_ptr = _composer->compose(it-_rows.begin());
               row.elem_ptr->layout(rctx);
               row.layout_id = _layout_id;
            }
            else if (row.layout_id != _layout_id)
            {
               row.elem_ptr->layout(rctx);
               row.layout_id = _layout_id;
            }
            row.elem_ptr->draw(rctx);
         }
         if (rctx.bounds.top > clip_extent.bottom)
            break;
      }
      std::size_t new_end = it - _rows.begin();

      // Cleanup old rows
      if (new_start != _previous_window_start || new_end != _previous_window_end)
      {
         for (auto i = _previous_window_start; i != _previous_window_end; ++i)
         {
            if (i < new_start || i >= new_end)
            {
               _rows[i].elem_ptr.reset();
               _rows[i].layout_id = -1;
            }
         }
      }

      _previous_window_start = new_start;
      _previous_window_end = new_end;
      _previous_size.x = ctx.bounds.width();
      _previous_size.y = ctx.bounds.height();
   }

   void dynamic::layout(context const& ctx)
   {
      if (_previous_size.x != ctx.bounds.width() ||
         _previous_size.y != ctx.bounds.height())
      {
         _previous_size.x = ctx.bounds.width();
         _previous_size.y = ctx.bounds.height();
         ++_layout_id;
      }
   }

   void dynamic::update()
   {
      if (_composer)
      {
         if (auto size = _composer->size())
         {
            double y = 0;
            _rows.reserve(_composer->size());
            for (std::size_t i = 0; i != size; ++i)
            {
               auto line_height = _composer->line_height(i);
               _rows.push_back({ y, line_height, nullptr });
               y += line_height;
            }
            _height = y;
         }
      }
      ++_layout_id;
   }
}}

