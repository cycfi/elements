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
            view_limits l = {{ w_limits.min, 0 }, { w_limits.max, full_extent }};
            for (std::size_t i = 0; i != size; ++i)
               l.min.y += _composer->line_height(i);
            l.max.y = l.min.y;
            return l;
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

      for (; it != _rows.end(); ++it)
      {
         auto& row = *it;
         context rctx { ctx, row.elem_ptr.get(), ctx.bounds };
         rctx.bounds.top = top + row.pos;
         rctx.bounds.height(row.height);
         if (intersects(clip_extent, rctx.bounds))
         {
            if (row.layout_id != _layout_id)
            {
               row.elem_ptr->layout(rctx);
               row.layout_id = _layout_id;
            }
            row.elem_ptr->draw(rctx);
         }
         if (rctx.bounds.top > clip_extent.bottom)
            break;
      }
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

   void dynamic::build()
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
               _rows.push_back({ y, line_height, _composer->compose(i) });
               y += line_height;
            }
         }
      }
      ++_layout_id;
   }
}}

