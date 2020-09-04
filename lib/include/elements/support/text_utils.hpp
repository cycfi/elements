/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_TEXT_UTILS_MAY_22_2016)
#define ELEMENTS_TEXT_UTILS_MAY_22_2016

#include <artist/canvas.hpp>
#include <artist/font.hpp>

namespace cycfi::elements
{
   using artist::canvas;
   using artist::rect;
   using artist::point;
   using artist::color;
   using artist::font_descr;

   ////////////////////////////////////////////////////////////////////////////
   void           draw_icon(canvas& cnv, rect bounds, uint32_t code, float size);
   void           draw_icon(canvas& cnv, rect bounds, uint32_t code, float size, color c);
   point          measure_icon(canvas& cnv, uint32_t cp, float size);
   point          measure_text(canvas& cnv, char const* text, font_descr font_);
}

#endif
