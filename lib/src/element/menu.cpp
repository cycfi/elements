/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/menu.hpp>

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

      if (k.action == key_action::press)
      {
         if (k.key == key_code::escape || k.key == key_code::enter)
         {
            if (k.key == key_code::enter)
            {
               // simulate a menu click:
               rect bounds = _popup->bounds();
               context new_ctx{ ctx.view, ctx.canvas, _popup.get(), bounds };
               mouse_button btn{
                  true, 1, mouse_button::left, 0, ctx.view.cursor_pos()
               };
               _popup->click(new_ctx, btn);
            }

            _popup->close(ctx.view);
            state(false);
            ctx.view.refresh();
            return true;
         }
      }

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
      if (ctx.bounds.includes(ctx.view.cursor_pos()))
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

   bool basic_menu_item_element::key(context const& ctx, key_info k)
   {
      if (k.key == shortcut.key && (k.modifiers & ~mod_action) == shortcut.modifiers)
      {
         if (on_click)
            on_click();
         ctx.give_feedback(this, "key");
         return true;
      }
      return false;
   }

   bool basic_menu_item_element::cursor(context const& ctx, point p, cursor_tracking status)
   {
      bool hit = ctx.bounds.includes(p);
      if (status == cursor_tracking::leaving || hit)
         ctx.view.refresh();
      proxy_base::cursor(ctx, p, status);
      return hit;
   }

   bool basic_menu_item_element::is_control() const
   {
      return true;
   }
}}
