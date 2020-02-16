#include <elements.hpp>
#include <pango/pangocairo.h>

using namespace cycfi::elements;

// Main window background color
auto constexpr bkd_color = rgba(35, 35, 37, 255);
auto background = box(bkd_color);

auto samples = basic(
   [](context const& ctx)
   {
      auto& cnv = ctx.canvas;
      auto descr = font_descr{ "Open Sans" }.size(24);
      auto descr_c = font_descr{ "Open Sans Condensed" }.size(24);

      text_layout layout_{ cnv };
      layout_.font(descr);

      cnv.fill_style(get_theme().label_font_color);

      layout_.text("Regular");
      cnv.fill({ 20, 20 }, layout_);

      layout_.text("Bold");
      layout_.font(descr.weight(font_descr::bold));
      cnv.fill({ 150, 20 }, layout_);

      layout_.text("Light");
      layout_.font(descr.weight(font_descr::light));
      cnv.fill({ 240, 20 }, layout_);

      layout_.text("Italic");
      layout_.font(descr.style(font_descr::italic));
      cnv.fill({ 330, 20 }, layout_);

       layout_.text("Condensed");
       layout_.font(descr_c.stretch(font_descr::condensed));
       cnv.fill({ 420, 20 }, layout_);

      layout_.text("Outline");
      layout_.font(descr);
      cnv.stroke_style(get_theme().label_font_color);
      cnv.line_width(0.5);
      cnv.stroke({ 20, 60 }, layout_);

      layout_.text("AV Kerning");
      cnv.fill({ 20, 100 }, layout_);

      cnv.font("Open Sans", 32);
      cnv.fill_style(colors::blue);
      cnv.fill_text({ 20, 160 }, "AV Kerning");

   }
);

int main(int argc, const char* argv[])
{
   app _app(argc, argv);
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   view_.content(
      {
         share(samples),
         share(background)    // Replace background with your main element,
                              // or keep it and add another layer on top of it.
      }
   );

   _app.run();
   return 0;
}
