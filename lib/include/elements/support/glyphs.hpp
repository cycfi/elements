/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_ELEMENTS_GUI_LIB_GLYPHS_SEPTEMBER_26_2016)
#define CYCFI_ELEMENTS_GUI_LIB_GLYPHS_SEPTEMBER_26_2016

#include <infra/assert.hpp>
#include <elements/support/canvas.hpp>
#include <elements/support/text_utils.hpp>
#include <vector>
#include <stdexcept>
#include <cairo.h>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // glyphs: Text drawing and measuring utility
   ////////////////////////////////////////////////////////////////////////////
   class master_glyphs;

   class glyphs
   {
   public:
                           glyphs(
                              char const* first, char const* last
                            , int glyph_start, int glyph_end
                            , int cluster_start, int cluster_end
                            , master_glyphs const& master
                            , bool strip_leading_spaces
                           );

      void                 draw(point pos, canvas& canvas_);
      float                width() const;

                           // for_each F signature:
                           // bool f(char const* utf8, float left, float right);
                           template <typename F>
      void                 for_each(F f);

      std::size_t          size() const      { return _last - _first; }
      char const*          begin() const     { return _first; }
      char const*          end() const       { return _last; }

      struct font_metrics
      {
         float             ascent;
         float             descent;
         float             leading;
      };

      font_metrics         metrics() const;

   protected:
                           glyphs(char const* first, char const* last);

      using scaled_font = cairo_scaled_font_t;
      using glyph = cairo_glyph_t;
      using cluster = cairo_text_cluster_t;
      using cluster_flags = cairo_text_cluster_flags_t;

      char const*          _first;
      char const*          _last;
      scaled_font*         _scaled_font   = nullptr;
      glyph*               _glyphs        = nullptr;
      int                  _glyph_count   = 0;
      cluster*             _clusters      = nullptr;
      int                  _cluster_count = 0;
      cluster_flags        _clusterflags;
   };

   ////////////////////////////////////////////////////////////////////////////
   struct failed_to_build_master_glyphs : std::runtime_error
   {
      failed_to_build_master_glyphs()
       : std::runtime_error("Error. Failed to build master glyphs.") {}
   };

   class master_glyphs : public glyphs
   {
   public:
                           master_glyphs(
                              char const* first, char const* last
                            , char const* face, float size
                           );

                           master_glyphs(
                              char const* first, char const* last
                            , master_glyphs const& source
                           );

                           master_glyphs(master_glyphs&&);
      master_glyphs&       operator=(master_glyphs&& rhs);

                           ~master_glyphs();

      void                 break_lines(float width, std::vector<glyphs>& lines);
      void                 text(char const* first, char const* last);

   private:
                           master_glyphs(master_glyphs const&) = delete;
      master_glyphs&       operator=(master_glyphs const& rhs) = delete;

      void                 build();
   };

   ////////////////////////////////////////////////////////////////////////////
   template <typename F>
   inline void glyphs::for_each(F f)
   {
      CYCFI_ASSERT(_scaled_font, "Precondition failure: _scaled_font must not be null");
      CYCFI_ASSERT(_glyphs, "Precondition failure: _glyphs must not be null");
      CYCFI_ASSERT(_clusters, "Precondition failure: _clusters must not be null");

      if (_first == _last)
         return;

      int   glyph_index = 0;
      int   byte_index = 0;
      float start_x = _glyphs->x;

      for (int i = 0; i < _cluster_count; i++)
      {
         cairo_text_cluster_t* cluster = _clusters + i;
         cairo_glyph_t* glyph = _glyphs + glyph_index;
         cairo_text_extents_t extents;
         cairo_scaled_font_glyph_extents(_scaled_font, glyph, 1, &extents);

         float x = glyph->x - start_x;
         if (!f(_first + byte_index, x, x + extents.x_advance))
            break;

         // glyph/byte position
         glyph_index += cluster->num_glyphs;
         byte_index += cluster->num_bytes;
      }
   }
}}

#endif
