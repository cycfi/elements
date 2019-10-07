/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/menu.hpp>

namespace cycfi { namespace elements
{
   element* basic_popup_element::hit_test(context const& ctx, point p)
   {
      // We call element::hit_test instead of proxy_base::hit_test because we
      // want to process hits/clicks outside the subject's bounds (e.g. to
      // dismiss the menu when clicked anywhere outside the menu bounds).
      return element::hit_test(ctx, p);
   }

   element* basic_popup_element::click(context const& ctx, mouse_button btn)
   {
      bool hit = false;
      auto ff = ctx.feedback(
         [&hit](auto* e)
         {
            if (dynamic_cast<basic_menu_item_element*>(e))
               hit = true;
         }
      );

      auto r = floating_element::click(ctx, btn);
      if (btn.down && ((r == nullptr) || hit) && _on_click)
         _on_click(ctx.view);
      ctx.feedback(ff);
      return r;
   }

   bool basic_popup_element::cursor(context const& ctx, point p, cursor_tracking status)
   {
      bool hit = proxy_base::hit_test(ctx, p);
      if (status == cursor_tracking::leaving || hit)
         ctx.view.refresh();
      proxy_base::cursor(ctx, p, status);
      return hit;
   }

   void basic_popup_element::open(view& view_, click_function on_click)
   {
      view_.add(shared_from_this());
      _on_click = on_click;
   }

   void basic_popup_element::close(view& view_)
   {
      view_.remove(shared_from_this());
   }
}}
