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
   {
      rhs._scaled_font = nullptr;
      rhs._glyphs = nullptr;
      rhs._clusters = nullptr;
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

         rhs._scaled_font = nullptr;
         rhs._glyphs = nullptr;
         rhs._clusters = nullptr;
      }
      return *this;
   }

   glyphs::~glyphs()
   {
      if (_glyphs)
         cairo_glyph_free(_glyphs);
      if (_clusters)
         cairo_text_cluster_free(_clusters);
      if (_scaled_font)
         cairo_scaled_font_destroy(_scaled_font);
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

      cairo_set_scaled_font(cr, _scaled_font);
      cairo_translate(cr, pos.x - _glyphs->x, pos.y - _glyphs->y);

      cairo_show_text_glyphs(
         cr, _first, int(_last - _first),
         _glyphs, _glyph_count,
         _clusters, _cluster_count, _clusterflags
      );
   }
}
