/*=============================================================================
   Copyright (c) 2024 Flole

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>

using namespace cycfi::elements;
using cycfi::artist::rgba;
namespace colors = cycfi::artist::colors;
using namespace std::chrono_literals;

constexpr auto bred     = colors::red.opacity(0.4);
constexpr auto bgreen   = colors::green.level(0.7).opacity(0.4);
constexpr auto brblue   = colors::royal_blue.opacity(0.4);
constexpr auto pgold    = colors::gold.opacity(0.8);

// Main window background color
auto constexpr bkd_color = rgba(35, 35, 37, 255);
auto background = box(bkd_color);

auto make_bars(view& view_)
{
   auto prog_bar        = share(progress_bar(rbox(colors::black), rbox(pgold)));
   auto bsy_bar         = share(busy_bar(rbox(colors::black), rbox(brblue)));
   auto prog_advance    = icon_button(icons::plus);
   auto busy_start      = button("I", 1.0, bgreen);
   auto busy_stop       = button("O", 1.0, bred);

   prog_advance.on_click =
      [prog_bar, &view_](bool) mutable
      {
         auto val = prog_bar->value();
         if (val > 0.9)
            prog_bar->value(0.0);
         else
            prog_bar->value(val + 0.125);
         view_.refresh(*prog_bar);
      };

   busy_stop.on_click =
      [bsy_bar, &view_](bool) mutable
      {
         bsy_bar->animate(view_, 0ms);
      };

   busy_start.on_click =
      [bsy_bar, &view_](bool) mutable
      {
         bsy_bar->animate(view_, 50ms);
      };

   return
      margin({20, 0, 20, 20},
         vtile(
            margin_top(20, htile(
               margin_right(3, valign(0.5, prog_advance)),
               vsize(27, hold(prog_bar))
            )),
            margin_top(20, vtile(
               margin_right(3, htile(
                  busy_stop,
                  busy_start
               )),
               vsize(27, hold(bsy_bar))
            ))
         )
      );
}


int main(int argc, char* argv[])
{
   app _app("Status Bars");
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   view_.content(
      make_bars(view_),
      background
   );

   _app.run();
   return 0;
}
