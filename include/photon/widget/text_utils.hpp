/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_TEXT_UTILS_MAY_22_2016)
#define PHOTON_GUI_LIB_TEXT_UTILS_MAY_22_2016

#include <photon/canvas.hpp>
#include <photon/theme.hpp>
#include <memory>

namespace photon
{
   class text_utils
   {
   public:
                           text_utils(theme const& theme_)
                            : _theme(theme_)
                           {}

      virtual              ~text_utils() {}

      class theme const&   theme() const { return _theme; }
      class canvas&        canvas() const { return _theme.canvas(); }

      void                 draw_label(rect bounds, char const* text) const;
      point                measure_label(char const* text) const;

      void                 draw_heading(rect bounds, char const* text) const;
      point                measure_heading(char const* text) const;

      void                 draw_icon(rect bounds, uint32_t code, int size) const;
      point                measure_icon(uint32_t code, int size) const;

      void                 draw_text_box(rect bounds, char const* text) const;
      void                 draw_edit_box_base(rect bounds) const;

      struct text_info
      {
         char const*       first;
         char const*       last;
      };

      struct text_draw_info
      {
         char const*       first;
         char const*       last;
         char const*       select_first;
         char const*       select_last;
         bool              is_focus;
         bool              is_active;
      };

      struct glyph_info
      {
         char const* str;
         float x;
         float y;
         float lineh;
         float minx;
         float maxx;

         rect bounds() const { return { minx, y, maxx, y+lineh }; }
      };

      void                 draw_edit_text_box(rect bounds, text_draw_info const& text) const;
      char const*          caret_position(rect bounds, text_info const& text, point p) const;
      glyph_info           glyph_bounds(rect bounds, text_info const& text, char const* cp) const;
      float                edit_text_box_height(rect bounds, text_info const& text) const;

   private:

      class theme const&   _theme;
   };
}

#endif
