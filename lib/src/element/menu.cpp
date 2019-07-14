/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/menu.hpp>

namespace cycfi { namespace elements
{
   void basic_dropdown_menu::layout(context const& ctx)
   {
      layered_button::layout(ctx);

      auto pu_limits = _popup->limits(ctx);
      rect  bounds = {
            ctx.bounds.left + 3, ctx.bounds.bottom,
            ctx.bounds.left + 3 + pu_limits.min.x, full_extent
         };

      context new_ctx{ ctx.view, ctx.canvas, _popup.get(), bounds };
      _popup->bounds(bounds);
      _popup->layout(new_ctx);
   }

   element* basic_dropdown_menu::click(context const& ctx, mouse_button btn)
   {
      if (btn.down)
      {
         if (state(true))
         {
            auto on_click = [this](context const& ctx, mouse_button btn)
            {
               _popup->close(ctx.view);
               this->value(0);
               ctx.view.refresh();
            };

            _popup->open(ctx.view, on_click);
            ctx.view.refresh();
         }
      }
      else
      {
         if (!value() || !hit_test(ctx, btn.pos))
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

   void basic_dropdown_menu::drag(context const& ctx, mouse_button btn)
   {
      ctx.view.refresh();
   }

   bool basic_dropdown_menu::key(context const& ctx, key_info k)
   {
      if (k.key == key_code::escape)
      {
         _popup->close(ctx.view);
         state(false);
         ctx.view.refresh();
         return true;
      }
      return false;
   }

   bool basic_dropdown_menu::focus(focus_request r)
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
      if (/*btn.down && */on_click)
         on_click();
      return proxy_base::click(ctx, btn);
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
