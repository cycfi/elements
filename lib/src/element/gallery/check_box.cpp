/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/gallery/check_box.hpp>

namespace cycfi { namespace elements
{
   void draw_check_box(
     context const& ctx, std::string const& text, bool state, bool hilite
   )
   {
      auto&       canvas_ = ctx.canvas;
      auto        canvas_state = canvas_.new_state();
      auto const& theme_ = get_theme();
      color const indicator_color = theme_.indicator_color;
      float const bg_alfa = theme_.box_widget_bg_opacity;
      rect        box = ctx.bounds.move(15, 0);

      box.width(box.height());

      color c1 = state ? indicator_color.level(1.5) : rgb(0, 0, 0).opacity(bg_alfa);

      if (state && hilite)
         c1 = c1.level(1.3);

      if (state)
         draw_icon(canvas_, box, icons::ok, 14, c1.level(2.0));

      color outline_color = theme_.frame_color;
      canvas_.begin_path();
      canvas_.round_rect(box.inset(1, 1), 3);
      canvas_.stroke_style(outline_color);
      canvas_.stroke();

      // Pseudo glow
      canvas_.round_rect(box, 4);
      canvas_.stroke_style(outline_color.opacity(0.1));
      canvas_.stroke();

      canvas_.fill_style(theme_.label_font_color);
      canvas_.font(
         theme_.label_font,
         theme_.label_font_size
      );
      canvas_.text_align(canvas_.left | canvas_.middle);
      float cx = box.right + 10;
      float cy = ctx.bounds.top + (ctx.bounds.height() / 2);
      canvas_.fill_text(point{ cx, cy }, text.c_str());
   }
}}
