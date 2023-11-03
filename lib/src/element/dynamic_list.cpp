/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/dynamic_list.hpp>
#include <elements/element/port.hpp>
#include <elements/view.hpp>

namespace cycfi { namespace elements
{
   std::size_t dynamic_list::size() const
   {
      return _cells.size();
   }

   element& dynamic_list::at(std::size_t ix) const
   {
      if (_cells[ix].elem_ptr)
         return *_cells[ix].elem_ptr.get();
      return *(_cells[ix].elem_ptr =_composer->compose(ix)).get();
   }

   view_limits dynamic_list::limits(basic_context const& ctx) const
   {
      if (_composer)
      {
         if (_update_request)
         {
            update(ctx);
         }
         auto secondary_limits = _composer->secondary_axis_limits(ctx);
         if (_composer->size())
         {
            return make_limits(float(_main_axis_full_size),  secondary_limits);
         }
      }
      return {{0, 0}, {0, 0}};
   }

   void dynamic_list::draw(context const& ctx)
   {
      // Johann Philippe : this seems to be necessary for context where a
      // hdynamic_list is inside vdynamic_list (2D tables)
      if (_update_request)
         update(ctx);

      auto& cnv = ctx.canvas;
      auto  state = cnv.new_state();
      auto  clip_extent = cnv.clip_extent();
      auto  main_axis_start = get_main_axis_start(ctx.bounds);
      auto  main_axis_clip_end = get_main_axis_end(clip_extent);

      if (!intersects(ctx.bounds, clip_extent))
         return;

      auto it = std::lower_bound(_cells.begin(), _cells.end(),
         get_main_axis_start(clip_extent) - main_axis_start,
         [](auto const& cell, double pivot)
         {
            return (cell.pos + cell.main_axis_size) < pivot;
         }
      );

      // Draw the rows within the visible bounds of the view
      std::size_t new_start = it - _cells.begin();

      for (; it != _cells.end(); ++it)
      {
         auto& cell = *it;
         context rctx {ctx, cell.elem_ptr.get(), ctx.bounds};
         set_bounds(rctx, main_axis_start, cell);
         if (intersects(clip_extent, rctx.bounds))
         {
            if (!cell.elem_ptr)
            {
               cell.elem_ptr = at(it-_cells.begin()).shared_from_this();
               cell.elem_ptr->layout(rctx);
               cell.layout_id = _layout_id;
            }
            else if (cell.layout_id != _layout_id)
            {
               cell.elem_ptr->layout(rctx);
               cell.layout_id = _layout_id;
            }
            cell.elem_ptr->draw(rctx);
         }

         if (get_main_axis_start(rctx.bounds) > main_axis_clip_end)
            break;
      }

      std::size_t new_end = it - _cells.begin();

      // Cleanup old rows
      if (new_start != _previous_window_start || new_end != _previous_window_end)
      {
         for (auto i = _previous_window_start; i != _previous_window_end; ++i)
         {
            if ((i < new_start || i >= new_end) && i < _cells.size())
            {
               _cells[i].layout_id = -1;
               _cells[i].elem_ptr.reset();
            }
         }
      }

      _previous_window_start = new_start;
      _previous_window_end = new_end;
      _previous_size.x = ctx.bounds.width();
      _previous_size.y = ctx.bounds.height();
   }

   void dynamic_list::for_each_visible(
      context const& ctx
    , for_each_callback f
    , bool reverse
   ) const
   {
      auto port_bounds = get_port_bounds(ctx);
      if (!intersects(ctx.bounds, port_bounds))
         return;

      auto main_axis_start = get_main_axis_start(ctx.bounds);
      auto main_axis_port_start = get_main_axis_start(port_bounds);
      auto main_axis_port_end = get_main_axis_end(port_bounds);

      if (reverse)
      {
         auto it = std::upper_bound(
            _cells.begin(), _cells.end(),
            main_axis_port_end - main_axis_start,
            [](double pivot, auto const& cell)
            {
               return pivot < (cell.pos + cell.main_axis_size);
            }
         );

         do
         {
            auto& cell = *it;
            rect bounds = ctx.bounds;
            set_bounds(bounds, main_axis_start, cell);
            if (intersects(port_bounds, bounds))
            {
               if (cell.elem_ptr && f(*cell.elem_ptr, it-_cells.begin(), bounds))
                  break;
            }
            if (main_axis_port_start > get_main_axis_end(bounds))
               break;

            ++it;
         }
         while (it != _cells.begin());
      }
      else
      {
         auto it = std::lower_bound(
            _cells.begin(), _cells.end(),
            main_axis_port_start - main_axis_start,
            [](auto const& cell, double pivot)
            {
               return (cell.pos + cell.main_axis_size) < pivot;
            }
         );

         while (it != _cells.end())
         {
            auto& cell = *it;
            rect bounds = ctx.bounds;
            set_bounds(bounds, main_axis_start, cell);
            if (intersects(port_bounds, bounds))
            {
               if (cell.elem_ptr && f(*cell.elem_ptr, it-_cells.begin(), bounds))
                  break;
            }
            if (get_main_axis_start(bounds) > main_axis_port_end)
               break;

            ++it;
         }
      }
   }

   void dynamic_list::layout(context const& ctx)
   {
      if (_previous_size.x != ctx.bounds.width() ||
         _previous_size.y != ctx.bounds.height())
      {
         _previous_size.x = ctx.bounds.width();
         _previous_size.y = ctx.bounds.height();
         ++_layout_id;
      }
   }

   void dynamic_list::update()
   {
      _update_request = true;
      _cells.clear();
      _main_axis_full_size = 0;
   }

   void dynamic_list::update(basic_context const& ctx) const
   {
      if (_composer)
      {
         if (auto size = _composer->size())
         {
            double y = 0;
            _cells.reserve(_composer->size());
            for (std::size_t i = 0; i != size; ++i)
            {
               auto main_axis_size = _composer->main_axis_size(i, ctx);
               _cells.push_back({y, main_axis_size, nullptr});
               y += main_axis_size;
            }
            _main_axis_full_size = y;
         }
      }
      ++_layout_id;
      _update_request = false;
   }

   void dynamic_list::resize(size_t n)
   {
      this->_composer->resize(n);
      this->update();
   }

   ////////////////////////////////////////////////////////////////////////////
   // Vertical dynamic_list methods
   ////////////////////////////////////////////////////////////////////////////
   float dynamic_list::get_main_axis_start(const rect &r) const
   {
      return r.top;
   }

   float dynamic_list::get_main_axis_end(const rect &r) const
   {
      return r.bottom;
   }

   view_limits dynamic_list::make_limits(float main_axis_size, cell_composer::limits secondary_axis_limits) const
   {
      return {
         {secondary_axis_limits.min, float(main_axis_size)}
       , {secondary_axis_limits.max, float(main_axis_size)}};
   }

   void dynamic_list::set_bounds(rect& r, float main_axis_pos, cell_info &cell) const
   {
      r.top = main_axis_pos + cell.pos;
      r.height(cell.main_axis_size);
   }

   rect dynamic_list::bounds_of(context const& ctx, std::size_t ix) const
   {
      rect r = ctx.bounds;
      r.top = ctx.bounds.top + _cells[ix].pos;
      r.height(_cells[ix].main_axis_size);
      return r;
   }

   ////////////////////////////////////////////////////////////////////////////
   // Horizontal dynamic_list methods
   ////////////////////////////////////////////////////////////////////////////

   float hdynamic_list::get_main_axis_start(const rect &r) const
   {
      return r.left;
   }

   float hdynamic_list::get_main_axis_end(const rect &r) const
   {
      return r.right;
   }

   view_limits hdynamic_list::make_limits(float main_axis_size, cell_composer::limits secondary_axis_limits) const
   {
      return {
         {main_axis_size, secondary_axis_limits.min}
       , {main_axis_size, secondary_axis_limits.max}};
   }

   void hdynamic_list::set_bounds(rect& r, float main_axis_pos, cell_info &cell) const
   {
      r.left = main_axis_pos + cell.pos;
      r.width(cell.main_axis_size);
   }

   rect hdynamic_list::bounds_of(context const& ctx, std::size_t ix) const
   {
      rect r = ctx.bounds;
      r.left = ctx.bounds.left + _cells[ix].pos;
      r.width(_cells[ix].main_axis_size);
      return r;
   }

}}


