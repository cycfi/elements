/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/menu.hpp>
#include <elements/element/composite.hpp>
#include <elements/element/traversal.hpp>
#include <elements/element/port.hpp>
#include <elements/support/theme.hpp>

namespace cycfi::elements
{
   namespace
   {
      bool matches(key_info k, shortcut_key const& shortcut)
      {
         int mask = 0xF;
         switch (shortcut.key)
         {
            case key_code::minus:
            case key_code::equal:
               mask &= ~mod_shift;
               break;
            default:
               break;
         }
         return shortcut.key != key_code::unknown
            && k.key == shortcut.key
            && (k.modifiers & mask) == (shortcut.modifiers & mask)
            ;
      }
   }

   void basic_button_menu::layout_menu(context const& ctx)
   {
      auto pu_limits = _popup->limits(ctx);
      rect  bounds;

      switch (_position)
      {
         case menu_position::top_left:
            bounds = rect{
               ctx.bounds.right - (3 + pu_limits.min.x),
               ctx.bounds.top - pu_limits.min.y,
               ctx.bounds.right - 3,
               ctx.bounds.top
            };
            break;

         case menu_position::top_right:
            bounds = rect{
               ctx.bounds.left + 3,
               ctx.bounds.top - pu_limits.min.y,
               ctx.bounds.left + 3 + pu_limits.min.x,
               ctx.bounds.top
            };
            break;

         case menu_position::bottom_left:
            bounds = rect{
               ctx.bounds.right - (3 + pu_limits.min.x),
               ctx.bounds.bottom,
               ctx.bounds.right - 3,
               ctx.bounds.bottom + pu_limits.min.y
            };
            break;

         case menu_position::bottom_right:
            bounds = rect{
               ctx.bounds.left + 3,
               ctx.bounds.bottom,
               ctx.bounds.left + 3 + pu_limits.min.x,
               ctx.bounds.bottom + pu_limits.min.y
            };
            break;
      }

      // On the side asked for, or the other if it does not fit there and
      // does on the other; then moved into the view.
      auto const view = ctx.view_bounds();
      auto const h = bounds.height();
      bool const below = bounds.top >= ctx.bounds.bottom;
      if (below && bounds.bottom > view.bottom
         && ctx.bounds.top - h >= view.top)
         bounds = bounds.move_to(bounds.left, ctx.bounds.top - h);
      else if (!below && bounds.top < view.top
         && ctx.bounds.bottom + h <= view.bottom)
         bounds = bounds.move_to(bounds.left, ctx.bounds.bottom);

      float dx = 0, dy = 0;
      if (bounds.right > view.right)
         dx = view.right - bounds.right;
      if (bounds.left + dx < view.left)
         dx = view.left - bounds.left;
      if (bounds.bottom > view.bottom)
         dy = view.bottom - bounds.bottom;
      if (bounds.top + dy < view.top)
         dy = view.top - bounds.top;
      bounds = bounds.move(dx, dy);

      context new_ctx{ctx.view, ctx.canvas, _popup.get(), bounds};
      _popup->bounds(bounds);
      _popup->layout(new_ctx);
   }

   bool basic_button_menu::click(context const& ctx, mouse_button btn)
   {
      if (!ctx.enabled || !is_enabled())
         return false;

      if (btn.down)
      {
         if (set_value(true))
         {
            if (on_open_menu)
               on_open_menu(*this);

            if (_popup)
            {
               layout_menu(ctx);
               _popup->menu_button(this);
               _popup->open(ctx.view);
            }
         }
      }
      else
      {
         if (_popup && (!value() || !hit_test(ctx, btn.pos, false, true)))
         {
            // simulate a menu click:
            btn.down = true;
            rect  bounds = _popup->bounds();
            context new_ctx{ctx.view, ctx.canvas, _popup.get(), bounds};
            _popup->click(new_ctx, btn);
         }
      }
      return true;
   }

   void basic_button_menu::drag(context const& ctx, mouse_button btn)
   {
      if (!_popup)
         return;

      rect  bounds = _popup->bounds();
      if (bounds.includes(btn.pos))
      {
         context new_ctx{ctx.view, ctx.canvas, _popup.get(), bounds};
         if (btn.down)
         {
            // simulate a menu hover (cursor):
            _popup->cursor(new_ctx, btn.pos, cursor_tracking::hovering);
         }
         else
         {
            // simulate a menu click:
            btn.down = true;
            _popup->click(new_ctx, btn);
         }
      }
   }

   bool basic_button_menu::hotkey(context const& ctx, key_info k)
   {
      // The menu's items, while it is closed; while it is open, they have
      // the keys themselves.
      if (!_popup)
         return false;
      context pctx{ctx.view, ctx.canvas, _popup.get(), _popup->bounds()};
      if (is_hotkey_test(k))
         return _popup->hotkey(pctx, k);
      if (!ctx.enabled || !is_enabled() || _popup->is_open(ctx.view))
         return false;
      return _popup->hotkey(pctx, k);
   }

   void basic_menu_item_element::draw(context const& ctx)
   {
      if (ctx.enabled && is_selected() && is_enabled())
      {
         auto& canvas_ = ctx.canvas;

         canvas_.begin_path();
         canvas_.add_round_rect(ctx.bounds, 2);
         canvas_.fill_style(get_theme().indicator_color.opacity(0.6));
         canvas_.fill();
      }
      proxy_base::draw(ctx);
   }

   element* basic_menu_item_element::hit_test(context const& ctx, point p, bool /*leaf*/, bool /*control*/)
   {
      if (ctx.enabled && is_enabled() && ctx.bounds.includes(p))
         return this;
      return nullptr;
   }

   bool basic_menu_item_element::click(context const& ctx, mouse_button btn)
   {
      // An item acts on the press alone. Its menu closes at the view's next
      // poll, so the release may land on the item too. A press, drag and
      // release from a button_menu reaches it as a press.
      if (!btn.down)
         return ctx.enabled && is_enabled() && ctx.bounds.includes(btn.pos);

      bool result = false;
      if (ctx.enabled && is_enabled() && ctx.bounds.includes(btn.pos))
      {
         if (on_click)
            on_click();
         select(false);

         if (auto _popup = find_parent<basic_popup_element*>(ctx))
            _popup->close(ctx.view);

         result = true;
      }
      auto proxy_result = proxy_base::click(ctx, btn);
      return result || proxy_result;
   }

   bool basic_menu_item_element::key(context const& ctx, key_info k)
   {
      if (k.action == key_action::press || k.action == key_action::repeat)
      {
         switch (k.key)
         {
            case key_code::enter:
            case key_code::escape:
            {
               // Close the popup
               if (auto _popup = find_parent<basic_popup_element*>(ctx))
                  _popup->close(ctx.view);

               auto [c, cctx] = find_composite(ctx);
               if (c)
               {
                  for (std::size_t i = 0; i != c->size(); ++i)
                  {
                     auto e = find_element<basic_menu_item_element*>(&c->at(i));
                     if (e && e->is_selected())
                     {
                        if (k.key == key_code::enter && e->on_click)
                           e->on_click();
                        e->select(false);
                        break;
                     }
                  }
               }
               return true;
            }
            break;

            case key_code::up:
            case key_code::down:
            {
               auto [c, cctx] = find_composite(ctx);
               if (c)
               {
                  auto find_selected = [](auto const& c)
                     -> std::pair<basic_menu_item_element*, int>
                  {
                     for (std::size_t i = 0; i != c->size(); ++i)
                     {
                        auto e = find_element<basic_menu_item_element*>(&c->at(i));
                        if (e && e->is_selected())
                           return std::make_pair(e, i);
                     }
                     return std::make_pair(nullptr, -1);
                  };

                  auto select_next = [&](auto const& c, auto& selected, auto& selected_index)
                  {
                     bool const down = k.key == key_code::down;
                     auto const last = static_cast<int>(c->size()) - 1;

                     int start = selected?
                        (down? selected_index+1 : selected_index-1) :
                        (down? 0 : last)
                     ;
                     int until = down? int(c->size()) : -1;
                     for (int i = start; i != until; i += down? +1 : -1)
                     {
                        auto p = find_element<basic_menu_item_element*>(&c->at(i));
                        if (p && p->is_enabled())
                        {
                           selected = p;
                           selected_index = i;
                           break;
                        }
                     }
                     if (selected)
                        selected->select(true);
                  };

                  auto unselect_rest = [](auto const& c, auto selected)
                  {
                     for (std::size_t i = 0; i != c->size(); ++i)
                     {
                        auto e = find_element<basic_menu_item_element*>(&c->at(i));
                        if (e && e != selected && e->is_enabled())
                           e->select(false);
                     }
                  };

                  auto [selected, selected_index] = find_selected(c);
                  select_next(c, selected, selected_index);
                  unselect_rest(c, selected);

                  scrollable::find(ctx).scroll_into_view(c->bounds_of(*cctx, selected_index));
                  cctx->view.refresh(*cctx);
               }
               return true;
            }
            break;

            default:
            {
               auto [c, cctx] = find_composite(ctx);
               if (c)
               {
                  for (std::size_t i = 0; i != c->size(); ++i)
                  {
                     auto e = find_element<basic_menu_item_element*>(&c->at(i));
                     if (e && e->is_enabled() && matches(k, e->shortcut))
                     {
                        if (e->on_click)
                           e->on_click();

                        // Close the popup
                        if (auto _popup = find_parent<basic_popup_element*>(ctx))
                           _popup->close(ctx.view);

                        for (std::size_t i = 0; i != c->size(); ++i)
                        {
                           auto e = find_element<basic_menu_item_element*>(&c->at(i));
                           if (e && e->is_selected())
                           {
                              e->select(false);
                              break;
                           }
                        }
                        return true;
                     }
                  }
               }
            }
         }
      }
      return false;
   }

   bool basic_menu_item_element::hotkey(context const& ctx, key_info k)
   {
      if (is_hotkey_test(k))
         return shortcut.key != key_code::unknown;
      if (!ctx.enabled || !is_enabled() || !matches(k, shortcut))
         return false;
      if (on_click)
         on_click();
      return true;
   }

   bool basic_menu_item_element::cursor(context const& ctx, point p, cursor_tracking status)
   {
      if (!ctx.enabled || !is_enabled())
         return false;

      bool hit = ctx.bounds.includes(p);
      if (status == cursor_tracking::leaving || (hit && !is_selected()))
      {
         auto [c, cctx] = find_composite(ctx);
         if (c)
         {
            for (std::size_t i = 0; i != c->size(); ++i)
            {
               if (auto e = find_element<selectable*>(&c->at(i)))
                  e->select(false);
            }
            cctx->view.refresh(*cctx);
         }
         select(hit);
      }
      proxy_base::cursor(ctx, p, status);
      return hit;
   }

   bool basic_menu_item_element::wants_control() const
   {
      return true;
   }

   std::string basic_button_menu::class_name() const
   {
      return "button_menu";
   }

   std::string basic_menu_item_element::class_name() const
   {
      return "menu_item";
   }
}
