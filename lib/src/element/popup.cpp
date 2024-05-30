/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/menu.hpp>
#include <infra/support.hpp>

namespace cycfi::elements
{
   element* basic_popup_element::hit_test(context const &, point p, bool /*leaf*/, bool /*control*/)
   {
      return bounds().includes(p)? this : nullptr;
   }

   bool basic_popup_element::cursor(context const& ctx, point p, cursor_tracking status)
   {
      bool hit = proxy_base::hit_test(ctx, p, false, true);
      if (status == cursor_tracking::leaving || hit)
         ctx.view.refresh();
      bool r = proxy_base::cursor(ctx, p, status);
      on_cursor(p, status);
      return r;
   }

   bool basic_popup_element::is_open(view& view_) const
   {
      return view_.is_open(std::const_pointer_cast<element>(shared_from_this()));
   }

   void basic_popup_element::open(view& view_)
   {
      view_.add(shared_from_this(), true);
      view_.refresh();
   }

   void basic_popup_element::close(view& view_)
   {
      view_.remove(shared_from_this());
      view_.refresh();
   }

   element* basic_popup_menu_element::hit_test(context const& ctx, point p, bool leaf, bool control)
   {
      // We call element::hit_test instead of proxy_base::hit_test because we
      // want to process hits/clicks outside the subject's bounds (e.g. to
      // dismiss the menu when clicked anywhere outside the menu bounds).
      return element::hit_test(ctx, p, leaf, control);
   }

   bool basic_popup_menu_element::click(context const& ctx, mouse_button btn)
   {
      auto new_ctx = ctx.sub_context();
      auto r = floating_element::click(new_ctx, btn);
      if (btn.down && !r)
         close(ctx.view);
      return r;
   }

   void basic_popup_menu_element::close(view& view_)
   {
      basic_popup_element::close(view_);
      if (_menu_button)
         _menu_button->value(0);
   }
}
