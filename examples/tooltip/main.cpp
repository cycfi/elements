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

template <typename Label>
auto make_button(std::string text, color c, Label label, view& view_)
{
   static int i = 1;
   auto tt = tooltip(toggle_button("Option " + std::to_string(i++), 1.0f, c), make_tip(text));
   tt.on_hover =
      [label, text, &view_, orig = label->get_text()](bool visible)
      {
         label->set_text(visible? text : orig);
         view_.refresh(*label.get(), 3);
      };
   return tt;
}

auto make_buttons(view& view_)
{
   auto status = share(label("Have you found your vision quest?"));
   auto button1 = make_button("Eons from now", bred, status, view_);
   auto button2 = make_button("Take a stand against delusion", bgreen, status, view_);
   auto button3 = make_button("Totality is calling", bblue, status, view_);
   auto button4 = make_button("Four-dimensional superstructures", pgold, status, view_);

   return
      margin({ 50, 20, 50, 40 },
         vtile(
            align_center(top_margin(20, hold(status))),
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
