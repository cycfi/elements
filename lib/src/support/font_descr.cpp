/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/support/font_descr.hpp>
#include <pango/pangocairo.h>
#include <utility>

namespace cycfi { namespace elements
{
   font_descr::font_descr(char const* descr)
    : _ptr{ pango_font_description_from_string(descr) }
   {}

   font_descr::font_descr(font_descr const& rhs)
    : _ptr{ pango_font_description_copy(rhs._ptr) }
   {}

   font_descr::font_descr(font_descr&& rhs)
    : _ptr{ rhs._ptr }
   {
      rhs._ptr = nullptr;
   }

   font_descr::~font_descr()
   {
      if (_ptr)
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

   font_descr font_descr::size(float pt) const
   {
      font_descr r = *this;
      pango_font_description_set_size(r._ptr, pt * PANGO_SCALE);
      return r;
   }

   font_descr font_descr::weight(int w) const
   {
      font_descr r = *this;
      pango_font_description_set_weight(r._ptr, PangoWeight(w));
      return r;
   }

   font_descr font_descr::style(style_enum s) const
   {
      font_descr r = *this;
      pango_font_description_set_style(r._ptr, PangoStyle(s));
      return r;
   }

   font_descr font_descr::stretch(stretch_enum s) const
   {
      font_descr r = *this;
      pango_font_description_set_stretch(r._ptr, PangoStretch(s));
      return r;
   }
}}


