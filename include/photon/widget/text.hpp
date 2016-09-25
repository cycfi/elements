/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_LABEL_APRIL_17_2016)
#define PHOTON_GUI_LIB_WIDGET_LABEL_APRIL_17_2016

#include <photon/widget/widget.hpp>
#include <photon/widget/flow.hpp>
#include <string>
#include <vector>
#include <cairo.h>

namespace photon
{
   //class flowable_text : public flowable
   //{
   //public:
   //                              flowable_text(char const* first, char const* last);
   //                              flowable_text(flowable_text&& rhs);
   //                              ~flowable_text();
   //
   //   char const*                first() const     { return _first; }
   //   char const*                last() const      { return _last; }
   //   void                       text(char const* first, char const* last);
   //   void                       font(char const* face, float size, int style);
   //
   //   virtual float              width_of(size_t index, basic_context const& ctx) const;
   //   virtual widget_ptr         make_row(size_t first, size_t last);
   //
   //private:
   //
   //                              flowable_text(flowable_text const&) = delete;
   //   flowable_text&             operator=(flowable_text const&) = delete;
   //   void                       reset();
   //
   //   char const*                _first;
   //   char const*                _last;
   //
   //   cairo_matrix_t             _matrix;
   //   cairo_scaled_font_t*       _font;
   //   cairo_glyph_t*             _glyphs = nullptr;
   //   int                        _num_glyphs;
   //   cairo_text_cluster_t*      _clusters = nullptr;
   //   int                        _num_clusters;
   //   cairo_text_cluster_flags_t _cluster_flags;
   //
   //   std::vector<float>         _word_widths;
   //};



   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Text Box
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class text_box : public widget
   {
   public:
                              text_box(std::string const& text);
                              ~text_box();

      virtual void            draw(context const& ctx);

      std::string             text() const                     { return _text; }
      void                    text(std::string const& text);

      using widget::text;

   private:

      std::string             _text;
   };
}

#endif