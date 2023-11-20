/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/drag_and_drop.hpp>
#include <elements/element/traversal.hpp>
#include <elements/element/composite.hpp>
#include <elements/element/port.hpp>
#include <elements/support/context.hpp>
#include <elements/support/theme.hpp>
#include <elements/view.hpp>

namespace cycfi { namespace elements
{
   drop_base::drop_base(std::initializer_list<std::string> mime_types_)
    : _mime_types{mime_types_}
   {}

   bool drop_base::wants_control() const
   {
      return true;
   }

   void drop_base::track_drop(context const& ctx, drop_info const& info, cursor_tracking status)
   {
      // Return early if none of registered mime types is in the `drop_info`
      if (!std::any_of(_mime_types.begin(), _mime_types.end(),
         [&](auto const& item)
         {
            return info.data.find(item) != info.data.end();
         }))
         return;

      auto new_is_tracking = status != cursor_tracking::leaving;
      if (new_is_tracking != _is_tracking)
      {
         _is_tracking = new_is_tracking;
         ctx.view.refresh(ctx.bounds);
      }
   }

   bool drop_base::drop(context const& /*ctx*/, drop_info const& /*info*/)
   {
      _is_tracking = false;
      return false;
   }

   drop_box_base::drop_box_base(std::initializer_list<std::string> mime_types_)
    : base_type{mime_types_}
   {}

   void drop_box_base::draw(context const& ctx)
   {
      proxy_base::draw(ctx);
      if (is_tracking())
      {
         auto& cnv = ctx.canvas;
         auto& bounds = ctx.bounds;
         cnv.stroke_style(get_theme().indicator_hilite_color.opacity(0.5));
         cnv.line_width(2.0);
         cnv.add_rect(bounds);
         cnv.stroke();
      }
   }

   bool drop_box_base::drop(context const& ctx, drop_info const& info)
   {
      base_type::drop(ctx, info);
      bool r = on_drop(info);
      ctx.view.refresh(ctx.bounds);
      return r;
   }

   drop_inserter_base::drop_inserter_base(std::initializer_list<std::string> mime_types_)
    : base_type{mime_types_}
   {}

   void drop_inserter_base::draw(context const& ctx)
   {
      proxy_base::draw(ctx);
      if (is_tracking())
      {
         if (auto c = find_subject<composite_base*>(this))
         {
            in_context_do(ctx, *c, [&](context const& cctx)
            {
               point cursor_pos = ctx.cursor_pos();
               auto hit_info = c->hit_element(cctx, cursor_pos, false);
               if (hit_info.element_ptr)
               {
                  rect const& bounds = hit_info.bounds;
                  bool before = cursor_pos.y < (bounds.top + (bounds.height()/2));
                  float pos = before? bounds.top : bounds.bottom;
                  _insertion_pos = before? hit_info.index : hit_info.index+1;

                  auto &cnv = cctx.canvas;
                  cnv.stroke_style(get_theme().indicator_hilite_color.opacity(0.5));
                  cnv.line_width(2.0);
                  cnv.move_to({bounds.left, pos});
                  cnv.line_to({bounds.right, pos});
                  cnv.stroke();
               }
            });
         }
      }
   }

   void drop_inserter_base::track_drop(context const& ctx, drop_info const& info, cursor_tracking status)
   {
      base_type::track_drop(ctx, info, status);
      if (is_tracking())
      {
         static constexpr auto offset = 20;
         rect r = {info.where.x-offset, info.where.y-offset, info.where.x+offset, info.where.y+offset};
         scrollable::find(ctx).scroll_into_view(r);
         ctx.view.refresh(ctx.bounds);
      }
   }

   bool drop_inserter_base::drop(context const& ctx, drop_info const& info)
   {
      base_type::drop(ctx, info);
      if (_insertion_pos >= 0)
      {
         bool r = on_drop(info, _insertion_pos);
         ctx.view.refresh(ctx.bounds);
         _insertion_pos = -1;
         return r;
      }
      return false;
   }

   void draggable_element::draw(context const& ctx)
   {
      if (is_selected() && is_enabled())
      {
         auto& canvas_ = ctx.canvas;

         canvas_.begin_path();
         canvas_.add_round_rect(ctx.bounds, 2);
         canvas_.fill_style(get_theme().indicator_color.opacity(0.6));
         canvas_.fill();
      }
      if (is_enabled())
      {
         proxy_base::draw(ctx);
      }
      else
      {
         auto r = override_theme(
            &theme::label_font_color
          , get_theme().inactive_font_color
         );
         proxy_base::draw(ctx);
      }
   }

   element* draggable_element::hit_test(context const& ctx, point p, bool /*leaf*/)
   {
      if (is_enabled() && ctx.bounds.includes(p))
         return this;
      return nullptr;
   }

   bool draggable_element::click(context const& ctx, mouse_button btn)
   {
      bool result = false;
      if (is_enabled() && ctx.bounds.includes(btn.pos))
      {
         if (!btn.down)
         {
            if ((btn.modifiers & (mod_shift | mod_action)) == 0)
            {
               auto [c, cctx] = find_composite(ctx);
               if (c)
               {
                  for (std::size_t i = 0; i != c->size(); ++i)
                  {
                     if (auto e = find_element<draggable_element*>(&c->at(i)))
                        e->select(false);
                  }
                  cctx->view.refresh(*cctx);
               }
            }
            select(true);
            if (btn.modifiers & mod_shift)
            {
               auto [c, cctx] = find_composite(ctx);
               if (c)
               {
                  // Find first selected
                  int first = -1;
                  for (std::size_t i = 0; i != c->size(); ++i)
                  {
                     if (auto e = find_element<draggable_element*>(&c->at(i)))
                     {
                        if (e->is_selected())
                        {
                           first = i;
                           break;
                        }
                     }
                  }
                  // Find last selected
                  if (first > 0)
                  {
                     int last = -1;
                     for (int i = c->size()-1; i >= 0; --i)
                     {
                        if (auto e = find_element<draggable_element*>(&c->at(i)))
                        {
                           if (e->is_selected())
                           {
                              last = i;
                              break;
                           }
                        }
                     }
                     if (last > 0)
                     {
                        // Select all from first to last
                        for (int i = first; i != last+1; ++i)
                        {
                           if (auto e = find_element<draggable_element*>(&c->at(i)))
                              e->select(true);
                        }
                     }
                  }
                  cctx->view.refresh(*cctx);
               }
            }
            ctx.view.refresh(ctx);
         }
         result = true;
      }
      auto proxy_result = proxy_base::click(ctx, btn);
      return result || proxy_result;
   }

   bool draggable_element::key(context const& ctx, key_info k)
   {
      if (k.action == key_action::press || k.action == key_action::repeat)
      {
         // auto&& equal =
         //    [](auto k, auto shortcut)
         //    {
         //       int mask = 0xF;
         //       switch (shortcut.key)
         //       {
         //          case key_code::minus:
         //          case key_code::equal:
         //             mask &= ~mod_shift;
         //             break;
         //          default:
         //             break;
         //       }
         //       return (k.key == shortcut.key) &&
         //          ((k.modifiers & mask) == (shortcut.modifiers & mask))
         //          ;
         //    };

         switch (k.key)
         {
            case key_code::up:
            case key_code::down:
            {
               // auto [c, cctx] = find_composite(ctx);
               // if (c)
               // {
               //    auto find_selected = [](auto const& c)
               //       -> std::pair<draggable_element*, int>
               //    {
               //       for (std::size_t i = 0; i != c->size(); ++i)
               //       {
               //          auto e = find_element<draggable_element*>(&c->at(i));
               //          if (e && e->is_selected())
               //             return std::make_pair(e, i);
               //       }
               //       return std::make_pair(nullptr, -1);
               //    };

               //    auto select_next = [&](auto const& c, auto& selected, auto& selected_index)
               //    {
               //       bool const down = k.key == key_code::down;
               //       auto const last = static_cast<int>(c->size()) - 1;

               //       int start = selected?
               //          (down? selected_index+1 : selected_index-1) :
               //          (down? 0 : last)
               //       ;
               //       int until = down? int(c->size()) : -1;
               //       for (int i = start; i != until; i += down? +1 : -1)
               //       {
               //          auto p = find_element<draggable_element*>(&c->at(i));
               //          if (p && p->is_enabled())
               //          {
               //             selected = p;
               //             selected_index = i;
               //             break;
               //          }
               //       }
               //       if (selected)
               //          selected->select(true);
               //    };

               //    auto unselect_rest = [](auto const& c, auto selected)
               //    {
               //       for (std::size_t i = 0; i != c->size(); ++i)
               //       {
               //          auto e = find_element<draggable_element*>(&c->at(i));
               //          if (e && e != selected && e->is_enabled())
               //             e->select(false);
               //       }
               //    };

               //    auto [selected, selected_index] = find_selected(c);
               //    select_next(c, selected, selected_index);
               //    unselect_rest(c, selected);

               //    scrollable::find(ctx).scroll_into_view(c->bounds_of(*cctx, selected_index));
               //    cctx->view.refresh(*cctx);
               // }
               return true;
            }
            break;

            default:
               break;
               // if (is_enabled() && equal(k, shortcut))
               // {
               //    if (on_click)
               //       on_click();
               //    ctx.notify(ctx, "key", this);
               //    return true;
               // }
         }
      }
      return false;
   }

   bool draggable_element::cursor(context const& ctx, point p, cursor_tracking status)
   {
      if (!is_enabled())
         return false;

      bool hit = ctx.bounds.includes(p);
      if (status == cursor_tracking::leaving || (hit && !is_selected()))
      {
         auto [c, cctx] = find_composite(ctx);
         if (c)
         {
            // for (std::size_t i = 0; i != c->size(); ++i)
            // {
            //    if (auto e = find_element<selectable*>(&c->at(i)))
            //       e->select(false);
            // }
            // cctx->view.refresh(*cctx);
         }
         // select(hit);
      }
      proxy_base::cursor(ctx, p, status);
      return hit;
   }

   bool draggable_element::wants_control() const
   {
      return true;
   }
}}

