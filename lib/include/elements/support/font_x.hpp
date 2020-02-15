/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_FONT_X_FEBRUARY_11_2020)
#define ELEMENTS_FONT_X_FEBRUARY_11_2020

extern "C"
{
   typedef struct _cairo_font_face cairo_font_face_t;
}

namespace cycfi { namespace elements
{
   class font_x
   {
   public:
                           font_x(char const* face);
                           font_x(font_x const& rhs);
                           ~font_x();
      font_x&                operator=(font_x const& rhs);

   private:

      friend class canvas;
      cairo_font_face_t*  _handle   = nullptr;
   };
}}

#endif
