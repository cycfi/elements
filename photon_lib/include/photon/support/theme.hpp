/*=============================================================================
   Copyright (c) 2016-2018 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(PHOTON_GUI_LIB_THEME_APRIL_15_2016)
#define PHOTON_GUI_LIB_THEME_APRIL_15_2016

#include <photon/support.hpp>

namespace photon
{
   class theme
   {
   public:

      color                panel_color                = { 28, 30, 34, 192 };
      color                frame_color                = { 220, 220, 220, 80 };
      float                frame_corner_radius        = 3.0;
      float                frame_stroke_width         = 1.0;
      color                scrollbar_color            = { 80, 80, 80, 80 };

      color                controls_color             = { 18, 49, 85, 200 };
      color                indicator_color            = { 0, 127, 255, 200 };
      color                basic_font_color           = { 220, 220, 220, 200 };

      color                heading_font_color         = basic_font_color;
      char const*          heading_font               = "Roboto";
      float                heading_font_size          = 16.0;
      canvas::font_style   heading_style              = canvas::normal;

      color                label_font_color           = basic_font_color;
      char const*          label_font                 = "Open Sans";
      float                label_font_size            = 14.0;
      canvas::font_style   label_style                = canvas::normal;

      color                icon_color                 = basic_font_color;
      char const*          icon_font                  = "photon_basic";
      float                icon_font_size             = 16.0;

      color                text_box_font_color        = basic_font_color;
      char const*          text_box_font              = "Open Sans";
      float                text_box_font_size         = 14.0;
      color                text_box_hilite_color      = { 0, 127, 255, 100 };
      color                text_box_caret_color       = { 0, 190, 255, 255 };
      float                text_box_caret_width       = 1.2;
      color                edit_box_fill_color        = { 63, 71, 77, 150 };
      color                inactive_font_color        = { 127, 127, 127, 150 };

      color                ticks_color                = { 127, 127, 127, 150 };
      float                major_ticks_level          = 0.5;
      float                major_ticks_width          = 1.5;
      float                minor_ticks_level          = 0.4;
      float                minor_ticks_width          = 0.7;

      color                major_grid_color           = frame_color;
      float                major_grid_width           = 0.5;
      color                minor_grid_color           = indicator_color;
      float                minor_grid_width           = 0.4;
   };

   // Access to the global theme
   theme const& get_theme();

   // Set the global theme
   void set_theme(theme const& thm);
}

#endif
