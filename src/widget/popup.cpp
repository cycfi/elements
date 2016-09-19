/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/menu.hpp>

namespace photon
{
   namespace
   {
      auto find_popup(context const& ctx, widget_ptr _popup)
      {
         return std::find(ctx.view.content.begin(), ctx.view.content.end(), _popup);
      }
   }

   widget* basic_popup_widget::hit_test(context const& ctx, point p)
   {
      // We call widget::hit_test instead of proxy_base::hit_test because
      // we want to process hits/clicks outside the subject's bounds (e.g.
      // to dismiss the menu when cliced anywhere outside the menu bounds).
      return widget::hit_test(ctx, p);
   }

   widget* basic_popup_widget::click(context const& ctx, mouse_button btn)
   {
      auto r = floating_widget::click(ctx, btn);
      remove(ctx);
      _button->value(0);
      ctx.view.refresh();
      return r;
   }

   bool basic_popup_widget::cursor(context const& ctx, point p, cursor_tracking status)
   {
      bool hit = proxy_base::hit_test(ctx, p);
      if (status == cursor_tracking::leaving || hit)
         ctx.view.refresh();
      proxy_base::cursor(ctx, p, status);
      return hit;
   }

   void basic_popup_widget::remove(context const& ctx)
   {
      auto i = find_popup(ctx, shared_from_this());
      if (i != ctx.view.content.end())
         ctx.view.content.erase(i);
   }

   void basic_popup_widget::install(context const& ctx, basic_button& button_)
   {
      ctx.view.content.push_back(shared_from_this());
      _button = &button_;
   }

   void basic_popup_widget::layout_from_button(context const& ctx)
   {
      rect  bounds = {
            ctx.bounds.left+3, ctx.bounds.bottom,
            ctx.bounds.right-3, full_extent
         };

      context new_ctx{ ctx.view, ctx.canvas, this, bounds };
      this->bounds(bounds);
      layout(new_ctx);
   }

   void basic_popup_widget::click_from_button(context const& ctx, mouse_button btn)
   {
      btn.down = true;
      rect  bounds = this->bounds();
      context new_ctx{ ctx.view, ctx.canvas, this, bounds };
      click(new_ctx, btn);
   }
}

