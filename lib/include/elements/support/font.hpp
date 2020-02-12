/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_FONTS_FEBRUARY_11_2020)
#define ELEMENTS_FONTS_FEBRUARY_11_2020

extern "C"
{
   typedef struct _cairo_scaled_font cairo_scaled_font_t;
}

namespace cycfi { namespace elements
{
   class font
   {
   public:
                           font(char const* face);
                           font(font const& rhs);
                           ~font();
      font&                operator=(font const& rhs);

   private:

      using scaled_font = cairo_scaled_font_t;
      friend class canvas;

      scaled_font*         _handle   = nullptr;
   };
}}

#endif
