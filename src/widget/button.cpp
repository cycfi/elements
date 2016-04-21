/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/button.hpp>
#include <photon/theme.hpp>
#include <photon/support.hpp>
#include <photon/app.hpp>
#include <photon/window.hpp>

namespace photon
{
   void button_widget::draw(context const& ctx)
   {
      auto c = _color;
      if (_is_active)
         c.alpha = 0.5;
      ctx.theme().draw_button(ctx.bounds, c);
      subject()->draw(ctx);
   }

   widget* button_widget::click(context const& ctx, mouse_button btn)
   {
      _is_active = btn.is_pressed && ctx.bounds.includes(ctx.cursor_pos());
      ctx.window.draw();
      return this;
   }

   void button_widget::drag(context const& ctx, mouse_button btn)
   {
      _is_active = ctx.bounds.includes(ctx.cursor_pos());
      ctx.window.draw();
   }

   bool button_widget::is_control() const
   {
      return true;
   }
}
