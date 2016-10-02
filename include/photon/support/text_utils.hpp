/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_TEXT_UTILS_MAY_22_2016)
#define PHOTON_GUI_LIB_TEXT_UTILS_MAY_22_2016

#include <photon/support/canvas.hpp>
#include <string>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   void           draw_icon(canvas& cnv, rect bounds, uint32_t code, float size);
   void           draw_icon(canvas& cnv, rect bounds, uint32_t code, float size, color c);
   point          measure_icon(canvas& cnv, uint32_t cp, float size);
   point          measure_text(canvas& cnv, char const* text, char const* face, float size);
   std::string    codepoint_to_utf8(unsigned codepoint);
   bool           is_space(unsigned codepoint);
   bool           is_newline(unsigned codepoint);
   unsigned       decode_utf8(unsigned& state, unsigned& codepoint, unsigned byte);

   ////////////////////////////////////////////////////////////////////////////////////////////////
   inline bool is_space(unsigned codepoint)
   {
      switch (codepoint)
      {
         case 9:        // \t
         case 11:       // \v
         case 12:       // \f
         case 32:       // space
			case 10:		   // \n
			case 13:		   // \r
         case 0xA0:     // NBSP
            return true;
         default:
            return false;
      }
   }

   // Check if codepoint is a new line
   inline bool is_newline(unsigned codepoint)
   {
      switch (codepoint)
      {
			case 10:		   // \n
			case 13:		   // \r
			case 0x85:	   // NEL
            return true;
         default:
            return false;
      }
   }

   // Check if byte is a valid UTF8 initial char
   inline bool valid_utf8_start(uint8_t byte)
   {
      return
         (byte >= 0x00 && byte <= 0x7F)   ||
         (byte >= 0xC2 && byte <= 0xDF)   ||
         (byte == 0xE0)                   ||
         (byte >= 0xE1 && byte <= 0xEC)   ||
         (byte == 0xED)                   ||
         (byte >= 0xEE && byte <= 0xEF)   ||
         (byte == 0xF0)                   ||
         (byte >= 0xF1 && byte <= 0xF3)   ||
         (byte == 0xF4)
      ;
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // decode_utf8
   // Copyright (c) 2008-2010 Bjoern Hoehrmann <bjoern@hoehrmann.de>
   // See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.

   enum
   {
      utf8_accept = 0,
      utf8_reject = 12
   };

   inline unsigned decode_utf8(unsigned& state, unsigned& codepoint, uint8_t byte)
   {
      static constexpr uint8_t utf8d[] =
      {
         // The first part of the table maps bytes to character classes that
         // to reduce the size of the transition table and create bitmasks.
         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,       0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,       0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,       0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,       0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
         1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,       9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
         7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,       7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
         8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,       2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
         10,3,3,3,3,3,3,3,3,3,3,3,3,4,3,3,      11,6,6,6,5,8,8,8,8,8,8,8,8,8,8,8,

         // The second part is a transition table that maps a combination
         // of a state of the automaton and a character class to a state.
         0,12,24,36,60,96,84,12,12,12,48,72,    12,12,12,12,12,12,12,12,12,12,12,12,
         12, 0,12,12,12,12,12, 0,12, 0,12,12,   12,24,12,12,12,12,12,24,12,24,12,12,
         12,12,12,12,12,12,12,24,12,12,12,12,   12,24,12,12,12,12,12,12,12,24,12,12,
         12,12,12,12,12,12,12,36,12,36,12,12,   12,36,12,12,12,12,12,36,12,36,12,12,
         12,36,12,12,12,12,12,12,12,12,12,12,
      };

      unsigned type = utf8d[byte];

      codepoint = (state != utf8_accept) ?
         (byte & 0x3fu) | (codepoint << 6) :
         (0xff >> type) & (byte)
         ;

      state = utf8d[256 + state + type];
      return state;
   }
}

#endif
