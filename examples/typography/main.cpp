/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>
#include <iostream>

using namespace cycfi::elements;

// Main window background color
auto constexpr bkd_color = rgba(35, 35, 37, 255);
auto background = box(bkd_color);

auto samples = basic(
   [](context const& ctx)
   {
      auto& cnv = ctx.canvas;
      cnv.fill_style(get_theme().label_font_color);
      cnv.font_size(36);

      // Note: This is just a demo. Normally, you do not want to
      // create fonts when drawing. Preferrably, you will want to
      // create your fonts somewhere else (e.g. in a constructor).

      cnv.font(font_descr{ "Open Sans" });
      cnv.fill_text({ 20, 40 }, "Regular");

      cnv.font(font_descr{ "Open Sans" }.bold());
      cnv.fill_text({ 160, 40 }, "Bold");

      cnv.font(font_descr{ "Open Sans" }.light());
      cnv.fill_text({ 250, 40 }, "Light");

      cnv.font(font_descr{ "Open Sans" }.italic());
      cnv.fill_text({ 340, 40 }, "Italic");

      // In this case, the font already describes the condensed 'stretch'
      cnv.font(font_descr{ "Open Sans Condensed" });
      cnv.fill_text({ 430, 40 }, "Condensed");

      // In this case, the font already describes the condensed 'stretch'
      cnv.font(font_descr{ "Open Sans Condensed" }.italic());
      cnv.fill_text({ 575, 40 }, "Condensed Italic" );

      cnv.font(font_descr{ "Open Sans" }.bold());
      cnv.line_width(0.5);
      cnv.stroke_text({ 20, 90 }, "Outline");
   }
);

int main(int argc, char* argv[])
{
   app _app(argc, argv, "Typography", "com.cycfi.typography");
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   view_.content(
      {
         share(samples),
         share(background)
      }
   );

   _app.run();
   return 0;
}
