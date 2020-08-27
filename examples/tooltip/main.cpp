/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>
#include <algorithm>
#include <random>

using namespace cycfi::elements;

// Main window background color
auto constexpr bkd_color = rgba(35, 35, 37, 255);
auto background = box(bkd_color);

constexpr auto bred     = colors::red.opacity(0.4);
constexpr auto bgreen   = colors::green.level(0.7).opacity(0.4);
constexpr auto bblue    = colors::blue.opacity(0.4);
constexpr auto pgold    = colors::gold.opacity(0.8);

auto make_tip(std::string text)
{
   return layer(
      margin({ 20, 8, 20, 8 }, label(text))
    , panel{}
   );
}

auto make_button(std::string text, color c)
{
   return tooltip(toggle_button(text, 1.0f, c), make_tip("This is " + text));
}

auto make_buttons(view& view_)
{
   auto button1 = make_button("Button 1", bred);
   auto button2 = make_button("Button 2", bgreen);
   auto button3 = make_button("Button 3", bblue);
   auto button4 = make_button("Button 4", pgold);

   return
      margin({ 50, 20, 50, 40 },
         vtile(
            top_margin(20, button1),
            top_margin(20, button2),
            top_margin(20, button3),
            top_margin(20, button4)
         )
      );
}

int main(int argc, char* argv[])
{
   app _app(argc, argv, "Tooltip", "com.cycfi.tooltip");
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   view_.content(
      make_buttons(view_),
      background
   );

   _app.run();
   return 0;
}
