/*=============================================================================
   Copyright (c) 2024 Flole
   Copyright (c) 2024 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>

using namespace cycfi::elements;
using namespace std::chrono_literals;

constexpr auto bgreen   = colors::green.level(0.7).opacity(0.8);
constexpr auto pgold    = colors::gold.opacity(0.8);

// Main window background color
auto constexpr bkd_color = rgba(35, 35, 37, 255);
auto background = box(bkd_color);

bool run = false;
void prog_animate(concepts::StatusBar auto& prog_bar, view& view_)
{
   if (run)
   {
      if (auto val = prog_bar.value() + 0.005; val > 1.0)
      {
         prog_bar.value(1.0);
         run = false;
      }
      else
      {
         prog_bar.value(val);
      }
      view_.refresh(prog_bar);
      view_.post(10ms,
         [&]()
         {
            prog_animate(prog_bar, view_);
         }
      );
   }
}

auto make_bars(view& view_)
{
   auto prog_bar        = share(progress_bar(rbox(colors::black), rbox(pgold)));
   auto bsy_bar         = share(busy_bar(rbox(colors::black), rbox(bgreen)));
   auto start_stop      = toggle_icon_button(icons::play, icons::stop, 2.0);

   start_stop.on_click =
      [prog_bar, bsy_bar, &view_](bool state) mutable
      {
         if (state)
         {
            bsy_bar->start(view_, 10ms);
            run = true;
            prog_bar->value(0.0);
            prog_animate(*prog_bar, view_);
         }
         else
         {
            bsy_bar->stop(view_);
            run = false;
         }
      };

   return
      margin({20, 20, 20, 20},
         htile(
            start_stop,
            hspace(10),
            vtile(
               vsize(27, hold(prog_bar)),
               vspace(10),
               vsize(27, hold(bsy_bar))
            )
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
