/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/menu.hpp>
#include <elements/element/composite.hpp>
#include <elements/element/traversal.hpp>
#include <elements/element/port.hpp>
#include <elements/support/theme.hpp>
#include <elements/support/text_utils.hpp>
#include <elements/support/icon_ids.hpp>
#include <algorithm>
#include <utility>
#include <vector>

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

      // A rect moved into the view, as little as it takes.
      rect into_view(rect b, rect view)
      {
         float dx = 0, dy = 0;
         if (b.right > view.right)
            dx = view.right - b.right;
         if (b.left + dx < view.left)
            dx = view.left - b.left;
         if (b.bottom > view.bottom)
            dy = view.bottom - b.bottom;
         if (b.top + dy < view.top)
            dy = view.top - b.top;
         return b.move(dx, dy);
      }

      // Where a submenu goes. From an item in a menu: to its right, or to
      // its left if only that fits. From an item in a bar: below it, or
      // above it if only that fits. Then moved into the view.
      rect submenu_bounds(context const& ctx, point size, bool in_menu)
      {
         auto const view = ctx.view_bounds();
         auto const& item = ctx.bounds;
         auto const w = size.x;
         auto const h = size.y;

         rect b;
         if (in_menu)
         {
            b = {item.right, item.top, item.right + w, item.top + h};
            if (b.right > view.right && item.left - w >= view.left)
               b = b.move_to(item.left - w, item.top);
         }
         else
         {
            b = {item.left, item.bottom, item.left + w, item.bottom + h};
            if (b.bottom > view.bottom && item.top - h >= view.top)
               b = b.move_to(item.left, item.top - h);
         }
         return into_view(b, view);
      }

      // The menu items in the composite that holds the element in ctx,
      // with their bounds.
      using item_list =
         std::vector<std::pair<rect, basic_menu_item_element*>>;

      item_list sibling_items(context const& ctx)
      {
         item_list items;
         auto [c, cctx] = find_composite(ctx);
         if (!c)
            return items;

         for (std::size_t i = 0; i != c->size(); ++i)
         {
            auto e = find_element<basic_menu_item_element*>(&c->at(i));
            if (e)
               items.emplace_back(c->bounds_of(*cctx, i), e);
         }
         return items;
      }

      // The first enabled menu item in e, depth first.
      basic_menu_item_element* first_item(element& e)
      {
         if (auto item = find_element<basic_menu_item_element*>(&e))
            return item->is_enabled()? item : nullptr;

         if (auto c = find_element<composite_base*>(&e))
         {
            for (std::size_t i = 0; i != c->size(); ++i)
            {
               if (auto item = first_item(c->at(i)))
                  return item;
            }
         }
         return nullptr;
      }

      // Unselects every menu item in e: a menu opens with nothing selected
      // from the last time it was open.
      void unselect_items(element& e)
      {
         if (auto item = find_element<basic_menu_item_element*>(&e))
         {
            item->select(false);
            return;
         }

         if (auto c = find_element<composite_base*>(&e))
         {
            for (std::size_t i = 0; i != c->size(); ++i)
               unselect_items(c->at(i));
         }
      }

      basic_menu_item_element* selected_item(context const& ctx)
      {
         for (auto [r, e] : sibling_items(ctx))
         {
            if (e->is_selected())
               return e;
         }
         return nullptr;
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

      bounds = into_view(bounds, view);

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

      // An item with a submenu, in a menu, shows an arrow at its right,
      // in the item text's right margin.
      if (_menu && find_parent<basic_popup_element*>(ctx))
      {
         auto const& theme = get_theme();
         auto const size = theme.icon_font._size;
         rect r = ctx.bounds;
         r.right -= 4;
         r.left = r.right - size;
         auto const c = (ctx.enabled && is_enabled())?
            theme.label_font_color : theme.inactive_font_color;
         draw_icon(ctx.canvas, r, icons::right_dir, size, c);
      }
   }

   void basic_menu_item_element::open_menu(context const& ctx)
   {
      auto& view_ = ctx.view;
      auto* parent = find_parent<basic_popup_menu_element*>(ctx);
      close_sibling_menus(ctx, parent);

      unselect_items(*_menu);
      auto const size = _menu->limits(ctx).min;
      auto const b = submenu_bounds(ctx, size, parent != nullptr);
      context mctx{view_, ctx.canvas, _menu.get(), b};
      _menu->bounds(b);
      _menu->layout(mctx);

      _menu->_menu_item = this;
      _menu->_parent = parent;
      if (parent)
         parent->_submenu = _menu.get();
      follow_pointer(ctx, parent);

      select(true);
      view_.refresh(ctx);
      _menu->open(view_);
   }

   // One menu open at a time among siblings: in a menu, its submenu; in a
   // bar, the menu of any other item. Only this item stays selected.
   void basic_menu_item_element::close_sibling_menus(
      context const& ctx, basic_popup_menu_element* parent)
   {
      if (parent && parent->_submenu && parent->_submenu != _menu.get())
         parent->_submenu->close(ctx.view);

      for (auto [r, e] : sibling_items(ctx))
      {
         if (e == this)
            continue;
         if (!parent && e->_menu && e->_menu->is_open(ctx.view))
            e->_menu->close(ctx.view);
         e->select(false);
      }
   }

   // While it is open, the menu sees the pointer anywhere in the view.
   // Over another item beside this one, that item's menu takes its place;
   // over a plain item in the same menu, the menu closes.
   void basic_menu_item_element::follow_pointer(
      context const& ctx, basic_popup_menu_element* parent)
   {
      auto& view_ = ctx.view;
      _menu->on_cursor =
         [this, &view_, parent, items = sibling_items(ctx)]
         (point p, cursor_tracking status)
         {
            if (status == cursor_tracking::leaving)
               return;

            for (auto [r, e] : items)
            {
               if (e == this || !r.includes(p))
                  continue;

               if (!e->_menu)
               {
                  if (parent)
                     _menu->close(view_);
               }
               else
               {
                  bool const opening = parent?
                     parent->_submenu == e->_menu.get() : e->is_selected();
                  if (!opening && e->is_enabled())
                     view_.in_context_do(*e,
                        [e](context const& ectx) { e->open_menu(ectx); });
               }
               return;
            }
         };
   }

   // Enter on the selected item: open its submenu, or pick it, closing
   // every menu it was reached through.
   bool basic_menu_item_element::pick(context const& ctx)
   {
      auto* selected = selected_item(ctx);
      if (selected && selected->_menu)
      {
         selected->open_by_key(ctx.view);
         return true;
      }

      if (selected)
      {
         if (selected->on_click)
            selected->on_click();
         selected->select(false);
      }
      close_all(ctx);
      return true;
   }

   // Escape: close the menu the item is in. A submenu's item stays
   // selected in the menu it was opened from, for the keys that follow.
   bool basic_menu_item_element::back_out(context const& ctx)
   {
      if (auto* selected = selected_item(ctx))
         selected->select(false);

      auto* menu = find_parent<basic_popup_menu_element*>(ctx);
      if (menu && menu->_parent)
      {
         close_submenu(ctx.view, menu);
      }
      else if (auto* popup = find_parent<basic_popup_element*>(ctx))
      {
         popup->close(ctx.view);
      }
      return true;
   }

   // Closes a submenu from the keyboard. The item that opened it stays
   // selected.
   void basic_menu_item_element::close_submenu(view& view_, popup_menu* menu)
   {
      auto* owner = menu->_menu_item;
      menu->close(view_);
      if (owner)
      {
         owner->select(true);
         view_.refresh();
      }
   }

   // Closes every menu the item in ctx was reached through.
   void basic_menu_item_element::close_all(context const& ctx)
   {
      if (auto* menu = find_parent<basic_popup_menu_element*>(ctx))
      {
         while (menu->_parent)
            menu = menu->_parent;
         menu->close(ctx.view);
      }
      else if (auto* popup = find_parent<basic_popup_element*>(ctx))
      {
         popup->close(ctx.view);
      }
   }

   // Right opens the selected item's submenu, and Left closes the submenu
   // this item is in. In a menu opened from a bar, either moves on to the
   // bar's next menu.
   bool basic_menu_item_element::move_across(context const& ctx, bool right)
   {
      auto* menu = find_parent<basic_popup_menu_element*>(ctx);
      auto* selected = selected_item(ctx);

      if (right && selected && selected->_menu)
      {
         selected->open_by_key(ctx.view);
         return true;
      }
      if (!right && menu && menu->_parent)
      {
         close_submenu(ctx.view, menu);
         return true;
      }
      if (menu && !menu->_parent && menu->_menu_item)
      {
         menu->_menu_item->open_next_menu(ctx.view, right);
         return true;
      }
      return false;
   }

   // Opens the menu of the next item beside this one that has a menu,
   // wrapping around: Right and Left across a menu bar.
   void basic_menu_item_element::open_next_menu(view& view_, bool right)
   {
      view_.in_context_do(*this,
         [this, right](context const& ctx)
         {
            std::vector<basic_menu_item_element*> menus;
            for (auto [r, e] : sibling_items(ctx))
            {
               if (e->_menu && e->is_enabled())
                  menus.push_back(e);
            }

            auto i = std::find(menus.begin(), menus.end(), this);
            if (i == menus.end() || menus.size() < 2)
               return;

            auto const n = std::ptrdiff_t(menus.size());
            auto const at = ((i - menus.begin()) + (right? 1 : -1) + n) % n;
            // As a click on its title opens it: nothing selected.
            auto* next = menus[at];
            ctx.view.in_context_do(*next,
               [next](context const& nctx) { next->open_menu(nctx); });
         });
   }

   // A submenu opened from the keyboard starts on its first enabled item,
   // for the keys that follow.
   void basic_menu_item_element::open_by_key(view& view_)
   {
      view_.in_context_do(*this,
         [this](context const& ctx) { open_menu(ctx); });

      if (auto item = first_item(*_menu))
      {
         item->select(true);
         view_.refresh();
      }
   }

   element* basic_menu_item_element::hit_test(context const& ctx, point p, bool /*leaf*/, bool /*control*/)
   {
      if (ctx.enabled && is_enabled() && ctx.bounds.includes(p))
         return this;
      return nullptr;
   }

   bool basic_menu_item_element::click(context const& ctx, mouse_button btn)
   {
      // An item with a submenu opens it on the press.
      if (_menu)
      {
         if (!ctx.enabled || !is_enabled() || !ctx.bounds.includes(btn.pos))
            return false;
         if (btn.down && !_menu->is_open(ctx.view))
            open_menu(ctx);
         return true;
      }

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
         close_all(ctx);
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
               return pick(ctx);

            case key_code::escape:
               return back_out(ctx);

            case key_code::right:
            case key_code::left:
               return move_across(ctx, k.key == key_code::right);

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
      // A submenu's items, while it is closed; while it is open, they have
      // the keys themselves.
      if (_menu)
      {
         context mctx{ctx.view, ctx.canvas, _menu.get(), _menu->bounds()};
         if (is_hotkey_test(k))
            return _menu->hotkey(mctx, k);
         if (!ctx.enabled || !is_enabled() || _menu->is_open(ctx.view))
            return false;
         return _menu->hotkey(mctx, k);
      }

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

      // In a menu, pointing at an item with a submenu opens it.
      if (_menu && hit && status != cursor_tracking::leaving
         && !_menu->is_open(ctx.view))
      {
         auto* menu = find_parent<basic_popup_menu_element*>(ctx);
         if (menu && menu->_submenu != _menu.get())
         {
            open_menu(ctx);
            return true;
         }
      }

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
