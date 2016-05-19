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

      /////////////////////////////////////////////////////////////////////////////////////////////
      // Panels and Frames

      float                panel_corner_radius        = 3.0;
      color                panel_color                = { 28, 30, 34, 192 };
      rect                 panel_shadow_offset        = { -10, -10, +20, +30 };

      virtual void         draw_panel(rect b) const;

      float                frame_corner_radius        = 3.0;
      float                frame_stroke_width         = 0.8;
      color                frame_color                = { 220, 220, 220, 70 };

      virtual void         draw_frame(rect b) const;
      virtual void         draw_title_bar(rect b) const;

      /////////////////////////////////////////////////////////////////////////////////////////////
      // Knobs, Sliders and Scrollbars

      color                knob_fill_color            = { 60, 60, 60, 50 };
      color                knob_outline_color         = { 20, 20, 20, 92 };
      color                knob_indicator_color       = { 200, 200, 200, 128 };

      virtual point        draw_knob(float pos, rect b) const;
      virtual bool         knob_hit_test(rect b, point p) const;

      float                slider_knob_radius         = 0.25;  // fraction of size (width or height)
      float                slider_slot_size           = 0.15;  // fraction of size (width or height)
      color                slider_knob_fill_color     = { 60, 60, 60, 255 };
      color                slider_knob_outline_color  = { 0, 0, 0, 92 };

      virtual void         draw_slider(float pos, rect b) const;
      virtual void         draw_slider_knob(float pos, rect b) const;
      virtual circle       slider_knob_position(float pos, rect b) const;

      color                scroll_bar_outline_color   = { 200, 200, 200, 50 };
      color                scroll_bar_fill_color      = { 0, 0, 0, 120 };
      float                scroll_bar_width           = 10;

      virtual void         draw_scroll_bar(float pos, float ext, rect b, point mp) const;
      virtual rect         scroll_bar_position(float pos, float ext, rect b) const;

      /////////////////////////////////////////////////////////////////////////////////////////////
      // Text

      color                heading_font_color         = { 220, 220, 220, 160 };
      char const*          heading_font               = "sans-bold";
      float                heading_font_size          = 20.0;

      virtual void         draw_label(rect b, char const* text) const;
      virtual point        measure_label(char const* text) const;

      color                label_font_color           = { 220, 220, 220, 160 };
      char const*          label_font                 = "sans";
      float                label_font_size            = 18.0;

      virtual void         draw_heading(rect b, char const* text) const;
      virtual point        measure_heading(char const* text) const;

      color                icon_color                 = { 220, 220, 220, 160 };
      char const*          icon_font                  = "icons";

      virtual void         draw_icon(rect b, uint32_t code, int size) const;
      virtual point        measure_icon(uint32_t code, int size) const;

      color                text_box_font_color        = { 220, 220, 220, 230 };
      char const*          text_box_font              = "sans";
      float                text_box_font_size         = 18.0;
      color                text_box_hilite_color      = { 255, 192, 0, 80 };
      color                text_box_caret_color       = { 255, 192, 0, 255 };
      color                edit_box_fill_color        = { 32, 32, 32, 32 };
      color                inactive_font_color        = { 127, 127, 127, 150 };

      virtual void         draw_text_box(rect b, char const* text) const;
      virtual void         draw_edit_box_base(rect b) const;

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

      virtual void         draw_edit_text_box(rect b, text_draw_info const& text) const;
      virtual char const*  caret_position(rect b, text_info const& text, point p) const;
      virtual glyph_info   glyph_bounds(rect b, text_info const& text, char const* cp) const;
      virtual float        edit_text_box_height(rect b, text_info const& text) const;

      /////////////////////////////////////////////////////////////////////////////////////////////
      // Buttons

      float                button_corner_radius        = 4.0;

      virtual void         draw_button(rect b, color button_color) const ;

      /////////////////////////////////////////////////////////////////////////////////////////////
      // Fonts

      char const*          icons       = "icons";
      char const*          sans        = "sans";
      char const*          sans_bold   = "sans-bold";

      virtual void         load_fonts() const;

   private:

      mutable class canvas _canvas;
   };

   using theme_ptr = std::shared_ptr<theme>;
}

#endif
