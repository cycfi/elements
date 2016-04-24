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
#include <memory>

// forward
struct NVGcontext;

namespace photon
{
   class theme
   {
   public:

                           theme() : _vg(0) {}
      virtual              ~theme() {}

      void                 canvas(NVGcontext* vg_)   { _vg = vg_; }
      NVGcontext*          canvas() const { return _vg; }

      /////////////////////////////////////////////////////////////////////////////////////////////
      // Panels and Frames

      double               panel_corner_radius        = 3.0;
      color                panel_color                = { 28, 30, 34, 192 };
      rect                 panel_shadow_offset        = { -10, -10, +20, +30 };

      virtual void         draw_panel(rect const& b) const;

      double               frame_corner_radius        = 3.0;
      double               frame_stroke_width         = 0.8;
      color                frame_color                = { 220, 220, 220, 70 };

      virtual void         draw_frame(rect const& b) const;

      /////////////////////////////////////////////////////////////////////////////////////////////
      // Sliders and Scrollbars

      double               slider_knob_radius         = 0.25;  // fraction of size (width or height)
      double               slider_slot_size           = 0.2;   // fraction of size (width or height)
      color                slider_knob_fill_color     = { 40, 43, 48, 255 };
      color                slider_knob_outline_color  = { 0, 0, 0, 92 };

      virtual void         draw_slider(double pos, rect const& b) const;
      virtual void         draw_slider_knob(double pos, rect const& b) const;
      virtual circle       slider_knob_position(double pos, rect const& b) const;

      color                scroll_bar_outline_color   = { 200, 200, 200, 50 };
      color                scroll_bar_fill_color      = { 0, 0, 0, 120 };

      virtual void         draw_scroll_bar(double pos, double ext, rect const& b) const;

      /////////////////////////////////////////////////////////////////////////////////////////////
      // Text

      color                heading_font_color         = { 220, 220, 220, 160 };
      char const*          heading_font               = "sans-bold";
      double               heading_font_size          = 20.0;

      virtual void         draw_label(rect const& b, char const* text) const;
      virtual point        measure_label(char const* text) const;

      color                label_font_color           = { 220, 220, 220, 160 };
      char const*          label_font                 = "sans";
      double               label_font_size            = 18.0;

      virtual void         draw_heading(rect const& b, char const* text) const;
      virtual point        measure_heading(char const* text) const;

      color                icon_color                 = { 220, 220, 220, 160 };
      char const*          icon_font                  = "icons";

      virtual void         draw_icon(rect const& b, uint32_t code, int size) const;
      virtual point        measure_icon(uint32_t code, int size) const;

      color                text_box_font_color        = { 220, 220, 220, 230 };
      char const*          text_box_font              = "sans";
      double               text_box_font_size         = 18.0;
      color                text_box_hilite_color      = { 255, 192, 0, 80 }; // { 50, 110, 220, 90 };
      color                text_box_caret_color       = { 255, 192, 0, 255 };

      virtual void         draw_text_box(rect const& b, char const* text) const;

      struct text_info
      {
         char const*       first;
         char const*       last;
         char const*       select_first;
         char const*       select_last;
         bool              is_focus;
      };

      struct caret_info
      {
         char const*       pos;
         bool              before; // before or after glyph
      };

      virtual void         draw_edit_text_box(rect const& b, text_info const& text) const;
      virtual char const*  caret_position(rect const& b, text_info const& text, point const& p) const;

      /////////////////////////////////////////////////////////////////////////////////////////////
      // Buttons

      double            button_corner_radius        = 4.0;

      virtual void      draw_button(rect const& b, color const& button_color) const;

      /////////////////////////////////////////////////////////////////////////////////////////////
      // Fonts

      char const*       icons       = "icons";
      char const*       sans        = "sans";
      char const*       sans_bold   = "sans-bold";

      virtual void      load_fonts() const;

   private:

      NVGcontext*       _vg;
   };

   using theme_ptr = std::shared_ptr<theme>;
}

#endif
