/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_THEME_APRIL_15_2016)
#define PHOTON_GUI_LIB_THEME_APRIL_15_2016

#include <photon/rect.hpp>
#include <photon/color.hpp>
#include <photon/circle.hpp>
#include <photon/canvas.hpp>
#include <memory>

namespace photon
{
   class theme
   {
   public:

                           theme() {}
      virtual              ~theme() {}

      void                 canvas(class canvas const& canvas_)    { _canvas = canvas_; }
      class canvas&        canvas() const                         { return _canvas; }
      virtual void         override()                             {}

      /////////////////////////////////////////////////////////////////////////////////////////////
      color                panel_color                = { 28, 30, 34, 192 };
      color                frame_color                = { 220, 220, 220, 70 };

      color                controls_color             = color{ 18, 49, 85, 200 };
      color                indicator_color            = color{ 0, 127, 255, 200 };

      /////////////////////////////////////////////////////////////////////////////////////////////
      // Text

      color                heading_font_color         = { 220, 220, 220, 160 };
      char const*          heading_font               = "sans-bold";
      float                heading_font_size          = 20.0;

      virtual void         draw_label(rect b, char const* text);
      virtual point        measure_label(char const* text);

      color                label_font_color           = { 220, 220, 220, 160 };
      char const*          label_font                 = "sans";
      float                label_font_size            = 18.0;

      virtual void         draw_heading(rect b, char const* text);
      virtual point        measure_heading(char const* text);

      color                icon_color                 = { 220, 220, 220, 160 };
      char const*          icon_font                  = "icons";

      virtual void         draw_icon(rect b, uint32_t code, int size);
      virtual point        measure_icon(uint32_t code, int size);

      color                text_box_font_color        = { 220, 220, 220, 230 };
      char const*          text_box_font              = "sans";
      float                text_box_font_size         = 18.0;
      color                text_box_hilite_color      = { 255, 192, 0, 80 };
      color                text_box_caret_color       = { 255, 192, 0, 255 };
      color                edit_box_fill_color        = { 32, 32, 32, 32 };
      color                inactive_font_color        = { 127, 127, 127, 150 };

      virtual void         draw_text_box(rect b, char const* text);
      virtual void         draw_edit_box_base(rect b);

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

      virtual void         draw_edit_text_box(rect b, text_draw_info const& text);
      virtual char const*  caret_position(rect b, text_info const& text, point p);
      virtual glyph_info   glyph_bounds(rect b, text_info const& text, char const* cp);
      virtual float        edit_text_box_height(rect b, text_info const& text);

      /////////////////////////////////////////////////////////////////////////////////////////////
      // Fonts

      char const*          icons       = "icons";
      char const*          sans        = "sans";
      char const*          sans_bold   = "sans-bold";

      virtual void         load_fonts();

   private:

      mutable class canvas _canvas;
   };

   using theme_ptr = std::shared_ptr<theme>;
}

#endif
