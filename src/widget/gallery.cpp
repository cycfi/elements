/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/gallery.hpp>

namespace photon
{
   rect label::limits(basic_context const& ctx) const
   {
      auto const&    theme_ = get_theme();
      auto           canvas_ = ctx.canvas();

      canvas_.font(theme_.label_font, theme_.label_font_size * _size);
      auto  info = canvas_.measure_text(_text.c_str());
      return { info.size.x, info.size.y, info.size.x, info.size.y };
   }

   void label::draw(context const& ctx)
   {
      auto const&    theme_ = get_theme();
      auto           canvas_ = ctx.canvas();
      auto           state = canvas_.new_state();

      canvas_.fill_style(theme_.label_font_color);
      canvas_.font(theme_.label_font, theme_.label_font_size * _size);
      canvas_.text_align(canvas_.middle | canvas_.center);

      float cx = ctx.bounds.left + (ctx.bounds.width() / 2);
      float cy = ctx.bounds.top + (ctx.bounds.height() / 2);

      canvas_.rect(ctx.bounds);
      //canvas_.clip();
      canvas_.fill_text(point{ cx, cy }, _text.c_str());
   }
}
