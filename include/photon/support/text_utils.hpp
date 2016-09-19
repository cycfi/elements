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
   void           draw_icon(canvas& cnv, rect bounds, uint32_t code, int size);
   void           draw_icon(canvas& cnv, rect bounds, uint32_t code, int size, color c);
   point          measure_icon(canvas& cnv, uint32_t cp, int size);
   std::string    codepoint_to_utf8(int cp);
}

#endif
