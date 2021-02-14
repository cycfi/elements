/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/support/theme.hpp>
#include <elements/element/dial.hpp>
#include <elements/view.hpp>

namespace cycfi { namespace elements
{
   theme::theme()
    : panel_color                { rgba(28, 30, 34, 192) }
    , frame_color                { rgba(220, 220, 220, 80) }
    , frame_hilite_color         { rgba(220, 220, 220, 160) }
    , frame_corner_radius        { 3.0 }
    , frame_stroke_width         { 1.0 }
    , scrollbar_color            { rgba(80, 80, 80, 80) }
    , default_button_color       { rgba(0, 0, 0, 0) }
    , button_margin              { 10, 5, 10, 5 }

    , controls_color             { rgba(18, 49, 85, 200) }
    , controls_frame_stroke_width{ 1.5 }
    , indicator_color            { rgba(0, 127, 255, 200) }
    , indicator_bright_color     { indicator_color.level(1.5) }
    , indicator_hilite_color     { indicator_color.level(2.0) }
    , basic_font_color           { rgba(220, 220, 220, 200) }

// The symbols_font font is the OS supplied font that includes unicode symbols
// such as Miscellaneous Technical : Unicode U+2300 – U+23FF (8960–9215)
#if defined(__APPLE__)
    , system_font                { font_descr{"Lucida Grande" } }
#elif defined(_WIN32)
    , system_font                { font_descr{ "Segoe UI Symbol" } }
#elif defined(__linux__)
    , system_font                { font_descr{ "DejaVu Sans" } }
#endif

    , element_background_opacity { 32.0f / 255.0f }

    , heading_font_color         { basic_font_color }
    , heading_font               { font_descr{ "Roboto" }.medium() }
    , heading_font_size          { 14.0 }
    , heading_text_align         { canvas::middle | canvas::center }

    , label_font_color           { basic_font_color }
    , label_font                 { font_descr{ "Open Sans" } }
    , label_font_size            { 14.0 }
    , label_text_align           { canvas::middle | canvas::center }

    , icon_color                 { basic_font_color }
    , icon_font                  { font_descr{ "elements_basic" } }
    , icon_font_size             { 16.0 }
    , icon_button_color          { default_button_color }

    , text_box_font_color        { basic_font_color }
    , text_box_font              { font_descr{ "Open Sans" } }
    , text_box_font_size         { 14.0 }
    , text_box_hilite_color      { rgba(0, 127, 255, 100) }
    , text_box_caret_color       { rgba(0, 190, 255, 255) }
    , text_box_caret_width       { 1.2 }
    , inactive_font_color        { rgba(127, 127, 127, 150) }
    , input_box_text_limit       { 1024 }

    , ticks_color                { rgba(127, 127, 127, 150) }
    , major_ticks_level          { 0.5 }
    , major_ticks_width          { 1.5 }
    , minor_ticks_level          { 0.4 }
    , minor_ticks_width          { 0.7 }

    , major_grid_color           { frame_color }
    , major_grid_width           { 0.5 }
    , minor_grid_color           { indicator_color }
    , minor_grid_width           { 0.4 }

    , dialog_button_size         { 100 }
    , message_textbox_size       { { 300, 120 } }

    , dial_mode                  { dial_mode_enum::linear }
    , dial_linear_range          { 200 }

    , child_window_title_size    { 1.0 }
    , child_window_opacity       { 0.95 }
   {
   }

   // The global theme
   theme& global_theme::_theme()
   {
      static theme thm;
      return thm;
   }

   theme const& get_theme()
   {
      return global_theme::_theme();
   }

   void set_theme(theme const& thm)
   {
      global_theme::_theme() = thm;
   }
}}
