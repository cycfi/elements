/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/text.hpp>
#include <photon/support/theme.hpp>
#include <photon/support/context.hpp>

namespace photon
{
   class glyphs
   {
   public:
                        glyphs(
                            char const* first
                          , char const* last
                          , char const* face
                          , float       size
                          , int         style = canvas::normal
                        );

                        ~glyphs();

      void              draw(point pos, context const& ctx);

                        template <typename F>
      void              for_each(F f);

   private:
                        glyphs(glyphs const&) = delete;

      using glyph_type = cairo_glyph_t;
      using cluster_type = cairo_text_cluster_t;
      using cluster_flags = cairo_text_cluster_flags_t;

      char const*       _first;
      char const*       _last;
      char const*       _face;
      float             _size;
      int               _style;

      glyph_type*       _glyphs = nullptr;
      int               _glyph_count;
      cluster_type*     _clusters = nullptr;
      int               _cluster_count;
      cluster_flags     _clusterflags;
   };

   namespace
   {
      class scratch_context
      {
      public:

         scratch_context()
         {
            _surface = cairo_recording_surface_create(CAIRO_CONTENT_COLOR_ALPHA, nullptr);
            _context = cairo_create(_surface);
         }

         ~scratch_context()
         {
            cairo_surface_destroy(_surface);
            cairo_destroy(_context);
         }

         cairo_t*          context() const { return _context; }

      private:

         scratch_context(scratch_context const&) = delete;

         cairo_surface_t*  _surface;
         cairo_t*          _context;
      };

      scratch_context scratch_context_;

      // Copyright (c) 2008-2010 Bjoern Hoehrmann <bjoern@hoehrmann.de>
      // See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.

      int const utf8_accept = 0;
      int const utf8_reject = 12;

      static const uint8_t utf8d[] =
      {
         // The first part of the table maps bytes to character classes that
         // to reduce the size of the transition table and create bitmasks.
         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
         1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
         7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
         8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
        10,3,3,3,3,3,3,3,3,3,3,3,3,4,3,3, 11,6,6,6,5,8,8,8,8,8,8,8,8,8,8,8,

         // The second part is a transition table that maps a combination
         // of a state of the automaton and a character class to a state.
         0,12,24,36,60,96,84,12,12,12,48,72, 12,12,12,12,12,12,12,12,12,12,12,12,
        12, 0,12,12,12,12,12, 0,12, 0,12,12, 12,24,12,12,12,12,12,24,12,24,12,12,
        12,12,12,12,12,12,12,24,12,12,12,12, 12,24,12,12,12,12,12,12,12,24,12,12,
        12,12,12,12,12,12,12,36,12,36,12,12, 12,36,12,12,12,12,12,36,12,36,12,12,
        12,36,12,12,12,12,12,12,12,12,12,12,
      };

      inline uint32_t decode_utf8(uint32_t& state, uint32_t& codep, uint32_t byte)
      {
         uint32_t type = utf8d[byte];

         codep = (state != utf8_accept) ?
            (byte & 0x3fu) | (codep << 6) :
            (0xff >> type) & (byte)
            ;

         state = utf8d[256 + state + type];
         return state;
      }
   }

   glyphs::glyphs(
       char const* first, char const* last
     , char const* face, float size, int style
   )
    : _first(first)
    , _last(last)
    , _face(face)
    , _size(size)
    , _style(style)
   {
      canvas cnv{ *scratch_context_.context() };
      cnv.font(face, size, style);
      auto scaled_font = cairo_get_scaled_font(scratch_context_.context());

      auto stat = cairo_scaled_font_text_to_glyphs(
         scaled_font, 0, 0, _first, int(_last - _first),
         &_glyphs, &_glyph_count, &_clusters, &_cluster_count,
         &_clusterflags);

      if (stat != CAIRO_STATUS_SUCCESS)
      {
         // $$$ JDG: Throw? $$$
         _glyphs = nullptr;
         _clusters = nullptr;
      }
   }

   glyphs::~glyphs()
   {
      cairo_glyph_free(_glyphs);
      cairo_text_cluster_free(_clusters);
   }

   template <typename F>
   void glyphs::for_each(F f)
   {
      uint32_t  codepoint;
      uint32_t  state = 0;
      int       glyph_index = 0;
      float     start_x = _glyphs->x;
      float     start_y = _glyphs->y;

      cairo_text_cluster_t* cluster = _clusters;
      for (auto i = _first; i != _last; ++i)
      {
         if (!decode_utf8(state, codepoint, uint8_t(*i)))
         {
            cairo_glyph_t*  glyph = _glyphs + glyph_index;
            f(codepoint, point{ float(glyph->x - start_x), float(glyph->y - start_y)});
            glyph_index += cluster->num_glyphs;
            ++cluster;
         }
      }

      //int glyph_index = 0;
      //int byte_index = 0;
      //
      //for (int i = 0; i < _cluster_count; i++)
      //{
      //   cairo_text_cluster_t* cluster = _clusters + i;
      //   glyph_index += cluster->num_glyphs;
      //   byte_index += cluster->num_bytes;
      //}
   }

   void glyphs::draw(point pos, context const& ctx)
   {
      auto& cnv = ctx.canvas;
      auto cr = &cnv.cairo_context();

      ctx.canvas.font(_face, _size, _style);
      cairo_translate(cr, pos.x - _glyphs->x, pos.y - _glyphs->y);

      cairo_show_text_glyphs(
         cr, _first, int(_last - _first),
         _glyphs, _glyph_count,
         _clusters, _cluster_count, _clusterflags
      );
   }

   text_box::text_box(std::string const& text)
    : _text(text)
   {
   }

   text_box::~text_box()
   {
   }

   void text_box::draw(context const& ctx)
   {
      auto& cnv = ctx.canvas;
      glyphs g{ _text.data(), _text.data() + _text.size(), "Open Sans", 32, cnv.bold };
      g.draw({ 0, 32 }, ctx);

      g.for_each(
         [](uint32_t codepoint, point pos)
         {
         }
      );


   /*
      auto& cnv = ctx.canvas;
      auto  cr = &cnv.cairo_context();

      int fontSize = 32;

      cnv.font("Open Sans", fontSize, cnv.bold);

      //cnv.stroke_style(colors::white);
      //cnv.stroke_rect(ctx.bounds);

      //cairo_font_face_t* fontFace = cairo_get_font_face (cr);


      //cairo_set_font_face(cr, fontFace);
      //cairo_set_font_size(cr, fontSize);
      auto scaled_face = cairo_get_scaled_font(cr);

      // get glyphs for the text
      cairo_glyph_t* glyphs = NULL;
      int glyph_count;
      cairo_text_cluster_t* clusters = NULL;
      int cluster_count;
      cairo_text_cluster_flags_t clusterflags;

      auto stat = cairo_scaled_font_text_to_glyphs(
         scaled_face, 0, 0, _text.data(), int(_text.size()),
         &glyphs, &glyph_count, &clusters, &cluster_count,
         &clusterflags);

      // check if conversion was successful
      if (stat == CAIRO_STATUS_SUCCESS)
      {
          // text paints on bottom line
          cairo_translate(cr, 0, fontSize);

         //int start = 0;
         //cairo_show_text_glyphs(cr,
         //               _text.data()+start, _text.size()-start,
         //               glyphs+start, glyph_count-start,
         //               clusters+start, cluster_count-start, clusterflags);


         //cairo_show_text_glyphs(cr,
         //               _text.data(), _text.size(),
         //               glyphs, glyph_count,
         //               clusters, cluster_count, clusterflags);



          // draw each cluster
          int glyph_index = 0;
          int byte_index = 0;

          for (int i = 0; i < cluster_count; i++)
          {
              cairo_text_cluster_t* cluster = clusters + i;
              cairo_glyph_t* clusterglyphs = glyphs + glyph_index;

              // get extents for the glyphs in the cluster
              cairo_text_extents_t extents;
              cairo_scaled_font_glyph_extents(scaled_face, clusterglyphs, cluster->num_glyphs, &extents);
              // ... for later use

              // put paths for current cluster to context
              cairo_glyph_path(cr, clusterglyphs, cluster->num_glyphs);

              cairo_fill(cr);

              // glyph/byte position
              glyph_index += cluster->num_glyphs;
              byte_index += cluster->num_bytes;
          }

      }

      */

   }

   void text_box::text(std::string const& text)
   {
      _text = text;
   }




//   flowable_text::flowable_text(char const* first, char const* last)
//   {
//      auto& thm = get_theme();
//      cairo_matrix_init_identity(_matrix);
//      _font = cairo_scaled_font_create(
//         thm.text_box_font
//      );
//      text(first, last);
//   }
//
//   flowable_text(flowable_text&& rhs)
//    : _font(rhs._font)
//    , _glyphs(rhs._glyphs)
//    , _num_glyphs(rhs._num_glyphs)
//    , _clusters(rhs._clusters)
//    , _num_clusters(rhs._num_clusters)
//    , _cluster_flags(rhs._cluster_flags)
//    , _word_widths(std::move(rhs._word_widths))
//   {
//      rhs._font = nullptr;
//      rhs._glyphs = nullptr;
//      rhs._clusters = nullptr;
//   }
//
//   flowable_text::~flowable_text()
//   {
//      reset();
//   }
//
//   void flowable_text::text(char const* first, char const* last)
//   {
//      auto status = cairo_scaled_font_text_to_glyphs(
//         scaled_font,
//         0, 0, // x, y
//         first, last-first,
//         &_glyphs, &_num_glyphs,
//         &_clusters, &_num_clusters, &_cluster_flags
//      );
//   }
//
//   void flowable_text::font(char const* face, float size, int style)
//   {
//
//   }
//
//   float flowable_text::width_of(size_t index, basic_context const& ctx) const
//   {
//
//   }
//
//   widget_ptr flowable_text::make_row(size_t first, size_t last)
//   {
//
//   }
//
//   void flowable_text::reset()
//   {
//      if (_glyphs)
//         cairo_glyph_free (_glyphs);
//      if (_clusters)
//         cairo_text_cluster_free(_clusters);
//   }
}
