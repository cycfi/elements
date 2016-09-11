/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_THEME_APRIL_15_2016)
#define PHOTON_GUI_LIB_THEME_APRIL_15_2016

#include <photon/support.hpp>

namespace photon
{
   class theme
   {
   public:

      color                panel_color                = { 28, 30, 34, 192 };
      color                frame_color                = { 220, 220, 220, 100 };
      float                frame_corner_radius        = 3.0;
      float                frame_stroke_width         = 1.5;

      color                controls_color             = { 18, 49, 85, 200 };
      color                indicator_color            = { 0, 127, 255, 200 };
      color                basic_font_color           = { 220, 220, 220, 200 };

      color                heading_font_color         = basic_font_color;
      char const*          heading_font               = "BlairMdITC TT Medium";
      float                heading_font_size          = 18.0;
      canvas::font_style   heading_style              = canvas::bold;

      color                label_font_color           = basic_font_color;
      char const*          label_font                 = "Roboto Regular";
      float                label_font_size            = 18.0;
      canvas::font_style   label_style                = canvas::normal;

      color                icon_color                 = basic_font_color;
      char const*          icon_font                  = "photon_basic";
      float                icon_font_size             = 18.0;

      color                text_box_font_color        = basic_font_color;
      char const*          text_box_font              = "Open Sans";
      float                text_box_font_size         = 18.0;
      color                text_box_hilite_color      = { 0, 127, 255, 100 };
      color                text_box_caret_color       = { 0, 190, 255, 255 };
      color                edit_box_fill_color        = { 32, 32, 32, 32 };
      color                inactive_font_color        = { 127, 127, 127, 150 };

      color                major_grid_color           = frame_color;
      float                major_grid_width           = 1.5;
      color                minor_grid_color           = indicator_color;
      float                minor_grid_width           = 0.4;
   };

   // Access to the global theme
   theme const& get_theme();

   // Set the global theme
   void set_theme(theme const& thm);
}

#endif
