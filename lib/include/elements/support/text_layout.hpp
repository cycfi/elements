/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_TEXT_LAYOUT_FEBRUARY_15_2020)
#define ELEMENTS_TEXT_LAYOUT_FEBRUARY_15_2020

#include <elements/support/font_descr.hpp>
#include <string_view>
#include <memory>

extern "C"
{
   struct _PangoLayout;
}

namespace cycfi { namespace elements
{
   class text_layout
   {
   public:
                           text_layout(canvas& cnv);
                           text_layout(text_layout const& rhs) = default;
                           ~text_layout()= default;
      text_layout&         operator=(text_layout const& rhs)= default;

      void                 text(std::string_view text_);
      void                 font(elements::font_descr const& font_);

   private:

      friend class canvas;
      using layout_ptr = std::shared_ptr<_PangoLayout>;

      layout_ptr           _ptr = nullptr;
   };
}}

#endif
