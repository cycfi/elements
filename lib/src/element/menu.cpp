/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/menu.hpp>
#include <elements/element/tile.hpp>
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
      if (!_popup)
         return false;

      // simulate a menu key:
      rect bounds = _popup->bounds();
      context new_ctx{ ctx.view, ctx.canvas, _popup.get(), bounds };

      bool hit = false;
      new_ctx.feedback(
         [&hit](auto* e, std::string_view what)
         {
            if (dynamic_cast<basic_menu_item_element*>(e))
               hit = true;
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
         ctx.give_feedback(this, "click");
         result = this;
      }
      element* proxy_result = proxy_base::click(ctx, btn);
      return result? result : proxy_result;
   }

   namespace
   {
      template <typename F>
      void find_composite(context const& ctx, F&& f)
      {
         auto p = ctx.parent;
         while (p)
         {
            bool break_ = false;
            auto&& find =
               [&](context const& ctx, element* e)
               {
                  if (auto vt = dynamic_cast<vtile_element*>(e))
                     f(ctx, vt, break_);
               };

            auto e = p->element;
            find(*p, e);
            proxy_base* proxy = dynamic_cast<proxy_base*>(e);

            while (!break_ && proxy)
            {
               auto* subject = &proxy->subject();
               find(*p, subject);
               proxy = dynamic_cast<proxy_base*>(subject);
            }

            if (break_)
               break;
            p = p->parent;
         }
      }
   }

   bool basic_menu_item_element::key(context const& ctx, key_info k)
   {
      if (k.action == key_action::press)
      {
         if (k.key == key_code::escape || k.key == key_code::enter)
         {
            if (k.key == key_code::enter && on_click)
               on_click();
            ctx.give_feedback(this, "key");
            return true;
         }

         else if (is_selected()
            && (k.key == key_code::up || k.key == key_code::down))
         {
            find_composite(ctx,
               [k, this, &ctx](context const& ctx, auto* c, bool& break_)
               {
                  rect bounds;
                  bool found = false;
                  if (k.key == key_code::down)
                  {
                     for (std::size_t i = 0; i != c->size(); ++i)
                     {
                        if (auto e = dynamic_cast<selectable*>(&c->at(i)))
                        {
                           if (e == this)
                           {
                              if (i == c->size()-1)
                              {
                                 break_ = true;
                                 break;
                              }
                              found = true;
                              e->select(false);
                           }
                           else if (found)
                           {
                              e->select(true);
                              bounds = c->bounds_of(ctx, i);
                              break_ = true;
                              break;
                           }
                        }
                     }
                  }
                  else
                  {
                     for (int i = c->size()-1; i >= 0; --i)
                     {
                        if (auto e = dynamic_cast<selectable*>(&c->at(i)))
                        {
                           if (e == this)
                           {
                              if (i == 0)
                              {
                                 break_ = true;
                                 break;
                              }
                              found = true;
                              e->select(false);
                           }
                           else if (found)
                           {
                              e->select(true);
                              bounds = c->bounds_of(ctx, i);
                              break_ = true;
                              break;
                           }
                        }
                     }
                  }
                  if (break_)
                  {
                     ctx.view.refresh(*c);
                     if (bounds != rect{} &&
                        !scrollable::find(ctx).scroll_into_view(bounds))
                        ctx.view.refresh(ctx);
                  }
               }
            );
            return true;
         }

         else if (k.key == shortcut.key && (k.modifiers & ~mod_action) == shortcut.modifiers)
         {
            if (on_click)
               on_click();
            ctx.give_feedback(this, "key");
            return true;
         }
      }
      return false;
   }

   bool basic_menu_item_element::cursor(context const& ctx, point p, cursor_tracking status)
   {
      bool hit = ctx.bounds.includes(p);
      if (status == cursor_tracking::leaving || hit)
      {
         find_composite(ctx,
            [](auto const& ctx, auto* c, bool& break_)
            {
               for (std::size_t i = 0; i != c->size(); ++i)
               {
                  if (auto e = dynamic_cast<selectable*>(&c->at(i)))
                  {
                     e->select(false);
                     break_ = true;
                  }
               }
            }
         );
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
