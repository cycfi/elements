/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/support/text_utils.hpp>
#include <elements/support/theme.hpp>

namespace cycfi { namespace elements
{
   void draw_icon(canvas& cnv, rect bounds, uint32_t code, float size, color c)
   {
      auto  state = cnv.new_state();
      auto& thm = get_theme();
      float cx = bounds.left + (bounds.width() / 2);
      float cy = bounds.top + (bounds.height() / 2);
      cnv.font(thm.icon_font, size);
      cnv.fill_style(c);
      cnv.text_align(cnv.middle | cnv.center);
      cnv.fill_text(point{ cx, cy }, codepoint_to_utf8(code).c_str());
   }

   void draw_icon(canvas& cnv, rect bounds, uint32_t code, float size)
   {
      draw_icon(cnv, bounds, code, size, get_theme().icon_color);
   }

   point measure_icon(canvas& cnv, uint32_t cp, float size)
   {
      auto  state = cnv.new_state();
      auto& thm = get_theme();
      cnv.font(thm.icon_font, size);
      return cnv.measure_text(codepoint_to_utf8(cp).c_str()).size;
   }

   point measure_text(canvas& cnv, char const* text, font const& font_, float size)
   {
      auto  state = cnv.new_state();
      cnv.font(font_, size);
      auto  info = cnv.measure_text(text);
      auto  height = info.ascent + info.descent + info.leading;
      return { info.size.x, height };
   }

   namespace detail
   {
      char const* codepoint_to_utf8(unsigned cp, char str[8])
      {
         size_t n = 0;
         if (cp < 0x80) n = 1;
         else if (cp < 0x800) n = 2;
         else if (cp < 0x10000) n = 3;
         else if (cp < 0x200000) n = 4;
         else if (cp < 0x4000000) n = 5;
         else if (cp <= 0x7fffffff) n = 6;
         str[n] = '\0';

         switch (n)
         {
            case 6: str[5] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0x4000000; [[fallthrough]];
            case 5: str[4] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0x200000;  [[fallthrough]];
            case 4: str[3] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0x10000;   [[fallthrough]];
            case 3: str[2] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0x800;     [[fallthrough]];
            case 2: str[1] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0xc0;      [[fallthrough]];
            case 1: str[0] = cp;
         }
         return str;
      }
   }

   std::string codepoint_to_utf8(unsigned codepoint)
   {
      char result[8];
      detail::codepoint_to_utf8(codepoint, &result[0]);
      return { result };
   }
}}
