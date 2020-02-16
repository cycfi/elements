/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_FONT_FEBRUARY_15_2020)
#define ELEMENTS_FONT_FEBRUARY_15_2020

extern "C"
{
   struct _PangoFontDescription;
}

namespace cycfi { namespace elements
{
   class canvas;

   class font_descr_x
   {
   public:
                           font_descr_x(char const* family);
                           font_descr_x(font_descr_x const& rhs);
                           font_descr_x(font_descr_x&& rhs);
                           ~font_descr_x();

      font_descr_x&          operator=(font_descr_x const& rhs);
      font_descr_x&          operator=(font_descr_x&& rhs);

      enum
      {
         thin        = 100,
         ultralight  = 200,
         light       = 300,
         semilight   = 350,
         book        = 380,
         normal      = 400,
         medium      = 500,
         semibold    = 600,
         bold        = 700,
         ultrabold   = 800,
         heavy       = 900,
         ultraheavy  = 1000
      };

      enum style_enum
      {
         style_normal,
         oblique,
         italic
      };

      enum stretch_enum
      {
         ultra_condensed,
         extra_condensed,
         condensed,
         semi_condensed,
         normal_stretch,
         semi_expanded,
         expanded,
         extra_expanded,
         ultra_expanded
      };

      font_descr_x           size(float pt) const;
      font_descr_x           weight(int w) const;
      font_descr_x           style(style_enum s) const;
      font_descr_x           stretch(stretch_enum s) const;
      font_descr_x           underline() const;
      font_descr_x           strikethrough() const;

   private:

      friend class canvas;
      friend class text_layout_x;
      using impl_ptr = _PangoFontDescription*;

      enum line { line_none, line_underline, line_strikethrough };

      impl_ptr             _ptr = nullptr;
      line                 _line = line_none;
   };
}}

#endif
