/*=============================================================================
   Copyright (c) 2016-2018 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <photon/element/menu.hpp>

namespace cycfi { namespace photon
{
   element* basic_popup_element::hit_test(context const& ctx, point p)
   {
      // We call element::hit_test instead of proxy_base::hit_test because
      // we want to process hits/clicks outside the subject's bounds (e.g.
      // to dismiss the menu when clicked anywhere outside the menu bounds).
      return element::hit_test(ctx, p);
   }

   element* basic_popup_element::click(context const& ctx, mouse_button btn)
   {
      auto r = floating_element::click(ctx, btn);
      if (_on_click)
         _on_click(ctx, btn);
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

   void basic_popup_element::open(context const& ctx, click_function on_click)
   {
      ctx.view.content().push_back(shared_from_this());
      _on_click = on_click;
   }

   void basic_popup_element::close(context const& ctx)
   {
      auto i = std::find(
         ctx.view.content().begin(), ctx.view.content().end(), shared_from_this()
      );

      if (i != ctx.view.content().end())
         ctx.view.content().erase(i);
   }
}}
