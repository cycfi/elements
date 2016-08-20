/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_THEME_APRIL_15_2016)
#define PHOTON_GUI_LIB_THEME_APRIL_15_2016

#include <photon/support.hpp>
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

      color                panel_color                = { 28, 30, 34, 192 };
      color                frame_color                = { 220, 220, 220, 70 };

      color                controls_color             = { 18, 49, 85, 200 };
      color                indicator_color            = { 0, 127, 255, 200 };

      color                heading_font_color         = { 220, 220, 220, 200 };
      char const*          heading_font               = "sans-bold";
      float                heading_font_size          = 20.0;

      color                label_font_color           = { 220, 220, 220, 200 };
      char const*          label_font                 = "sans";
      float                label_font_size            = 18.0;

      color                icon_color                 = { 220, 220, 220, 200 };
      char const*          icon_font                  = "icons";
      float                icon_font_size             = 18.0;

      color                text_box_font_color        = { 220, 220, 220, 200 };
      char const*          text_box_font              = "sans";
      float                text_box_font_size         = 18.0;
      color                text_box_hilite_color      = { 0, 127, 255, 100 };
      color                text_box_caret_color       = { 0, 190, 255, 255 };
      color                edit_box_fill_color        = { 32, 32, 32, 32 };
      color                inactive_font_color        = { 127, 127, 127, 150 };

      char const*          icons                      = "icons";
      char const*          sans                       = "sans";
      char const*          sans_bold                  = "sans-bold";

      virtual void         load_fonts();

   private:

      mutable class canvas _canvas;
   };

   using theme_ptr = std::shared_ptr<theme>;
   
   template <typename T>
   struct setter
   {
      setter(T& ref, T const& new_val)
       : _ref(ref)
       , _save(ref)
      {
         _ref = new_val;
      }
      
      ~setter()
      {
         _ref = _save;
      }

      T& _ref;
      T  _save;
   };
   
   template <typename T>
   inline setter<T> set(T& ref, T const& new_val)
   {
      return { ref, new_val };
   };
}

#endif
