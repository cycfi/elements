/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/menu.hpp>
#include <elements/element/composite.hpp>
#include <elements/element/traversal.hpp>
#include <elements/element/port.hpp>
#include <elements/element/traversal.hpp>

namespace cycfi { namespace elements
{
   void basic_menu::layout_menu(context const& ctx)
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

      context new_ctx{ ctx.view, ctx.canvas, _popup.get(), bounds };
      _popup->bounds(bounds);
      _popup->layout(new_ctx);
   }

   bool basic_menu::click(context const& ctx, mouse_button btn)
   {
      if (btn.down)
      {
         if (state(true))
         {
            if (on_open_menu)
               on_open_menu(*this);

            if (_popup)
            {
               layout_menu(ctx);

               _popup->on_click =
                  [this, &view_ = ctx.view]()
                  {
                     _popup->close(view_);
                     this->value(0);
                     view_.refresh();
                  };

               _popup->open(ctx.view);
               ctx.view.refresh();
            }
         }
      }
      else
      {
         if (_popup && (!value() || !hit_test(ctx, btn.pos)))
         {
            // simulate a menu click:
            btn.down = true;
            rect  bounds = _popup->bounds();
            context new_ctx{ ctx.view, ctx.canvas, _popup.get(), bounds };
            _popup->click(new_ctx, btn);
         }
      }
      return true;
   }

   void basic_menu::drag(context const& ctx, mouse_button btn)
   {
      if (!_popup)
         return;

      rect  bounds = _popup->bounds();
      if (bounds.includes(btn.pos))
      {
         context new_ctx{ ctx.view, ctx.canvas, _popup.get(), bounds };
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

   bool basic_menu::key(context const& ctx, key_info k)
   {
      if (!_popup)
         return false;

      // simulate a menu key:
      rect bounds = _popup->bounds();
      context new_ctx{ ctx.view, ctx.canvas, _popup.get(), bounds };

      basic_menu_item_element* hit = nullptr;
      basic_menu_item_element* first = nullptr;
      basic_menu_item_element* last = nullptr;

      new_ctx.listen<basic_menu_item_element>(
         [&](auto const& /* ctx */, auto& e, auto what)
         {
            if (what == "key" || what == "click")
            {
               hit = &e;
            }
            else if (what == "arrows" && e.is_enabled())
            {
               if (!first)
                  first = &e;
               last = &e;
            }
         }
      );

      if (_popup->key(new_ctx, k))
      {
         if (hit)
         {
            _popup->close(ctx.view);
            state(false);
            ctx.view.refresh();
         }
         return true;
      }

      if (k.action == key_action::press || k.action == key_action::repeat)
      {
         if (k.key == key_code::down)
         {
            if (first)
            {
               first->select(true);
               ctx.view.refresh(new_ctx);
               first->scroll_into_view();
            }
         }
         else if (k.key == key_code::up)
         {
            if (last)
            {
               last->select(true);
               ctx.view.refresh(new_ctx);
               last->scroll_into_view();
            }
         }
         return false;
      }

      // Call base key
      return layered_button::key(ctx, k);
   }

   bool basic_menu::wants_focus() const
   {
      return true;
   }

   void basic_menu_item_element::draw(context const& ctx)
   {
      if (is_selected() && is_enabled())
      {
         if (_scroll_into_view)
         {
            scrollable::find(ctx).scroll_into_view(ctx.bounds);
            ctx.view.refresh(ctx.bounds);
            _scroll_into_view = false;
         }

         auto& canvas_ = ctx.canvas;

         canvas_.begin_path();
         canvas_.round_rect(ctx.bounds, 2);
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

   element* basic_menu_item_element::hit_test(context const& ctx, point p)
   {
      if (is_enabled() && ctx.bounds.includes(p))
         return this;
      return nullptr;
   }

   bool basic_menu_item_element::click(context const& ctx, mouse_button btn)
   {
      bool result = false;
      if (is_enabled() && ctx.bounds.includes(btn.pos))
      {
         if (on_click)
            on_click();
         select(false);
         ctx.notify(ctx, "click", this);
         result = true;
      }
      auto proxy_result = proxy_base::click(ctx, btn);
      return result || proxy_result;
   }

   bool basic_menu_item_element::key(context const& ctx, key_info k)
   {
      if (k.action == key_action::press || k.action == key_action::repeat)
      {
         auto&& equal =
            [](auto k, auto shortcut)
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
               return (k.key == shortcut.key) &&
                  ((k.modifiers & mask) == (shortcut.modifiers & mask))
                  ;
            };

         switch (k.key)
         {
            case key_code::enter:
               if (is_selected() && is_enabled())
               {
                  select(false);
                  if (on_click)
                     on_click();
                  ctx.notify(ctx, "key", this);
                  return true;
               }
               break;

            case key_code::escape:
               {
                  select(false);
                  ctx.notify(ctx, "key", this);
                  return true;
               }
               break;

            case key_code::up:
            case key_code::down:
               {
                  if (!is_selected())
                  {
                     ctx.notify(ctx, "arrows", this);
                     return false;
                  }

                  auto [c, cctx] = find_composite(ctx);
                  if (c)
                  {
                     bool const down = k.key == key_code::down;
                     auto const last = static_cast<int>(c->size()) - 1;
                     bool found = false;
                     for (
                        int i = down? 0 : last;
                        i != (down? static_cast<int>(c->size()) : -1);
                        i += down? +1 : -1
                     )
                     {
                        auto e = find_element<basic_menu_item_element*>(&c->at(i));
                        if (e && e->is_enabled())
                        {
                           if (e == this)
                           {
                              if (i != (down? last : 0))
                                 found = true;
                           }
                           else if (found)
                           {
                              select(false);
                              e->select(true);
                              rect bounds = c->bounds_of(*cctx, i);
                              cctx->view.refresh(*cctx);
                              scrollable::find(ctx).scroll_into_view(bounds);
                              break;
                           }
                        }
                     }
                  }
                  return true;
               }
               break;

            default:
               if (is_enabled() && equal(k, shortcut))
               {
                  if (on_click)
                     on_click();
                  ctx.notify(ctx, "key", this);
                  return true;
               }
         }
      }
      return false;
   }

   bool basic_menu_item_element::cursor(context const& ctx, point p, cursor_tracking status)
   {
      if (!is_enabled())
         return false;

      bool hit = ctx.bounds.includes(p);
      if (status == cursor_tracking::leaving || hit)
      {
         auto [c, cctx] = find_composite(ctx);
         if (c)
         {
            for (std::size_t i = 0; i != c->size(); ++i)
            {
               if (auto e = find_element<selectable*>(&c->at(i)))
                  e->select(false);
            }
         }
         select(hit);
         cctx->view.refresh(*cctx);
      }
      proxy_base::cursor(ctx, p, status);
      return hit;
   }

   bool basic_menu_item_element::wants_control() const
   {
      return true;
   }
}}
