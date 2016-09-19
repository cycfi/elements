/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/menu.hpp>

namespace photon
{
   void basic_dropdown_menu::layout(context const& ctx)
   {
      basic_button::layout(ctx);
      popup().layout_from_button(ctx);
   }

   widget* basic_dropdown_menu::click(context const& ctx, mouse_button btn)
   {
      if (btn.down)
      {
         if (state(true))
         {
            popup().install(ctx, *this);
            ctx.view.refresh();
         }
      }
      else
      {
         if (!value() || !hit_test(ctx, btn.pos))
         {
            // simulate a menu click:
            popup().click_from_button(ctx, btn);
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

   basic_popup_widget& basic_dropdown_menu::popup() const
   {
      return *std::dynamic_pointer_cast<basic_popup_widget>(_menu).get();
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
