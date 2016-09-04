/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/support/text_utils.hpp>
#include <photon/support/misc.hpp>

namespace photon
{
   void draw_icon(canvas& cnv, rect bounds, uint32_t code)
   {
      auto  state = cnv.new_state();
      float cx = bounds.left + (bounds.width() / 2);
      float cy = bounds.top + (bounds.height() / 2);
      cnv.text_align(cnv.middle | cnv.center);
      cnv.fill_text(point{ cx, cy }, codepoint_to_utf8(code).c_str());
   }

   point measure_icon(canvas& cnv, uint32_t cp)
   {
      return cnv.measure_text(codepoint_to_utf8(cp).c_str()).size;
   }

   namespace detail
   {
      char const* codepoint_to_utf8(int cp, char str[8])
      {
         int n = 0;
         if (cp < 0x80) n = 1;
         else if (cp < 0x800) n = 2;
         else if (cp < 0x10000) n = 3;
         else if (cp < 0x200000) n = 4;
         else if (cp < 0x4000000) n = 5;
         else if (cp <= 0x7fffffff) n = 6;
         str[n] = '\0';

         switch (n)
         {
            case 6: str[5] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0x4000000;
            case 5: str[4] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0x200000;
            case 4: str[3] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0x10000;
            case 3: str[2] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0x800;
            case 2: str[1] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0xc0;
            case 1: str[0] = cp;
         }
         return str;
      }
   }

   std::string codepoint_to_utf8(int cp)
   {
      std::string result{ 8 };
      detail::codepoint_to_utf8(cp, &result[0]);
      return result;
   }
}
