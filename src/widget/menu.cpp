/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/menu.hpp>

namespace photon
{
   void menu_background::draw(context const& ctx)
   {
      auto&       canvas_ = ctx.canvas;
      auto const& bounds = ctx.bounds;

      // Panel fill
      canvas_.begin_path();
      canvas_.rect(bounds);
      canvas_.fill_style(get_theme().panel_color.opacity(0.95));
      canvas_.fill();
   }

   namespace
   {
      auto find_menu(context const& ctx, widget_ptr _menu)
      {
         return std::find(ctx.view.content.begin(), ctx.view.content.end(), _menu);
      }

      void install_menu(context const& ctx, widget_ptr _menu, basic_button& button_)
      {
         if (auto p = std::dynamic_pointer_cast<basic_menu_widget>(_menu))
         {
            ctx.view.content.push_back(_menu);
            p->button(&button_);
         }
      }

      void layout_menu(context const& ctx, widget_ptr _menu)
      {
         if (auto p = std::dynamic_pointer_cast<basic_menu_widget>(_menu))
         {
            rect  bounds = {
                  ctx.bounds.left+3, ctx.bounds.bottom,
                  ctx.bounds.right-3, full_extent
               };

            context new_ctx{ ctx.view, ctx.canvas, _menu.get(), bounds };
            p->bounds(bounds);
            p->layout(new_ctx);
         }
      }

      void click_menu(context const& ctx, widget_ptr _menu, mouse_button btn)
      {
         if (auto p = std::dynamic_pointer_cast<basic_menu_widget>(_menu))
         {
            btn.down = true;
            rect  bounds = p->bounds();
            context new_ctx{ ctx.view, ctx.canvas, _menu.get(), bounds };
            p->click(new_ctx, btn);
         }
      }

      void remove_menu(context const& ctx, widget_ptr _menu)
      {
         auto i = find_menu(ctx, _menu);
         if (i != ctx.view.content.end())
            ctx.view.content.erase(i);
      }
   }

   void basic_dropdown_menu::layout(context const& ctx)
   {
      basic_button::layout(ctx);
      layout_menu(ctx, _menu);
   }

   widget* basic_dropdown_menu::click(context const& ctx, mouse_button btn)
   {
      if (btn.down)
      {
         if (state(true))
         {
            install_menu(ctx, _menu, *this);
            ctx.view.refresh();
         }
      }
      else
      {
         if (!value() || !hit_test(ctx, btn.pos))
         {
            // simulate a menu click:
            click_menu(ctx, _menu, btn);
         }
      }
      return this;
   }

   void basic_dropdown_menu::drag(context const& ctx, mouse_button btn)
   {
      ctx.view.refresh();
   }

   //bool basic_dropdown_menu::key(context const& ctx, key_info const& k)
   //{
   //   if (k.key == key_code::key_escape)
   //   {
   //      remove_menu(ctx, _menu);
   //      state(false);
   //      ctx.view.refresh();
   //      return true;
   //   }
   //   return false;
   //}

   bool basic_dropdown_menu::focus(focus_request r)
   {
      return true;
   }

   widget* basic_menu_widget::hit_test(context const& ctx, point p)
   {
      // We call widget::hit_test instead of proxy_base::hit_test because
      // we want to process hits/clicks outside the subject's bounds (e.g.
      // to dismiss the menu when cliced anywhere outside the menu bounds).
      return widget::hit_test(ctx, p);
   }

   widget* basic_menu_widget::click(context const& ctx, mouse_button btn)
   {
      auto r = floating_widget::click(ctx, btn);
      remove_menu(ctx, shared_from_this());
      _button->value(0);
      ctx.view.refresh();
      return r;
   }

   bool basic_menu_widget::cursor(context const& ctx, point p, cursor_tracking status)
   {
      bool hit = proxy_base::hit_test(ctx, p);
      if (status == cursor_tracking::leaving || hit)
         ctx.view.refresh();
      proxy_base::cursor(ctx, p, status);
      return hit;
   }

   void basic_menu_item_widget::draw(context const& ctx)
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

   widget* basic_menu_item_widget::hit_test(context const& ctx, point p)
   {
      if (ctx.bounds.includes(p))
         return this;
      ctx.view.refresh(ctx);
      return 0;
   }

   widget* basic_menu_item_widget::click(context const& ctx, mouse_button btn)
   {
      if (/*btn.down && */on_click)
         on_click();
      return proxy_base::click(ctx, btn);
   }

   bool basic_menu_item_widget::cursor(context const& ctx, point p, cursor_tracking status)
   {
      bool hit = ctx.bounds.includes(p);
      if (status == cursor_tracking::leaving || hit)
         ctx.view.refresh();
      proxy_base::cursor(ctx, p, status);
      return hit;
   }

   bool basic_menu_item_widget::is_control() const
   {
      return true;
   }
}
