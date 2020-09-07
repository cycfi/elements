/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/support/text_utils.hpp>
#include <infra/utf8_utils.hpp>
#include <elements/support/theme.hpp>

namespace cycfi { namespace elements
{
   void draw_icon(canvas& cnv, rect bounds, uint32_t code, float size, color c)
   {
      auto  state = cnv.new_state();
      auto& thm = get_theme();
      float cx = bounds.left + (bounds.width() / 2);
      float cy = bounds.top + (bounds.height() / 2);
      cnv.font(thm.icon_font.size(size));
      cnv.fill_style(c);
      cnv.text_align(cnv.middle | cnv.center);
      cnv.fill_text(codepoint_to_utf8(code).c_str(), point{ cx, cy });
   }

   void draw_icon(canvas& cnv, rect bounds, uint32_t code, float size)
   {
      draw_icon(cnv, bounds, code, size, get_theme().icon_color);
   }

   point measure_icon(canvas& cnv, uint32_t cp, float size)
   {
      auto  state = cnv.new_state();
      auto& thm = get_theme();
      cnv.font(thm.icon_font.size(size));
      return cnv.measure_text(codepoint_to_utf8(cp).c_str()).size;
   }

   point measure_text(canvas& cnv, std::string_view text, font_descr font_)
   {
      auto  state = cnv.new_state();
      cnv.font(font_);
      auto  info = cnv.measure_text(text);
      auto  height = info.ascent + info.descent + info.leading;
      return { info.size.x, height };
   }
}}
