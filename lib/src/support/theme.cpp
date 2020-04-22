/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/support/theme.hpp>
#include <elements/view.hpp>

namespace cycfi { namespace elements
{
   using artist::rgba;
   using artist::font_descr;

   theme::theme()
    : panel_color               { rgba(28, 30, 34, 192) }
    , frame_color               { rgba(220, 220, 220, 80) }
    , frame_corner_radius       { 3.0 }
    , frame_stroke_width        { 1.0 }
    , scrollbar_color           { rgba(80, 80, 80, 80) }
    , default_button_color      { rgba(0, 0, 0, 0) }
    , button_margin             { 10, 5, 10, 5 }

    , controls_color            { rgba(18, 49, 85, 200) }
    , indicator_color           { rgba(0, 127, 255, 200) }
    , basic_font_color          { rgba(220, 220, 220, 200) }

// The symbols_font font is the OS supplied font that includes unicode symbols
// such as Miscellaneous Technical : Unicode U+2300 – U+23FF (8960–9215)
#if defined(__APPLE__)
    , system_font               { "Lucida Grande"  }
#elif defined(_WIN32)
    , system_font               { "Segoe UI Symbol" }
#elif defined(__linux__)
    , system_font               { "DejaVu Sans" }
#endif

    , box_widget_bg_opacity     { 32.0f / 255.0f }

    , heading_font_color        { basic_font_color }
    , heading_font              { font_descr{ "Roboto", 14 }.medium() }
    , heading_text_align        { canvas::middle | canvas::center }

    , label_font_color          { basic_font_color }
    , label_font                { font_descr{ "Open Sans", 14 } }
    , label_text_align          { canvas::middle | canvas::center }

    , icon_color                { basic_font_color }
    , icon_font                 { font_descr{ "elements_basic", 16 } }
    , icon_button_color         { default_button_color }

    , text_box_font_color       { basic_font_color }
    , text_box_font             { font_descr{ "Open Sans", 14 } }
    , text_box_hilite_color     { rgba(0, 127, 255, 100) }
    , text_box_caret_color      { rgba(0, 190, 255, 255) }
    , text_box_caret_width      { 1.2 }
    , inactive_font_color       { rgba(127, 127, 127, 150) }

    , ticks_color               { rgba(127, 127, 127, 150) }
    , major_ticks_level         { 0.5 }
    , major_ticks_width         { 1.5 }
    , minor_ticks_level         { 0.4 }
    , minor_ticks_width         { 0.7 }

    , major_grid_color          { frame_color }
    , major_grid_width          { 0.5 }
    , minor_grid_color          { indicator_color }
    , minor_grid_width          { 0.4 }

    , dialog_button_size        { 100 }
    , message_textbox_size      { { 300, 120 } }
   {
   }

   // The global theme
   theme global_theme::_theme;

   theme const& get_theme()
   {
      return global_theme::_theme;
   }

   void set_theme(theme const& thm)
   {
      global_theme::_theme = thm;
   }
}}
