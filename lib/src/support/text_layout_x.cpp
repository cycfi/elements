/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/support/text_layout_x.hpp>
#include <elements/support/canvas.hpp>
#include <pango/pangocairo.h>

namespace cycfi { namespace elements
{
   text_layout_x::text_layout_x(canvas& cnv)
    : _ptr{
         pango_layout_new(cnv.pango_context())
       , [](auto* ptr){ g_object_unref(ptr); }
      }
   {}

   void text_layout_x::text(std::string_view text_)
   {
      pango_layout_set_text(_ptr.get(), text_.begin(), text_.size());
   }

   void text_layout_x::font(elements::font_descr_x const& font_)
   {
      pango_layout_set_font_description(_ptr.get(), font_._ptr);
   }
}}


