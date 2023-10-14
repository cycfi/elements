/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/support/glyphs.hpp>
#include <elements/support/detail/scratch_context.hpp>

namespace cycfi { namespace elements
{
   static detail::scratch_context scratch_context_;

   glyphs::glyphs(char const* first, char const* last)
    : _first(first)
    , _last(last)
   {
      CYCFI_ASSERT(_first, "Precondition failure: _first must not be null");
      CYCFI_ASSERT(_last, "Precondition failure: _last must not be null");
   }

   glyphs::glyphs(
      char const* first, char const* last
    , int glyph_start, int glyph_end
    , int cluster_start, int cluster_end
    , master_glyphs const& master
    , bool strip_leading_spaces
   )
    : _first(first)
    , _last(last)
    , _scaled_font(master._scaled_font)
    , _glyphs(master._glyphs + glyph_start)
    , _glyph_count(glyph_end - glyph_start)
    , _clusters(master._clusters + cluster_start)
    , _cluster_count(cluster_end - cluster_start)
    , _clusterflags(master._clusterflags)
   {
      CYCFI_ASSERT(_first, "Precondition failure: _first must not be null");
      CYCFI_ASSERT(_last, "Precondition failure: _last must not be null");
      CYCFI_ASSERT(_scaled_font, "Precondition failure: _scaled_font must not be null");
      CYCFI_ASSERT(_glyphs, "Precondition failure: _glyphs must not be null");
      CYCFI_ASSERT(_clusters, "Precondition failure: _clusters must not be null");

      // We strip leading spaces until after the last leading newline.
      // Examples:
      //
      //    "\n\n  " ===>  "  "
      //    " \n\n"  ===>  ""
      //    "   xxx" ===>  "xxx"

      auto  strip_leading = [this](auto f)
      {
         int      glyph_index = 0;
         unsigned codepoint;
         unsigned state = 0;

         cairo_text_cluster_t* cluster = _clusters;
         for (auto i = _first; i != _last; ++i)
         {
            if (!decode_utf8(state, codepoint, uint8_t(*i)))
            {
               if (!f(codepoint))
                  break;
               glyph_index += cluster->num_glyphs;
               ++cluster;
            }
         }

         auto   clusters_skipped = int(cluster - _clusters);

         _glyph_count -= glyph_index;
         _glyphs += glyph_index;
         _cluster_count -= clusters_skipped;
         _clusters = cluster;
         _first += clusters_skipped;
      };

      if (strip_leading_spaces)
         strip_leading([](auto cp){ return !is_newline(cp) && is_space(cp); });
      strip_leading([](auto cp){ return is_newline(cp); });
   }

   void glyphs::draw(point pos, canvas& canvas_)
   {
      // return early if there's nothing to draw
      if (_first == _last)
         return;

      CYCFI_ASSERT(_scaled_font, "Precondition failure: _scaled_font must not be null");
      CYCFI_ASSERT(_glyphs, "Precondition failure: _glyphs must not be null");
      CYCFI_ASSERT(_clusters, "Precondition failure: _clusters must not be null");

      auto cr = &canvas_.cairo_context();
      auto state = canvas_.new_state();

      cairo_set_scaled_font(cr, _scaled_font);
      cairo_translate(cr, pos.x - _glyphs->x, pos.y - _glyphs->y);
      canvas_.apply_fill_style();

      cairo_show_text_glyphs(
         cr, _first, int(_last - _first),
         _glyphs, _glyph_count,
         _clusters, _cluster_count, _clusterflags
      );
   }

   float glyphs::width() const
   {
      if (_first == _last)
         return 0;

      CYCFI_ASSERT(_scaled_font, "Precondition failure: _scaled_font must not be null");
      CYCFI_ASSERT(_glyphs, "Precondition failure: _glyphs must not be null");
      CYCFI_ASSERT(_clusters, "Precondition failure: _clusters must not be null");

      if (_glyph_count)
      {
         cairo_text_extents_t extents;
         auto glyph = _glyphs + _glyph_count -1;
         cairo_scaled_font_glyph_extents(_scaled_font, glyph, 1, &extents);
         return (glyph->x + extents.x_advance) - _glyphs->x;
      }
      return 0;
   }

   glyphs::font_metrics glyphs::metrics() const
   {
      cairo_font_extents_t font_extents;
      cairo_scaled_font_extents(_scaled_font, &font_extents);

      return {
         /*ascent=*/    float(font_extents.ascent),
         /*descent=*/   float(font_extents.descent),
         /*leading=*/   float(font_extents.height-(font_extents.ascent + font_extents.descent)),
      };
   }

   ////////////////////////////////////////////////////////////////////////////
   master_glyphs::master_glyphs(
       char const* first, char const* last
     , font font_, float size
     , point start
   )
    : glyphs(first, last)
   {
      canvas cnv{ *scratch_context_.context() };
      cnv.font(font_, size);
      auto cr = scratch_context_.context();
      _scaled_font = cairo_scaled_font_reference(cairo_get_scaled_font(cr));
      build(start);
   }

   master_glyphs::master_glyphs(
      char const* first
    , char const* last
    , master_glyphs const& source
    , point start
   )
    : glyphs(first, last)
   {
      canvas cnv{ *scratch_context_.context() };
      _scaled_font = cairo_scaled_font_reference(source._scaled_font);
      build(start);
   }

   master_glyphs::master_glyphs(master_glyphs&& rhs)
    : glyphs(rhs._first, rhs._last)
   {
      _scaled_font = rhs._scaled_font;
      _glyphs = rhs._glyphs;
      _glyph_count = rhs._glyph_count;
      _clusters = rhs._clusters;
      _cluster_count = rhs._cluster_count;
      _clusterflags = rhs._clusterflags;

      rhs._glyphs = nullptr;
      rhs._clusters = nullptr;
      rhs._scaled_font = nullptr;
   }

   master_glyphs& master_glyphs::operator=(master_glyphs&& rhs)
   {
      if (&rhs != this)
      {
         _first = rhs._first;
         _last = rhs._last;
         _scaled_font = rhs._scaled_font;
         _glyphs = rhs._glyphs;
         _glyph_count = rhs._glyph_count;
         _clusters = rhs._clusters;
         _cluster_count = rhs._cluster_count;
         _clusterflags = rhs._clusterflags;

         rhs._glyphs = nullptr;
         rhs._clusters = nullptr;
         rhs._scaled_font = nullptr;
      }
      return *this;
   }

   master_glyphs::~master_glyphs()
   {
      if (_glyphs)
         cairo_glyph_free(_glyphs);
      if (_clusters)
         cairo_text_cluster_free(_clusters);
      if (_scaled_font)
         cairo_scaled_font_destroy(_scaled_font);

      _glyphs = nullptr;
      _clusters = nullptr;
      _scaled_font = nullptr;
   }

   void master_glyphs::text(char const* first, char const* last, point start)
   {
      if (_glyphs)
      {
         cairo_glyph_free(_glyphs);
         _glyphs = nullptr;
      }
      if (_clusters)
      {
         cairo_text_cluster_free(_clusters);
         _clusters = nullptr;
      }

      _first = first;
      _last = last;
      build(start);
   }

   void master_glyphs::break_lines(float width, std::vector<glyphs>& lines)
   {
      CYCFI_ASSERT(_scaled_font, "Precondition failure: _scaled_font must not be null");

      // reurn early if there's nothing to break
      if (_first == _last)
         return;

      CYCFI_ASSERT(_glyphs, "Precondition failure: _glyphs must not be null");
      CYCFI_ASSERT(_clusters, "Precondition failure: _clusters must not be null");

      char const* first = _first;
      char const* last = _last;
      char const* space_pos = _first;
      int         start_glyph_index = 0;
      int         start_cluster_index = 0;
      int         space_glyph_index = 0;
      int         space_cluster_index = 0;
      float       start_x = _glyphs->x;

      auto add_line = [&]()
      {
         glyphs glyph_{
            first, space_pos
          , start_glyph_index, space_glyph_index
          , start_cluster_index, space_cluster_index
          , *this
          , lines.size() > 0 // skip leading spaces if this is not the first line
         };
         lines.push_back(std::move(glyph_));
         first = space_pos;
         start_glyph_index = space_glyph_index;
         start_cluster_index = space_cluster_index;
         start_x = _glyphs[space_glyph_index].x;
      };

      int      glyph_index = 0;
      unsigned codepoint;
      unsigned state = 0;

      cairo_text_cluster_t* cluster = _clusters;
      for (auto i = _first; i != _last; ++i)
      {
         if (!decode_utf8(state, codepoint, uint8_t(*i)))
         {
            cairo_glyph_t*  glyph = _glyphs + glyph_index;

            // Check if we exceeded the line width:
            cairo_text_extents_t extents;
            cairo_scaled_font_glyph_extents(_scaled_font, glyph, 1, &extents);
            if (((glyph->x + extents.x_advance) - start_x) > width)
            {
               // Add the line if we did (exceed the line width)
               add_line();
            }

            // Did we have a space?
            else if (is_space(codepoint))
            {
               // Mark the spaces for later
               space_glyph_index = glyph_index;
               space_cluster_index = int(cluster - _clusters);
               space_pos = i;

               // If we got an explicit new line, add the line right away.
               if ((space_glyph_index != start_glyph_index) && is_newline(codepoint))
                  add_line();
            }

            glyph_index += cluster->num_glyphs;
            ++cluster;
         }
      }

      glyphs glyph_{
         first, last
       , start_glyph_index, _glyph_count
       , start_cluster_index, _cluster_count
       , *this
       , lines.size() > 1 // skip leading spaces if this is not the first line
      };

      lines.push_back(std::move(glyph_));
   }

   void master_glyphs::build(point start)
   {
      // reurn early if there's nothing to build
      if (_first == _last)
         return;

      auto stat = cairo_scaled_font_text_to_glyphs(
         _scaled_font, start.x, start.y, _first, int(_last - _first),
         &_glyphs, &_glyph_count, &_clusters, &_cluster_count,
         &_clusterflags);

      if (stat != CAIRO_STATUS_SUCCESS)
      {
         _glyphs = nullptr;
         _clusters = nullptr;
         throw failed_to_build_master_glyphs{};
      }
   }
}}
