/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/list.hpp>
#include <elements/element/port.hpp>
#include <elements/view.hpp>

namespace cycfi { namespace elements
{
   list::list(composer_ptr composer, bool manage_externally)
    : _composer(composer)
    , _manage_externally(manage_externally)
    , _update_request{true}
    , _move_request{false}
    , _insert_request{false}
    , _erase_request{false}
   {}

   list::list(list const& rhs)
    : _cells{rhs._cells}
    , _composer{rhs._composer}
    , _manage_externally{rhs._manage_externally}
    , _previous_size{rhs._previous_size}
    , _previous_window_start{rhs._previous_window_start}
    , _previous_window_end{rhs._previous_window_end}
    , _main_axis_full_size{rhs._main_axis_full_size}
    , _layout_id{rhs._layout_id}
    , _update_request{true}
    , _move_request{false}
    , _insert_request{false}
    , _erase_request{false}
    , _request_info{nullptr}
   {}

   list::list(list&& rhs)
    : _cells{std::move(rhs._cells)}
    , _composer{rhs._composer}
    , _manage_externally{rhs._manage_externally}
    , _previous_size{rhs._previous_size}
    , _previous_window_start{rhs._previous_window_start}
    , _previous_window_end{rhs._previous_window_end}
    , _main_axis_full_size{rhs._main_axis_full_size}
    , _layout_id{rhs._layout_id}
    , _update_request{true}
    , _move_request{false}
    , _insert_request{false}
    , _erase_request{false}
    , _request_info{nullptr}
   {}

   list& list::operator=(list const& rhs)
   {
      if (this != &rhs)
      {
         _cells = rhs._cells;
         _composer = rhs._composer;
         _manage_externally = rhs._manage_externally;
         _previous_size = rhs._previous_size;
         _previous_window_start = rhs._previous_window_start;
         _previous_window_end = rhs._previous_window_end;
         _main_axis_full_size = rhs._main_axis_full_size;
         _layout_id = rhs._layout_id;
         _update_request = true;
         _move_request = false;
         _insert_request = false;
         _erase_request = false;
         _request_info.reset();
      }
      return *this;
   }

   list& list::operator=(list&& rhs)
   {
      if (this != &rhs)
      {
         _cells = std::move(rhs._cells);
         _composer = rhs._composer;
         _manage_externally = rhs._manage_externally;
         _previous_size = rhs._previous_size;
         _previous_window_start = rhs._previous_window_start;
         _previous_window_end = rhs._previous_window_end;
         _main_axis_full_size = rhs._main_axis_full_size;
         _layout_id = rhs._layout_id;
         _update_request = true;
         _move_request = false;
         _insert_request = false;
         _erase_request = false;
         _request_info.reset();
      }
      return *this;
   }

   std::size_t list::size() const
   {
      return _cells.size();
   }

   element& list::at(std::size_t ix) const
   {
      if (_cells[ix].elem_ptr)
         return *_cells[ix].elem_ptr.get();
      return *(_cells[ix].elem_ptr =_composer->compose(ix)).get();
   }

   view_limits list::limits(basic_context const& ctx) const
   {
      if (_composer)
      {
         sync(ctx);
         auto secondary_limits = _composer->secondary_axis_limits(ctx);
         return make_limits(float(_main_axis_full_size),  secondary_limits);
      }
      return {{0, 0}, {0, 0}};
   }

   void list::draw(context const& ctx)
   {
      sync(ctx);

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

      // Cleanup old rows
      if (_manage_externally)
      {
         std::size_t new_end = it - _cells.begin();
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
      }

      _previous_size.x = ctx.bounds.width();
      _previous_size.y = ctx.bounds.height();
   }

   void list::for_each_visible(
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

   void list::layout(context const& ctx)
   {
      if (_previous_size.x != ctx.bounds.width() ||
         _previous_size.y != ctx.bounds.height())
      {
         _previous_size.x = ctx.bounds.width();
         _previous_size.y = ctx.bounds.height();
         ++_layout_id;
      }
   }

   void list::update()
   {
      _update_request = true;
      _cells.clear();
      _main_axis_full_size = 0;
   }

   void list::update(basic_context const& ctx) const
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

   void list::resize(size_t n)
   {
      this->_composer->resize(n);
      this->update();
   }

   void list::move(std::size_t pos, indices_type const& indices)
   {
      _move_request = true;
      if (!_request_info)
      {
         _request_info = std::make_unique<request_info>();
         _request_info->_move_pos = pos;
         _request_info->_move_indices = indices;
      }
   }

   void list::insert(std::size_t pos, std::size_t num_items)
   {
      _insert_request = true;
      if (!_request_info)
      {
         _request_info = std::make_unique<request_info>();
         _request_info->_insert_pos = pos;
         _request_info->_insert_num_items = num_items;
      }
   }

   void list::erase(indices_type const& indices)
   {
      _erase_request = true;
      if (!_request_info)
      {
         _request_info = std::make_unique<request_info>();
         _request_info->_delete_indices = indices;
      }
   }

   void list::move(basic_context const& ctx) const
   {
      auto const& _move_indices = _request_info->_move_indices;
      auto _move_pos = _request_info->_move_pos;
      move_indices(_cells, _move_pos, _move_indices);

      double y = 0;
      auto size = _composer->size();
      for (std::size_t i = 0; i != size; ++i)
      {
         auto main_axis_size = _composer->main_axis_size(i, ctx);
         _cells[i].pos = y;
         _cells[i].main_axis_size = main_axis_size;
         y += main_axis_size;
      }
      ++_layout_id;
      _move_request = false;
   }

   void list::insert(basic_context const& ctx) const
   {
      auto _insert_pos = _request_info->_insert_pos;
      auto _insert_num_items = _request_info->_insert_num_items;

      this->_composer->resize(this->_composer->size() + _insert_num_items);
      _cells.insert(_cells.begin()+_insert_pos, _insert_num_items, cell_info{});

      double y = 0;
      auto size = _composer->size();
      _main_axis_full_size = 0;
      for (std::size_t i = 0; i != size; ++i)
      {
         auto main_axis_size = _composer->main_axis_size(i, ctx);
         _cells[i].pos = y;
         _cells[i].main_axis_size = main_axis_size;
         y += main_axis_size;
         _main_axis_full_size = y;
      }

      ++_layout_id;
      _insert_request = false;
   }

   void list::erase(basic_context const& ctx) const
   {
      auto const& _delete_indices = _request_info->_delete_indices;
      this->_composer->resize(this->_composer->size() - _delete_indices.size());
      erase_indices(_cells, _delete_indices);

      double y = 0;
      auto size = _composer->size();
      _main_axis_full_size = 0;
      for (std::size_t i = 0; i != size; ++i)
      {
         auto main_axis_size = _composer->main_axis_size(i, ctx);
         _cells[i].pos = y;
         _cells[i].main_axis_size = main_axis_size;
         y += main_axis_size;
         _main_axis_full_size = y;
      }

      ++_layout_id;
      _erase_request = false;
   }

   void list::sync(basic_context const& ctx) const
   {
      if (_update_request)
         update(ctx);
      if (_request_info)
      {
         if (_move_request)
            move(ctx);
         if (_insert_request)
            insert(ctx);
         if (_erase_request)
            erase(ctx);
      }
      _request_info.reset();
   }

   ////////////////////////////////////////////////////////////////////////////
   // Vertical list methods
   ////////////////////////////////////////////////////////////////////////////
   float list::get_main_axis_start(const rect &r) const
   {
      return r.top;
   }

   float list::get_main_axis_end(const rect &r) const
   {
      return r.bottom;
   }

   view_limits list::make_limits(float main_axis_size, cell_composer::limits secondary_axis_limits) const
   {
      return {
         {secondary_axis_limits.min, float(main_axis_size)}
       , {secondary_axis_limits.max, float(main_axis_size)}};
   }

   void list::set_bounds(rect& r, float main_axis_pos, cell_info &cell) const
   {
      r.top = main_axis_pos + cell.pos;
      r.height(cell.main_axis_size);
   }

   rect list::bounds_of(context const& ctx, std::size_t ix) const
   {
      rect r = ctx.bounds;
      r.top = ctx.bounds.top + _cells[ix].pos;
      r.height(_cells[ix].main_axis_size);
      return r;
   }

   ////////////////////////////////////////////////////////////////////////////
   // Horizontal list methods
   ////////////////////////////////////////////////////////////////////////////

   float hlist::get_main_axis_start(const rect &r) const
   {
      return r.left;
   }

   float hlist::get_main_axis_end(const rect &r) const
   {
      return r.right;
   }

   view_limits hlist::make_limits(float main_axis_size, cell_composer::limits secondary_axis_limits) const
   {
      return {
         {main_axis_size, secondary_axis_limits.min}
       , {main_axis_size, secondary_axis_limits.max}};
   }

   void hlist::set_bounds(rect& r, float main_axis_pos, cell_info &cell) const
   {
      r.left = main_axis_pos + cell.pos;
      r.width(cell.main_axis_size);
   }

   rect hlist::bounds_of(context const& ctx, std::size_t ix) const
   {
      rect r = ctx.bounds;
      r.left = ctx.bounds.left + _cells[ix].pos;
      r.width(_cells[ix].main_axis_size);
      return r;
   }

}}


