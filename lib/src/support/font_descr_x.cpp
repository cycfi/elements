/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/support/font_descr_x.hpp>
#include <pango/pangocairo.h>
#include <utility>

namespace cycfi { namespace elements
{
   font_descr_x::font_descr_x(char const* family)
    : _ptr{ pango_font_description_new() }
   {
      pango_font_description_set_family(_ptr, family);
   }

   font_descr_x::font_descr_x(font_descr_x const& rhs)
    : _ptr{ pango_font_description_copy(rhs._ptr) }
   {}

   font_descr_x::font_descr_x(font_descr_x&& rhs)
    : _ptr{ rhs._ptr }
   {
      rhs._ptr = nullptr;
   }

   font_descr_x::~font_descr_x()
   {
      if (_ptr)
         pango_font_description_free(_ptr);
   }

   font_descr_x& font_descr_x::operator=(font_descr_x const& rhs)
   {
      if (&rhs != this)
         _ptr = pango_font_description_copy(rhs._ptr);
      return *this;
   }

   font_descr_x& font_descr_x::operator=(font_descr_x&& rhs)
   {
      if (&rhs != this)
      {
         _ptr = rhs._ptr;
         rhs._ptr = nullptr;
      }
      return *this;
   }

   font_descr_x font_descr_x::size(float pt) const
   {
      font_descr_x r = *this;
      pango_font_description_set_size(r._ptr, pt * PANGO_SCALE);
      return r;
   }

   font_descr_x font_descr_x::weight(int w) const
   {
      font_descr_x r = *this;
      pango_font_description_set_weight(r._ptr, PangoWeight(w));
      return r;
   }

   font_descr_x font_descr_x::style(style_enum s) const
   {
      font_descr_x r = *this;
      pango_font_description_set_style(r._ptr, PangoStyle(s));
      return r;
   }

   font_descr_x font_descr_x::stretch(stretch_enum s) const
   {
      font_descr_x r = *this;
      pango_font_description_set_stretch(r._ptr, PangoStretch(s));
      return r;
   }

   font_descr_x font_descr_x::underline() const
   {
      font_descr_x r = *this;
      r._line = line_underline;
      return r;
   }

   font_descr_x font_descr_x::strikethrough() const
   {
      font_descr_x r = *this;
      r._line = line_strikethrough;
      return r;
   }
}}


