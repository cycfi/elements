/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/gallery/check_box.hpp>

namespace cycfi { namespace elements
{
   void check_box_element::draw(context const& ctx)
   {
      auto&       canvas_ = ctx.canvas;
      auto        canvas_state = canvas_.new_state();
      auto const& theme_ = get_theme();
      rect        box = ctx.bounds.move(15, 0);

      box.width(box.height());
      auto  state = value();
      auto  value = state.value;
      auto  hilite = state.hilite;
      auto  tracking = state.tracking;

      color c1 = (value || tracking) ?
         (hilite? theme_.indicator_hilite_color : theme_.indicator_bright_color) :
         colors::black.opacity(theme_.element_background_opacity)
         ;

      if (tracking)
         c1 = c1.level(0.4);

      if (value || tracking)
         draw_icon(canvas_, box, icons::ok, 14, c1);

      auto line_width = theme_.controls_frame_stroke_width;
      color outline_color = hilite? theme_.frame_hilite_color : theme_.frame_color;
      canvas_.line_width(line_width);
      canvas_.begin_path();
      canvas_.round_rect(box.inset(1, 1), 3);
      canvas_.stroke_style(outline_color);
      canvas_.stroke();

      // Pseudo glow
      auto glow_width = hilite? line_width*2 : line_width;
      auto inset = glow_width/3;
      auto glow_box = box.inset(inset, inset);
      canvas_.round_rect(glow_box, 4);
      canvas_.line_width(glow_width);
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
      canvas_.fill_text(point{ cx, cy }, _text.c_str());
   }
}}
