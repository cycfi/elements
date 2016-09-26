/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/support/glyphs.hpp>
#include <photon/support/detail/scratch_context.hpp>

namespace photon
{
   static detail::scratch_context scratch_context_;

   glyphs::glyphs(
       char const* first, char const* last
     , char const* face, float size, int style
   )
    : _first(first)
    , _last(last)
   {
      canvas cnv{ *scratch_context_.context() };
      cnv.font(face, size, style);
      auto cr = scratch_context_.context();
      _scaled_font = cairo_scaled_font_reference(cairo_get_scaled_font(cr));
      build();

   }

   glyphs::glyphs(char const* first, char const* last, glyphs const& source)
    : _first(first)
    , _last(last)
   {
      canvas cnv{ *scratch_context_.context() };
      _scaled_font = cairo_scaled_font_reference(source._scaled_font);
      build();
   }

   glyphs::glyphs(glyphs&& rhs)
    : _first(rhs._first)
    , _last(rhs._last)
    , _scaled_font(rhs._scaled_font)
    , _glyphs(rhs._glyphs)
    , _glyph_count(rhs._glyph_count)
    , _clusters(rhs._clusters)
    , _cluster_count(rhs._cluster_count)
    , _clusterflags(rhs._clusterflags)
    , _owns(rhs._owns)
   {
      rhs._owns = false;
   }

   glyphs& glyphs::operator=(glyphs&& rhs)
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
         _owns = rhs._owns;
         rhs._owns = false;
      }
      return *this;
   }

   glyphs::glyphs(
      char const* first, char const* last
    , int glyph_start, int glyph_end
    , int cluster_start, int cluster_end
    , glyphs const& source
   )
    : _first(first)
    , _last(last)
    , _scaled_font(source._scaled_font)
    , _glyphs(source._glyphs + glyph_start)
    , _glyph_count(glyph_end - glyph_start)
    , _clusters(source._clusters + cluster_start)
    , _cluster_count(cluster_end - cluster_start)
    , _clusterflags(source._clusterflags)
    , _owns(false)
   {
   }

   glyphs::~glyphs()
   {
      if (_owns)
      {
         if (_glyphs)
            cairo_glyph_free(_glyphs);
         if (_clusters)
            cairo_text_cluster_free(_clusters);
         if (_scaled_font)
            cairo_scaled_font_destroy(_scaled_font);
      }
   }

   void glyphs::build()
   {
      auto stat = cairo_scaled_font_text_to_glyphs(
         _scaled_font, 0, 0, _first, int(_last - _first),
         &_glyphs, &_glyph_count, &_clusters, &_cluster_count,
         &_clusterflags);

      if (stat != CAIRO_STATUS_SUCCESS)
      {
         // $$$ JDG: Throw? $$$
         _glyphs = nullptr;
         _clusters = nullptr;
      }
   }

   void glyphs::draw(point pos, canvas& canvas_)
   {
      auto cr = &canvas_.cairo_context();
      auto state = canvas_.new_state();

      cairo_set_scaled_font(cr, _scaled_font);
      cairo_translate(cr, pos.x - _glyphs->x, pos.y - _glyphs->y);

      cairo_show_text_glyphs(
         cr, _first, int(_last - _first),
         _glyphs, _glyph_count,
         _clusters, _cluster_count, _clusterflags
      );
   }

   void glyphs::break_lines(float width, std::vector<glyphs>& lines)
   {
      char const* first = _first;
      char const* last = _last;
      char const* pivot = _first;
      int         start_glyph_index = 0;
      int         start_cluster_index = 0;
      int         got_glyph_index = 0;
      int         got_cluster_index = 0;
      float       start_x = _glyphs->x;

      auto add_line = [&]()
      {
         glyphs glyph_{
            first, pivot
          , start_glyph_index, got_glyph_index
          , start_cluster_index, got_cluster_index, *this
         };
         lines.push_back(std::move(glyph_));
         first = pivot;
         start_glyph_index = got_glyph_index;
         start_cluster_index = got_cluster_index;
         start_x = _glyphs[got_glyph_index].x;
      };

      int   glyph_index = 0;
      cairo_text_cluster_t* cluster = _clusters;
      for (auto i = _first; i != _last; ++i)
      {
         unsigned    codepoint;
         unsigned    state = 0;

         if (!decode_utf8(state, codepoint, uint8_t(*i)))
         {
            cairo_glyph_t*  glyph = _glyphs + glyph_index;

            if ((glyph->x - start_x) > width)
               add_line();

            else if (is_space(codepoint))
            {
               got_glyph_index = glyph_index;
               got_cluster_index = int(cluster - _clusters);
               pivot = i;
               if ((got_glyph_index != start_glyph_index) && is_newline(codepoint))
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
      };

      lines.push_back(std::move(glyph_));
   }

   float glyphs::width() const
   {
      if (_glyph_count)
      {
         cairo_text_extents_t extents;
         auto glyph = _glyphs + _glyph_count -1;
         cairo_scaled_font_glyph_extents(_scaled_font, glyph, 1, &extents);
         return (glyph->x + extents.x_advance) - _glyphs->x;
      }
      return 0;
   }
}
