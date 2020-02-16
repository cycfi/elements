/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_TEXT_LAYOUT_FEBRUARY_15_2020)
#define ELEMENTS_TEXT_LAYOUT_FEBRUARY_15_2020

#include <elements/support/font_descr_x.hpp>
#include <string_view>
#include <memory>

extern "C"
{
   struct _PangoLayout;
}

namespace cycfi { namespace elements
{
   class text_layout_x
   {
   public:
                           text_layout_x(canvas& cnv);
                           text_layout_x(text_layout_x const& rhs) = default;
                           ~text_layout_x()= default;
      text_layout_x&         operator=(text_layout_x const& rhs)= default;

      void                 text(std::string_view text_);
      void                 font(elements::font_descr_x const& font_);

   private:

      friend class canvas;
      using layout_ptr = std::shared_ptr<_PangoLayout>;

      layout_ptr           _ptr = nullptr;
   };
}}

#endif
