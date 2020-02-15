/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/support/font_descr.hpp>
#include <pango/pangocairo.h>

namespace cycfi { namespace elements
{
   font_descr::font_descr(char const* descr)
    : _ptr{ pango_font_description_from_string(descr) }
   {}

   font_descr::font_descr(font_descr const& rhs)
    : _ptr{ pango_font_description_copy(rhs._ptr) }
   {}

   font_descr::~font_descr()
   {
      pango_font_description_free(_ptr);
   }

   font_descr& font_descr::operator=(font_descr const& rhs)
   {
      if (&rhs != this)
         _ptr = pango_font_description_copy(rhs._ptr);
      return *this;
   }

   font_descr& font_descr::operator=(font_descr&& rhs)
   {
      if (&rhs != this)
      {
         _ptr = rhs._ptr;
         rhs._ptr = nullptr;
      }
      return *this;
   }

   void font_descr::size(float pt)
   {
      pango_font_description_set_size(_ptr, pt * PANGO_SCALE);
   }

   float font_descr::size() const
   {
      return pango_font_description_get_size(_ptr) / PANGO_SCALE;
   }

   void font_descr::weight(int w)
   {
      pango_font_description_set_weight(_ptr, PangoWeight(w));
   }

   int font_descr::weight() const
   {
      return int(pango_font_description_get_weight(_ptr));
   }
}}


