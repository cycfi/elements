/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_FONTS_FEBRUARY_11_2020)
#define ELEMENTS_FONTS_FEBRUARY_11_2020

extern "C"
{
   typedef struct _cairo_font_face cairo_font_face_t;
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

      friend class canvas;
      cairo_font_face_t*  _handle   = nullptr;
   };
}}

#endif
