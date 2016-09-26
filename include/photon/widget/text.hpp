/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_TEXT_APRIL_17_2016)
#define PHOTON_GUI_LIB_WIDGET_TEXT_APRIL_17_2016

#include <photon/support/glyphs.hpp>
#include <photon/support/theme.hpp>
#include <photon/widget/widget.hpp>
#include <string>
#include <vector>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Text Box
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class text_box : public widget
   {
   public:
                              text_box(
                                 std::string const& text
                               , char const* face  = get_theme().text_box_font
                               , float size        = get_theme().text_box_font_size
                               , color color_      = get_theme().text_box_font_color
                               , int style         = canvas::normal
                              );

      virtual widget_limits   limits(basic_context const& ctx) const;
      virtual void            layout(context const& ctx);
      virtual void            draw(context const& ctx);

      std::string             text() const                     { return _text; }
      void                    text(std::string const& text);
      virtual void            value(std::string val);

      using widget::text;

   private:

      std::string             _text;
      glyphs                  _layout;
      std::vector<glyphs>     _rows;
      color                   _color;
      float                   _current_width;
   };
}

#endif