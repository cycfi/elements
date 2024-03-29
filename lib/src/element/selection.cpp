/*=================================================================================================
   Copyright (c) 2016-2024 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=================================================================================================*/
#include <elements/element/selection.hpp>
#include <elements/element/composite.hpp>
#include <elements/element/traversal.hpp>
#include <elements/element/port.hpp>
#include <elements/view.hpp>

namespace cycfi::elements
{
   namespace detail
   {
      std::vector<std::size_t> get_selection(composite_base const& c)
      {
         std::vector<std::size_t> indices;
         for (std::size_t i = 0; i != c.size(); ++i)
         {
            if (auto e = find_element<selectable*>(&c.at(i)))
            {
               if (e->is_selected())
                  indices.push_back(i);
            }
         }
         return indices;
      }

      void select_none(composite_base const& c)
      {
         for (std::size_t i = 0; i != c.size(); ++i)
         {
            if (auto e = find_element<selectable*>(&c.at(i)))
               e->select(false);
         }
      }

      void set_selection(composite_base& c, std::vector<std::size_t> const selection)
      {
         select_none(c);
         for (std::size_t i : selection)
         {
            if (i > c.size())
               continue; // Ignore out of bounds indices
            if (auto e = find_element<selectable*>(&c.at(i)))
               e->select(true);
         }
      }

      void set_selection(composite_base& c, std::size_t from_, std::size_t to_)
      {
         select_none(c);
         auto from = std::min(from_, to_);
         auto to = std::max(from_, to_);
         for (std::size_t i = from; i != to; ++i)
         {
            if (i > c.size())
               break;
            if (auto e = find_element<selectable*>(&c.at(i)))
               e->select(true);
         }
      }

      void select_all(composite_base const& c)
      {
         for (std::size_t i = 0; i != c.size(); ++i)
         {
            if (auto e = find_element<selectable*>(&c.at(i)))
               e->select(true);
         }
      }

      void select(
         composite_base const& c
       , selectable* e
       , std::size_t index
       , int& _select_start
       , int& _select_end
      )
   {
         select_none(c);
         e->select(true);
         _select_start = _select_end = index;
      }

      bool select(
         composite_base const& c
       , composite_base::hit_info const& hit
       , int& _select_start
       , int& _select_end
      )
      {
         if (auto e = find_element<selectable*>(hit.element_ptr))
         {
            select(c, e, hit.index, _select_start, _select_end);
            return true;
         }
         return false;
      }

      std::size_t count_selected(composite_base const& c)
      {
         std::size_t n = 0;
         for (std::size_t i = 0; i != c.size(); ++i)
         {
            if (auto e = find_element<selectable*>(&c.at(i)))
            {
               if (e->is_selected())
                  ++n;
            }
         }
         return n;
      }

      bool action_select(
         composite_base const& c
       , composite_base::hit_info const& hit
       , int& _select_start
       , int& _select_end
       , bool _multi_select
      )
      {
         if (auto e = find_element<selectable*>(hit.element_ptr))
         {
            if (!_multi_select)
            {
               if (auto pe = find_element<selectable*>(&c.at(_select_start)); pe && e != pe)
                  pe->select(false);
            }
            e->select(!e->is_selected());
            if (e->is_selected())
               _select_start = _select_end = e->is_selected()? hit.index : -1;
            if (count_selected(c) == 0)
               _select_start = _select_end = -1;
            return true;
         }
         return false;
      }

      void shift_select(
         composite_base const& c
       , std::size_t index
       , int _select_start
       , int& _select_end
      )
      {
         auto start = std::max(_select_start, 0);
         auto end = std::max(_select_end, 0);
         auto from = std::min(start, end);
         auto to = std::max(start, end);

         for (int i = from; i <= to; ++i)
         {
            if (auto e = find_element<selectable*>(&c.at(i)))
               e->select(false);
         }

         from = std::min<std::size_t>(start, index);
         to = std::max<std::size_t>(start, index);

         for (int i = from; i <= to; ++i)
         {
            if (auto e = find_element<selectable*>(&c.at(i)))
               e->select(true);
         }
         _select_end = index;
      }

      bool shift_select(
         composite_base const& c
       , composite_base::hit_info const& hit
       , int _select_start
       , int& _select_end
      )
      {
         if (auto e = find_element<selectable*>(hit.element_ptr))
         {
            shift_select(c, hit.index, _select_start, _select_end);
            return true;
         }
         return false;
      }

      void select_first(
         composite_base const& c
       , int& _select_start
       , int& _select_end
      )
      {
         if (c.size())
         {
            if (auto e = find_element<selectable*>(&c.at(0)))
               select(c, e, 0, _select_start, _select_end);
         }
      }

      void select_last(
         composite_base const& c
       , int& _select_start
       , int& _select_end
      )
      {
         if (c.size())
         {
            if (auto e = find_element<selectable*>(&c.at(c.size()-1)))
               select(c, e, c.size()-1, _select_start, _select_end);
         }
      }

      void select_next(
         composite_base const& c
       , int& _select_start
       , int& _select_end
       , bool shift
      )
      {
         if ((_select_end+1) < int(c.size()))
         {
            if (shift)
               shift_select(c, _select_end+1, _select_start, _select_end);
            else if (auto e = find_element<selectable*>(&c.at(_select_end+1)))
               select(c, e, _select_end+1, _select_start, _select_end);
         }
      }

      void select_prev(
         composite_base const& c
       , int& _select_start
       , int& _select_end
       , bool shift
      )
      {
         if ((_select_end-1) >= 0)
         {
            if (shift)
               shift_select(c, _select_end-1, _select_start, _select_end);
            else if (auto e = find_element<selectable*>(&c.at(_select_end-1)))
               select(c, e, _select_end-1, _select_start, _select_end);
         }
      }
   }

   using namespace detail;

   bool selection_list_element::click(context const& ctx, mouse_button btn)
   {
      bool r = base_type::click(ctx, btn);
      if (r)
         return true;

      if (auto c = find_subject<composite_base*>(this))
      {
         in_context_do(ctx, *c,
            [&](context const& cctx)
            {
               auto hit = c->hit_element(cctx, btn.pos, false);
               if (hit.element_ptr)
               {
                  if (btn.modifiers & mod_action)
                  {
                     // Process action-select
                     if (btn.down)
                        r = action_select(*c, hit, _select_start, _select_end, _multi_select);
                  }
                  else if (_multi_select && (btn.modifiers & mod_shift))
                  {
                     // Process shift-select
                     if (btn.down)
                        r = shift_select(*c, hit, _select_start, _select_end);
                  }
                  else
                  {
                     // Process select
                     if (btn.down)
                        select(*c, hit, _select_start, _select_end);
                     r = true;
                  }
               }
               if (r && _select_start >= 0)
               {
                  ctx.view.refresh(ctx);
                  on_select(_select_start, _select_end);
               }
            }
         );
      }
      return r;
   }

   bool selection_list_element::key(context const& ctx, key_info k)
   {
      bool r = base_type::key(ctx, k);
      if (k.action == key_action::press || k.action == key_action::repeat)
      {
         switch (k.key)
         {
            case key_code::a:
               if (k.modifiers & mod_action)
               {
                  if (auto c = find_subject<composite_base*>(this))
                  {
                     detail::select_all(*c);
                     ctx.view.refresh(ctx);
                     return true;
                  }
               }
               break;

            case key_code::up:
            {
               if (auto c = find_subject<composite_base*>(this))
               {
                  if (_select_start == -1)
                  {
                     select_last(*c, _select_start, _select_end);
                  }
                  else
                  {
                     auto extend = _multi_select && (k.modifiers & mod_shift);
                     select_prev(*c, _select_start, _select_end, extend);
                  }
                  if (_select_end != -1)
                  {
                     in_context_do(ctx, *c,
                        [&](context const& cctx)
                        {
                           scrollable::find(ctx).scroll_into_view(c->bounds_of(cctx, _select_end));
                           ctx.view.refresh(ctx);
                        }
                     );
                     r = true;
                  }
               }
               break;
            }

            case key_code::down:
            {
               if (auto c = find_subject<composite_base*>(this))
               {
                  if (_select_start == -1)
                  {
                     select_first(*c, _select_start, _select_end);
                  }
                  else
                  {
                     auto extend = _multi_select && (k.modifiers & mod_shift);
                     select_next(*c, _select_start, _select_end, extend);
                  }
                  if (_select_end != -1)
                  {
                     in_context_do(ctx, *c,
                        [&](context const& cctx)
                        {
                           scrollable::find(ctx).scroll_into_view(c->bounds_of(cctx, _select_end));
                           ctx.view.refresh(ctx);
                        }
                     );
                     r = true;
                  }
               }
               break;
            }
            default:
               break;
         }
      }
      return r;
   }

   selection_list_element::indices_type
   selection_list_element::get_selection() const
   {
      if (auto c = find_subject<composite_base const*>(this))
         return detail::get_selection(*c);
      return {};
   }

   void selection_list_element::set_selection(indices_type const& selection)
   {
      if (auto c = find_subject<composite_base*>(this))
         return detail::set_selection(*c, selection);
   }

   void selection_list_element::update_selection(int start, int end)
   {
      _select_start = start;
      _select_end = end;
   }

   int selection_list_element::get_select_start() const
   {
      return _select_start;
   }

   int selection_list_element::get_select_end() const
   {
      return _select_end;
   }

   void selection_list_element::select_all()
   {
      if (_multi_select)
      {
         if (auto c = find_subject<composite_base*>(this))
         {
            detail::select_all(*c);
            _select_start = 0;
            _select_end = c->size()-1;
            on_select(_select_start, _select_end);
         }
      }
   }

   void selection_list_element::select_none()
   {
      if (auto c = find_subject<composite_base*>(this))
      {
         detail::select_none(*c);
         _select_start = _select_end = -1;
         on_select(_select_start, _select_end);
      }
   }
}

