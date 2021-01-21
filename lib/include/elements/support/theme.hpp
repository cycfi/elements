/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_THEME_APRIL_15_2016)
#define ELEMENTS_THEME_APRIL_15_2016

#include <elements/support.hpp>

namespace cycfi { namespace elements
{
   enum class dial_mode_enum : int;

   class theme
   {
   public:

                           theme();

      color                panel_color;
      color                frame_color;
      color                frame_hilite_color;
      float                frame_corner_radius;
      float                frame_stroke_width;
      color                scrollbar_color;
      color                default_button_color;
      rect                 button_margin;

      color                controls_color;
      float                controls_frame_stroke_width;
      color                indicator_color;
      color                indicator_bright_color;
      color                indicator_hilite_color;
      color                basic_font_color;

      font                 system_font;   // The system_font font is the font the OS
                                          // uses for displaying text in OS UI elements
                                          // such as menus, window title-bars, etc.

      float                element_background_opacity;

      color                heading_font_color;
      font                 heading_font;
      float                heading_font_size;
      int                  heading_text_align;

      color                label_font_color;
      font                 label_font;
      float                label_font_size;
      int                  label_text_align;

      color                icon_color;
      font                 icon_font;
      float                icon_font_size;
      color                icon_button_color;

      color                text_box_font_color;
      font                 text_box_font;
      float                text_box_font_size;
      color                text_box_hilite_color;
      color                text_box_caret_color;
      float                text_box_caret_width;
      color                inactive_font_color;
      std::size_t          input_box_text_limit;

      color                ticks_color;
      float                major_ticks_level;
      float                major_ticks_width;
      float                minor_ticks_level;
      float                minor_ticks_width;

      color                major_grid_color;
      float                major_grid_width;
      color                minor_grid_color;
      float                minor_grid_width;

      float                dialog_button_size;
      extent               message_textbox_size;

      dial_mode_enum       dial_mode;
      float                dial_linear_range;

      float                child_window_title_size;
      float                child_window_opacity;
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

      static theme& _theme();
   };

   template <typename T>
   scoped_theme_override<T>
   override_theme(T theme::*pmem, T val)
   {
      return scoped_theme_override<T>{
         global_theme::_theme(), pmem, val
      };
   }
}}

#endif
