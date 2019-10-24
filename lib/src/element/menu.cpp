/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/menu.hpp>
#include <elements/element/composite.hpp>
#include <elements/element/port.hpp>

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

   element* basic_menu::click(context const& ctx, mouse_button btn)
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

               auto on_click = [this](view& view_)
               {
                  _popup->close(view_);
                  this->value(0);
                  view_.refresh();
               };

               _popup->open(ctx.view, on_click);
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
      return this;
   }

   void basic_menu::drag(context const& ctx, mouse_button btn)
   {
      ctx.view.refresh();
   }

   bool basic_menu::key(context const& ctx, key_info k)
   {
      if (!_popup || !ctx.view.is_open(_popup))
         return false;

      // simulate a menu key:
      rect bounds = _popup->bounds();
      context new_ctx{ ctx.view, ctx.canvas, _popup.get(), bounds };

      bool hit = false;
      basic_menu_item_element* first = nullptr;
      basic_menu_item_element* last = nullptr;
      rect first_bounds, last_bounds;
      new_ctx.feedback(
         [&](context const& ctx, auto* e, std::string_view what)
         {
            if (auto me = dynamic_cast<basic_menu_item_element*>(e))
            {
               if (what == "key" || what == "click")
               {
                  hit = true;
               }
               else if (what == "arrows")
               {
                  if (!first)
                  {
                     first = me;
                     first_bounds = ctx.bounds;
                  }
                  last = me;
                  last_bounds = ctx.bounds;
               }
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
               ctx.view.refresh(first_bounds);
               first->scroll_into_view();
            }
         }
         else if (k.key == key_code::up)
         {
            if (last)
            {
               last->select(true);
               ctx.view.refresh(last_bounds);
               last->scroll_into_view();
            }
         }
         return true;
      }

      // Call base key
      return layered_button::key(ctx, k);
   }

   bool basic_menu::focus(focus_request r)
   {
      return true;
   }

   void basic_menu_item_element::draw(context const& ctx)
   {
      if (is_selected())
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
      proxy_base::draw(ctx);
   }

   element* basic_menu_item_element::hit_test(context const& ctx, point p)
   {
      if (ctx.bounds.includes(p))
         return this;
      ctx.view.refresh(ctx);
      return 0;
   }

   element* basic_menu_item_element::click(context const& ctx, mouse_button btn)
   {
      element* result = nullptr;
      if (ctx.bounds.includes(btn.pos))
      {
         if (on_click)
            on_click();
         ctx.give_feedback(ctx, "click", this);
         result = this;
      }
      element* proxy_result = proxy_base::click(ctx, btn);
      return result? result : proxy_result;
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
               if (is_selected())
               {
                  if (on_click)
                     on_click();
                  select(false);
                  ctx.give_feedback(ctx, "key", this);
                  return true;
               }
               break;

            case key_code::escape:
               {
                  ctx.give_feedback(ctx, "key", this);
                  return true;
               }
               break;

            case key_code::up:
            case key_code::down:
               {
                  if (!is_selected())
                  {
                     ctx.give_feedback(ctx, "arrows", this);
                     return false;
                  }

                  rect bounds;
                  auto [c, cctx] = find_composite(ctx);
                  auto&& refresh = [&](auto c, auto cctx, auto const& bounds)
                  {
                     cctx->view.refresh(*cctx);
                     scrollable::find(ctx).scroll_into_view(bounds);
                  };

                  if (c)
                  {
                     if (k.key == key_code::down)
                     {
                        bool found = false;
                        for (std::size_t i = 0; i != c->size(); ++i)
                        {
                           if (auto e = dynamic_cast<selectable*>(&c->at(i)))
                           {
                              if (e == this)
                              {
                                 if (i == c->size()-1)
                                    break;
                                 found = true;
                                 e->select(false);
                              }
                              else if (found)
                              {
                                 e->select(true);
                                 bounds = c->bounds_of(*cctx, i);
                                 refresh(c, cctx, bounds);
                                 break;
                              }
                           }
                        }
                     }
                     else
                     {
                        bool found = false;
                        for (int i = c->size()-1; i >= 0; --i)
                        {
                           if (auto e = dynamic_cast<selectable*>(&c->at(i)))
                           {
                              if (e == this)
                              {
                                 if (i == 0)
                                    break;
                                 found = true;
                                 e->select(false);
                              }
                              else if (found)
                              {
                                 e->select(true);
                                 bounds = c->bounds_of(*cctx, i);
                                 refresh(c, cctx, bounds);
                                 break;
                              }
                           }
                        }
                     }
                  }
                  return true;
               }
               break;

            default:
               if (equal(k, shortcut))
               {
                  if (on_click)
                     on_click();
                  ctx.give_feedback(ctx, "key", this);
                  return true;
               }
         }
      }
      return false;
   }

   bool basic_menu_item_element::cursor(context const& ctx, point p, cursor_tracking status)
   {
      bool hit = ctx.bounds.includes(p);
      if (status == cursor_tracking::leaving || hit)
      {
         auto [c, cctx] = find_composite(ctx);
         if (c)
         {
            for (std::size_t i = 0; i != c->size(); ++i)
            {
               if (auto e = dynamic_cast<selectable*>(&c->at(i)))
                  e->select(false);
            }
         }
         select(hit);
         ctx.view.refresh();
      }
      proxy_base::cursor(ctx, p, status);
      return hit;
   }

   bool basic_menu_item_element::is_control() const
   {
      return true;
   }
}}
