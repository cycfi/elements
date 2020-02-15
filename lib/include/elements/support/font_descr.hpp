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

   class font_descr
   {
   public:
                           font_descr(char const* descr);
                           font_descr(font_descr const& rhs);
                           font_descr(font_descr& rhs);
                           ~font_descr();

      font_descr&                operator=(font_descr const& rhs);
      font_descr&                operator=(font_descr&& rhs);

      void                 size(float pt);
      float                size() const;

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

      void                 weight(int w);
      int                  weight() const;

   private:

      friend class canvas;
      friend class text_layout;
      using font_descr_ptr = _PangoFontDescription*;

      font_descr_ptr       _ptr = nullptr;
   };
}}

#endif
