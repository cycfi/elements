/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/dynamic_list.hpp>
#include <elements/view.hpp>

namespace cycfi { namespace elements
{
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
      return {{ 0, 0 }, { 0, 0 }};
   }

   void dynamic_list::draw(context const& ctx)
   {
       // Johann Philippe : this seems to be necessary for context where a hdynamic_list is inside vdynamic_list (2D tables)
      if (_update_request)
           update(ctx);

      auto& cnv = ctx.canvas;
      auto  state = cnv.new_state();
      auto  clip_extent = cnv.clip_extent();
      auto main_axis_start = get_main_axis_start(ctx.bounds);

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
         context rctx { ctx, cell.elem_ptr.get(), ctx.bounds };
         make_bounds(rctx, main_axis_start, cell);
         if (intersects(clip_extent, rctx.bounds))
         {
            if (!cell.elem_ptr)
            {
               cell.elem_ptr = _composer->compose(it-_cells.begin());
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

         if(get_main_axis_start(rctx.bounds) > get_main_axis_end(clip_extent))
             break;
      }
      std::size_t new_end = it - _cells.begin();

      // Cleanup old rows
      if (new_start != _previous_window_start || new_end != _previous_window_end)
      {
         for (auto i = _previous_window_start; i != _previous_window_end; ++i)
         {
            if (i < new_start || i >= new_end)
            {
               _cells[i].layout_id = -1;
            }
         }
      }

      _previous_window_start = new_start;
      _previous_window_end = new_end;
      _previous_size.x = ctx.bounds.width();
      _previous_size.y = ctx.bounds.height();
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
               _cells.push_back({ y, main_axis_size, nullptr });
               y += main_axis_size;
            }
            _main_axis_full_size = y;
         }
      }
      ++_layout_id;
      _update_request = false;
   }


   bool dynamic_list::click(const context &ctx, mouse_button btn)
   {
       if (!_cells.empty())
       {
          if (btn.down) // button down
          {
             hit_info info = hit_element(ctx, btn.pos, true);
             if (info.element)
             {
                if (wants_focus() && _focus != info.index)
                   new_focus(ctx, info.index);

                context ectx{ ctx, info.element.get(), info.bounds };
                if (info.element->click(ectx, btn))
                {
                   if (btn.down)
                      _click_tracking = info.index;
                   return true;
                }
             }
          }
          else if (_click_tracking != -1) // button up
          {
             rect  bounds = bounds_of(ctx, _click_tracking);
             auto& e = *_composer->compose(_click_tracking);
             context ectx{ ctx, &e, bounds };
             if (e.click(ectx, btn))
             {
                return true;
             }
          }
       }
       _click_tracking = -1;
       return false;
   }

   bool dynamic_list::text(context const& ctx, text_info info)
   {
      if (_focus != -1)
      {
         rect  bounds = bounds_of(ctx, _focus);
         auto& focus_ = *_cells[_focus].elem_ptr;
         context ectx{ ctx, &focus_, bounds };
         return focus_.text(ectx, info);
      };
      return false;
   }

   void dynamic_list::new_focus(context const& ctx, int index)
   {
       if (_focus != -1 )
       {
           _composer->compose(_focus)->end_focus();
           ctx.view.refresh(ctx);
       }

       // start a new focus
       _focus = index;
       if (_focus != -1 && _cells[_focus].elem_ptr != nullptr)

       {
           _cells[_focus].elem_ptr->begin_focus();
           ctx.view.refresh(ctx);
       }
   }


   bool dynamic_list::key(const context &ctx, key_info k)
   {
       auto&& try_key = [&](int ix) -> bool
       {
           rect bounds = bounds_of(ctx, ix);
           auto& e = *_composer->compose(ix).get();
           context ectx{ ctx, &e, bounds };
           bool b = e.key(ectx, k);
           return b;
       };


       auto&& try_focus = [&](int ix) -> bool
       {
           if (_composer->compose(ix)->wants_focus())
           {
               new_focus(ctx, ix);
               return true;
           }
           return false;
       };

       if (_focus != -1)
       {
           // when tab is pressed at the end of the scroller, it doesn't automatically scroll
           if (try_key(_focus))
               return true;
       }


       if ((k.action == key_action::press || k.action == key_action::repeat)
               && k.key == key_code::tab && _cells.size())
       {
           int next_focus = _focus;
           bool reverse = (k.modifiers & mod_shift) ^ reverse_index();

           if( (next_focus == -1)  || !reverse)
           {
               while (++next_focus != static_cast<int>(_cells.size()))
               {
                   if (try_focus(next_focus))
                       return true;
               }
               return false;
           }
           else
           {
               while (--next_focus >= 0)
               {
                   if (_composer->compose(next_focus)->wants_focus())
                       if (try_focus(next_focus))
                           return true;
               }
               return false;
           }
       }
       // Johann Philippe : The following code comes from composite_base.
       // I commented that since it caused crashes. Still don't know if that can be useful to adapt.

       // If we reached here, then there's either no focus, or the
       // focus did not handle the key press.

       /*
       if (reverse_index())
       {
          for (int ix = int(_cells.size())-1; ix >= 0; --ix)
          {
             if (try_key(ix))
                return true;
          }
       }
       else
       {
          for (std::size_t ix = 0; ix < _cells.size(); ++ix)
          {
             if (try_key(ix))
                return true;
          }
       }
       */
       return false;
   }

   bool dynamic_list::cursor(const context &ctx, point p, cursor_tracking status)
   {

      if (_cursor_tracking >= int(_cells.size())) // just to be sure!
         _cursor_tracking = -1;

      // Send cursor leaving to all currently hovering elements if cursor is
      // leaving the composite.
      if (status == cursor_tracking::leaving)
      {
         for (auto ix : _cursor_hovering)
         {
            if (ix < int(_cells.size()))
            {
               auto& e = *_composer->compose(ix).get();
               context ectx{ ctx, &e, bounds_of(ctx, ix) };
               e.cursor(ectx, p, cursor_tracking::leaving);
            }
         }
         return false;
      }

      // Send cursor leaving to all currently hovering elements if p is
      // outside the elements's bounds or if the element is no longer hit.
      for (auto i = _cursor_hovering.begin(); i != _cursor_hovering.end();)
      {
         if (*i < int(_cells.size()))
         {
            auto& e = *_composer->compose(*i).get();
            rect  b = bounds_of(ctx, *i);
            context ectx{ ctx, &e, b };
            if (!b.includes(p) || !e.hit_test(ectx, p))
            {
               e.cursor(ectx, p, cursor_tracking::leaving);
               i = _cursor_hovering.erase(i);
               continue;
            }
         }
         ++i;
      }

      // Send cursor entering to newly hit element or hovering to current
      // tracking element
      hit_info info = hit_element(ctx, p, true);
      if (info.element)
      {
         _cursor_tracking = info.index;
         status = cursor_tracking::hovering;
         if (_cursor_hovering.find(info.index) == _cursor_hovering.end())
         {
             status = cursor_tracking::entering;
            _cursor_hovering.insert(_cursor_tracking);
         }
         auto& e = *_composer->compose(_cursor_tracking).get();
         context ectx{ ctx, &e, bounds_of(ctx, _cursor_tracking) };
         return e.cursor(ectx, p, status);
      }

      return false;
   }

   void dynamic_list::drag(const context &ctx, mouse_button btn)
   {
       if (_click_tracking != -1)
       {
          rect  bounds = bounds_of(ctx, _click_tracking);
          auto& e = *_composer->compose(_click_tracking).get();
          context ectx{ ctx, &e, bounds };
          e.drag(ectx, btn);
       }
   }


   bool dynamic_list::scroll(context const& ctx, point dir, point p)
   {
      if (!_cells.empty())
      {
         hit_info info = hit_element(ctx, p, true);
         if (auto ptr = info.element; ptr && elements::intersects(info.bounds, ctx.view_bounds()))
         {
            context ectx{ ctx, ptr.get(), info.bounds };
            return ptr->scroll(ectx, dir, p);
         }
      }
      return false;
   }


   bool dynamic_list::wants_focus() const
   {
       for (std::size_t ix = 0; ix != _cells.size(); ++ix)
           if (_cells[ix].elem_ptr != nullptr && _cells[ix].elem_ptr->wants_focus())
               return true;
       return false;
   }

   bool dynamic_list::wants_control() const
   {
      for (std::size_t ix = 0; ix < _cells.size(); ++ix)
         if (_cells[ix].elem_ptr != nullptr && _cells[ix].elem_ptr->wants_control())
            return true;
      return false;
   }

   void dynamic_list::begin_focus()
   {
       if (_focus == -1)
           _focus = _saved_focus;
       if (_focus == -1)
       {
           for (std::size_t ix = 0; ix != _cells.size(); ++ix)
               if (_cells[ix].elem_ptr != nullptr && _cells[ix].elem_ptr->wants_focus())
               {
                   _focus = ix;
                   break;
               }
       }
       if (_focus != -1 && _cells[_focus].elem_ptr != nullptr)
           _cells[_focus].elem_ptr->begin_focus();
   }


   void dynamic_list::end_focus()
   {
       if (_focus != -1 && _cells[_focus].elem_ptr != nullptr)
           _cells[_focus].elem_ptr->end_focus();
       _saved_focus = _focus;
       _focus = -1;
   }


   element const* dynamic_list::focus() const
   {
       return (_cells.empty() || (_focus == -1))? 0 : _cells[_focus].elem_ptr.get();
   }


   element* dynamic_list::focus()
   {
       return (_cells.empty() || (_focus == -1))? 0 : _cells[_focus].elem_ptr.get();
   }


   void dynamic_list::focus(std::size_t index)
   {
       if (index < _cells.size())
           _focus = int(index);
   }


   void dynamic_list::reset()
   {
       _focus = -1;
       _saved_focus = -1;
       _click_tracking = -1;
       _cursor_tracking = -1;
       _cursor_hovering.clear();
   }

   void dynamic_list::resize(size_t n)
   {
       this->_composer->resize(n);
       this->update();
   }

   dynamic_list::hit_info dynamic_list::hit_element(context const& ctx, point p, bool control) const
   {
      auto&& test_element =
         [&](int ix, hit_info& info) -> bool
         {
            if(_cells[ix].elem_ptr == nullptr) {return false;}
            auto& e = *_cells[ix].elem_ptr;
            if (!control || e.wants_control())
            {
               rect bounds = bounds_of(ctx, ix);
               if (bounds.includes(p))
               {
                  context ectx{ ctx, &e, bounds };
                  if (e.hit_test(ectx, p))
                  {
                     info = hit_info{ e.shared_from_this(), bounds, int(ix) };
                     return true;
                  }
               }
            }
            return false;
         };

      hit_info info = hit_info{ {}, rect{}, -1 };
      if (reverse_index())
      {
         for (int ix = int(_cells.size())-1; ix >= 0; --ix)
            if (test_element(ix, info))
               break;
      }
      else
      {
         for (std::size_t ix = 0; ix < _cells.size(); ++ix)
            if (test_element(ix, info))
               break;
      }
      return info;
   }

   ////////////////////////////////////////////////////////////////////////////
   // Vertical dynamic_list methods
   ////////////////////////////////////////////////////////////////////////////
   float dynamic_list::get_main_axis_start(const rect &r)
   {return r.top;}

   float dynamic_list::get_main_axis_end(const rect &r)
   {return r.bottom;}

   view_limits dynamic_list::make_limits(float main_axis_size, cell_composer::limits secondary_axis_limits) const
   {
       return {
          { secondary_axis_limits.min, float(main_axis_size) }
        , { secondary_axis_limits.max, float(main_axis_size) } };
   }

   void dynamic_list::make_bounds(context &ctx, float main_axis_pos, cell_info &cell)
   {
       ctx.bounds.top = main_axis_pos + cell.pos;
       ctx.bounds.height(cell.main_axis_size);
   }

   rect dynamic_list::bounds_of(context const& ctx, int ix) const
   {
       rect r = ctx.bounds;
       r.top = ctx.bounds.top + _cells[ix].pos;
       r.height(_cells[ix].main_axis_size);
       return r;
   }

   ////////////////////////////////////////////////////////////////////////////
   // Horizontal dynamic_list methods
   ////////////////////////////////////////////////////////////////////////////

   float hdynamic_list::get_main_axis_start(const rect &r)
   {return r.left;}

   float hdynamic_list::get_main_axis_end(const rect &r)
   {return r.right;}

   view_limits hdynamic_list::make_limits(float main_axis_size, cell_composer::limits secondary_axis_limits) const
   {
       return {
          { main_axis_size, secondary_axis_limits.min }
        , { main_axis_size, secondary_axis_limits.max } };
   }

   void hdynamic_list::make_bounds(context &ctx, float main_axis_pos, cell_info &cell)
   {
       ctx.bounds.left = main_axis_pos + cell.pos;
       ctx.bounds.width(cell.main_axis_size);
   }

   rect hdynamic_list::bounds_of(context const& ctx, int ix) const
   {
       rect r = ctx.bounds;
       r.left = ctx.bounds.left + _cells[ix].pos;
       r.width(_cells[ix].main_axis_size);
       return r;
   }

}}


