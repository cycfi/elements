/*=============================================================================
   Copyright (c) 2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/gallery/tab.hpp>
#include <elements/support/draw_utils.hpp>

namespace cycfi { namespace elements
{
   view_limits tab_element::limits(basic_context const& ctx) const
   {
      auto& thm = get_theme();
      auto  size = measure_text(ctx.canvas, _text.c_str(), thm.label_font, thm.label_font_size);
      size.x += 35;
      size.y += 10;
      return { { size.x, size.y }, { size.x, size.y } };
   }

   void tab_element::draw(context const& ctx)
   {
      auto&       canvas_ = ctx.canvas;
      auto const& theme_ = get_theme();
      rect        box = ctx.bounds;
      auto        corner_radius = get_theme().frame_corner_radius;
      bool        state = value() > 1;
      bool        hilite = value() & 1;

      color c1 = (state ? colors::gray[60] : colors::black).opacity(0.5);
      color c2 = theme_.label_font_color;

      if (hilite)
      {
         c1 = c1.level(1.2);
         c2 = colors::white;
      }

      draw_button(canvas_, box, c1, corner_radius);
      canvas_.fill_style(c2);
      canvas_.font(
         theme_.label_font,
         theme_.label_font_size
      );
      canvas_.text_align(canvas_.center | canvas_.middle);
      float cx = box.left + (box.width() / 2);
      float cy = box.top + (box.height() / 2);
      canvas_.fill_text(point{ cx, cy }, _text.c_str());
   }

   bool tab_element::cursor(context const& ctx, point /* p */, cursor_tracking /* status */)
   {
      ctx.view.refresh(ctx);
      return true;
   }

   bool tab_element::wants_control() const
   {
      return true;
   }
}}
