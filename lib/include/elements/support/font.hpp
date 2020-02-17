/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_FONT_X_FEBRUARY_11_2020)
#define ELEMENTS_FONT_X_FEBRUARY_11_2020

#include <string_view>

extern "C"
{
   typedef struct _cairo_font_face cairo_font_face_t;
}

namespace cycfi { namespace elements
{
   struct font_descr
   {
      enum weight_enum
      {
	      thin              = 10,
	      extra_light       = 20,
	      light             = 30,
	      normal            = 40,
	      medium            = 50,
	      semi_bold         = 60,
	      bold              = 70,
	      extra_bold        = 80,
	      black             = 90,
	      extra_black       = 95,
      };

      enum slant_enum
      {
         slant_normal      = 0,
         italic            = 90,
         oblique           = 100
      };

      enum stretch_enum
      {
         ultra_condensed	= 25,
         extra_condensed	= 31,
         condensed	      = 38,
         semi_condensed	   = 44,
         stretch_normal	   = 50,
         semi_expanded	   = 57,
         expanded	         = 63,
         extra_expanded	   = 75,
         ultra_expanded	   = 100
      };

                           font_descr(std::string_view families_)
                            : families(families_)
                            , weight(normal)
                            , slant(slant_normal)
                            , stretch(stretch_normal)
                           {}

                           font_descr(
                              std::string_view families_
                            , weight_enum weight_
                           )
                            : families(families_)
                            , weight(weight_)
                            , slant(slant_normal)
                            , stretch(stretch_normal)
                           {}

                           font_descr(
                              std::string_view families_
                            , slant_enum slant_
                           )
                            : families(families_)
                            , weight(normal)
                            , slant(slant_)
                            , stretch(stretch_normal)
                           {}

                           font_descr(
                              std::string_view families_
                            , stretch_enum stretch_
                           )
                            : families(families_)
                            , weight(normal)
                            , slant(slant_normal)
                            , stretch(stretch_)
                           {}

                           font_descr(
                              std::string_view families_
                            , weight_enum weight_
                            , slant_enum slant_
                           )
                            : families(families_)
                            , weight(weight_)
                            , slant(slant_)
                            , stretch(stretch_normal)
                           {}

                           font_descr(
                              std::string_view families_
                            , weight_enum weight_
                            , stretch_enum stretch_
                           )
                            : families(families_)
                            , weight(weight_)
                            , slant(slant_normal)
                            , stretch(stretch_)
                           {}

                           font_descr(
                              std::string_view families_
                            , slant_enum slant_
                            , stretch_enum stretch_
                           )
                            : families(families_)
                            , weight(normal)
                            , slant(slant_)
                            , stretch(stretch_)
                           {}

                           font_descr(
                              std::string_view families_
                            , weight_enum weight_
                            , slant_enum slant_
                            , stretch_enum stretch_
                           )
                            : families(families_)
                            , weight(weight_)
                            , slant(slant_)
                            , stretch(stretch_)
                           {}

                           font_descr(
                              std::string_view families_
                            , int weight_
                            , int slant_ = slant_normal
                            , int stretch_ = stretch_normal
                           )
                            : families(families_)
                            , weight(normal)
                            , slant(slant_normal)
                            , stretch(stretch_)
                           {}

      std::string_view     families;
      uint8_t              weight;
      uint8_t              slant;
      uint8_t              stretch;
   };

   class font
   {
   public:
                           font(font_descr descr);
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
