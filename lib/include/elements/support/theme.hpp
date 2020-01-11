/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_THEME_APRIL_15_2016)
#define ELEMENTS_THEME_APRIL_15_2016

#include <elements/support.hpp>

namespace cycfi { namespace elements
{
   class theme
   {
   public:

      color                panel_color                = rgba(28, 30, 34, 192);
      color                frame_color                = rgba(220, 220, 220, 80);
      float                frame_corner_radius        = 3.0;
      float                frame_stroke_width         = 1.0;
      color                scrollbar_color            = rgba(80, 80, 80, 80);
      color                default_button_color       = rgba(0, 0, 0, 0);

      color                controls_color             = rgba(18, 49, 85, 200);
      color                indicator_color            = rgba(0, 127, 255, 200);
      color                basic_font_color           = rgba(220, 220, 220, 200);

// The symbols_font font is the OS supplied font that includes unicode symbols
// such as Miscellaneous Technical : Unicode U+2300 – U+23FF (8960–9215)
#if defined(__APPLE__)
      char const*          symbols_font               = "Lucida Grande";
#elif defined(_WIN32)
      char const*          symbols_font               = "Segoe UI Symbol";
#elif defined(__linux__)
      char const*          symbols_font               = "Arial";
#endif

      color                heading_font_color         = basic_font_color;
      char const*          heading_font               = "Roboto Bold";
      float                heading_font_size          = 14.0;

      color                label_font_color           = basic_font_color;
      char const*          label_font                 = "Open Sans";
      float                label_font_size            = 14.0;

      color                icon_color                 = basic_font_color;
      char const*          icon_font                  = "elements_basic";
      float                icon_font_size             = 16.0;
      color                icon_button_color          = default_button_color;

      color                text_box_font_color        = basic_font_color;
      char const*          text_box_font              = "Open Sans";
      float                text_box_font_size         = 14.0;
      color                text_box_hilite_color      = rgba(0, 127, 255, 100);
      color                text_box_caret_color       = rgba(0, 190, 255, 255);
      float                text_box_caret_width       = 1.2;
      color                inactive_font_color        = rgba(127, 127, 127, 150);

      color                ticks_color                = rgba(127, 127, 127, 150);
      float                major_ticks_level          = 0.5;
      float                major_ticks_width          = 1.5;
      float                minor_ticks_level          = 0.4;
      float                minor_ticks_width          = 0.7;

      color                major_grid_color           = frame_color;
      float                major_grid_width           = 0.5;
      color                minor_grid_color           = indicator_color;
      float                minor_grid_width           = 0.4;

      float                dialog_button_size         = 100;
      extent               message_textbox_size       = { 300, 120 };
   };

   // Access to the global theme
   theme const& get_theme();

   // Set the global theme
   void set_theme(theme const& thm);

   template <typename T>
   class scoped_theme_override
   {
   public:

       scoped_theme_override(theme& thm, T theme::*pmem, T val)
       : _thm(thm)
       , _pmem(pmem)
       , _save(thm.*pmem)
      {
         _thm.*_pmem = val;
      }

       scoped_theme_override(scoped_theme_override&& rhs)
       : _thm(rhs._thm)
       , _pmem(rhs._pmem)
       , _save(rhs._thm.*_pmem)
      {
         rhs._pmem = nullptr;
      }

      ~scoped_theme_override()
      {
         if (_pmem)
            _thm.*_pmem = _save;
      }

   private:

      theme&      _thm;
      T theme::*  _pmem;
      T           _save;
   };

   class global_theme
   {
      template <typename T>
      friend class scoped_theme_override;

      friend theme const& get_theme();
      friend void set_theme(theme const& thm);

      template <typename T>
      friend scoped_theme_override<T>
      override_theme(T theme::*pmem, T val);

      static theme _theme;
   };

   template <typename T>
   scoped_theme_override<T>
   override_theme(T theme::*pmem, T val)
   {
      return scoped_theme_override<T>{
         global_theme::_theme, pmem, val
      };
   }
}}

#endif
