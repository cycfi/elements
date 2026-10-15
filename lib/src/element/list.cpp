/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/list.hpp>
#include <elements/element/port.hpp>
#include <elements/view.hpp>
#include <algorithm>

namespace cycfi::elements
{
   list::list(composer_ptr composer, bool manage_externally)
    : _composer(composer)
    , _manage_externally(manage_externally)
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
         _relinquish_focus_request = false;
         _requests.clear();
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
         _relinquish_focus_request = false;
         _requests.clear();
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

   // The cells come and go as the list scrolls: the list takes hotkeys,
   // and offers them to the cells it holds now. at() would make the others.
   bool list::hotkey(context const& ctx, key_info k)
   {
      if (is_hotkey_test(k))
         return true;
      if (!ctx.enabled)
         return false;
      for (std::size_t i = 0; i != _cells.size(); ++i)
      {
         auto& e = _cells[i].elem_ptr;
         if (!e)
            continue;
         context ectx{ctx, e.get(), bounds_of(ctx, i)};
         if (e->hotkey(ectx, hotkey_test) && e->hotkey(ectx, k))
            return true;
      }
      return false;
   }

   // Only the cells the list holds now: at() would make the others.
   mime_types list::drop_types() const
   {
      mime_types types;
      for (auto const& cell : _cells)
      {
         if (cell.elem_ptr)
            types.merge(cell.elem_ptr->drop_types());
      }
      return types;
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
      if (_relinquish_focus_request)
      {
         relinquish_focus(*this, ctx);
         _relinquish_focus_request = false;
      }

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
               rctx.enabled = rctx.parent->enabled && cell.elem_ptr->is_enabled();
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
         // From the last cell that starts before the end of the port, back
         // to the first that ends after its start.
         auto it = std::upper_bound(
            _cells.begin(), _cells.end(),
            main_axis_port_end - main_axis_start,
            [](double pivot, auto const& cell)
            {
               return pivot < cell.pos;
            }
         );

         while (it != _cells.begin())
         {
            --it;
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
         }
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

   void list::clear()
   {
      this->_composer->resize(0);
      this->update();
   }

   void list::resize(size_t n)
   {
      this->_composer->resize(n);
      this->update();
   }

   namespace
   {
      // Sorted, without repeats: as move_indices and erase_indices take
      // them.
      list::indices_type sorted(list::indices_type indices)
      {
         std::sort(indices.begin(), indices.end());
         indices.erase(std::unique(indices.begin(), indices.end()), indices.end());
         return indices;
      }
   }

   void list::move(std::size_t pos, indices_type const& indices)
   {
      _requests.push_back({request::move_, pos, 0, sorted(indices)});
   }

   void list::insert(std::size_t pos, std::size_t num_items)
   {
      _requests.push_back({request::insert_, pos, num_items, {}});
   }

   void list::erase(indices_type const& indices)
   {
      _requests.push_back({request::erase_, 0, 0, sorted(indices)});
   }

   // Lay the cells end to end again, after the cells change.
   void list::place_cells(basic_context const& ctx) const
   {
      double y = 0;
      auto size = std::min(_composer->size(), _cells.size());
      for (std::size_t i = 0; i != size; ++i)
      {
         auto main_axis_size = _composer->main_axis_size(i, ctx);
         _cells[i].pos = y;
         _cells[i].main_axis_size = main_axis_size;
         y += main_axis_size;
      }
      _main_axis_full_size = y;
      ++_layout_id;
   }

   void list::move(basic_context const& ctx, request const& r) const
   {
      auto focus = focus_index();
      if (focus != -1)
      {
         // Where the focused cell goes.
         indices_type order(_cells.size());
         for (std::size_t i = 0; i != order.size(); ++i)
            order[i] = i;
         move_indices(order, r.pos, r.indices);
         auto i = std::find(order.begin(), order.end(), std::size_t(focus));
         const_cast<list*>(this)->focus(i - order.begin());
      }
      move_indices(_cells, r.pos, r.indices);
      place_cells(ctx);
   }

   void list::insert(basic_context const& ctx, request const& r) const
   {
      auto pos = std::min(r.pos, _cells.size());
      _composer->resize(_composer->size() + r.num_items);
      _cells.insert(_cells.begin()+pos, r.num_items, cell_info{});

      auto focus = focus_index();
      if (focus != -1 && pos <= std::size_t(focus))
         const_cast<list*>(this)->focus(focus + r.num_items);
      place_cells(ctx);
   }

   void list::erase(basic_context const& ctx, request const& r) const
   {
      auto const& indices = r.indices;
      _composer->resize(_composer->size() - indices.size());
      erase_indices(_cells, indices);

      // Let go of the focus if its cell is gone, or follow it if not.
      auto focus = focus_index();
      if (focus != -1)
      {
         if (std::binary_search(indices.begin(), indices.end(), std::size_t(focus)))
         {
            _relinquish_focus_request = true;
         }
         else
         {
            auto before = std::lower_bound(indices.begin(), indices.end(), std::size_t(focus))
               - indices.begin();
            const_cast<list*>(this)->focus(focus - before);
         }
      }
      place_cells(ctx);
   }

   void list::sync(basic_context const& ctx) const
   {
      // An update makes every cell again: the requests before it are moot.
      if (_update_request)
      {
         update(ctx);
         _requests.clear();
      }
      for (auto const& r : _requests)
      {
         switch (r.kind)
         {
            case request::move_:    move(ctx, r); break;
            case request::insert_:  insert(ctx, r); break;
            case request::erase_:   erase(ctx, r); break;
         }
      }
      _requests.clear();
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

   std::string list::class_name() const
   {
      return "list";
   }

   std::string hlist::class_name() const
   {
      return "hlist";
   }
}


