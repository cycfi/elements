/*=============================================================================
   Copyright (c) 2024 Flole
   Copyright (c) 2024 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>

using namespace cycfi::elements;
using cycfi::artist::rgba;
namespace colors = cycfi::artist::colors;
using namespace std::chrono_literals;

constexpr auto bred     = colors::red.opacity(0.4);
constexpr auto bgreen   = colors::green.level(0.7).opacity(0.8);
constexpr auto pgold    = colors::gold.opacity(0.8);

// Main window background color
auto constexpr bkd_color = rgba(35, 35, 37, 255);
auto background = box(bkd_color);

using bar_list = std::vector<std::shared_ptr<status_bar_base>>;

// Advance the progress bars together, a step every 10 ms, until full.
bool run = false;
void prog_animate(bar_list bars, view& view_)
{
   if (!run)
      return;

   for (auto& bar : bars)
   {
      bar->value(std::min(bar->value() + 0.005, 1.0));
      view_.refresh(*bar);
   }
   if (bars.front()->value() >= 1.0)
   {
      run = false;
      return;
   }
   view_.post(10ms, [bars, &view_]() { prog_animate(bars, view_); });
}

auto make_bars(view& view_)
{
   // Horizontal bars fill from the left; vertical ones, from the bottom.
   auto prog_bar = share(progress_bar(rbox(colors::black), rbox(pgold)));
   auto bsy_bar = share(busy_bar(rbox(colors::black), rbox(bgreen)));
   auto vprog_bar = share(progress_bar(rbox(colors::black), rbox(pgold)));
   auto vbsy_bar = share(busy_bar(rbox(colors::black), rbox(bgreen)));
   auto start_stop = toggle_icon_button(icons::play, icons::stop, 2.0, bred);

   start_stop.on_click =
      [=, &view_](bool state)
      {
         if (state)
         {
            bsy_bar->start(view_, 10ms);
            vbsy_bar->start(view_, 10ms);
            run = true;
            prog_bar->value(0.0);
            vprog_bar->value(0.0);
            prog_animate({prog_bar, vprog_bar}, view_);
         }
         else
         {
            bsy_bar->stop(view_);
            vbsy_bar->stop(view_);
            run = false;
         }
      };

   return
      margin({20, 20, 20, 20},
         htile_spaced(20,
            align_middle(start_stop),
            align_middle(vtile_spaced(10,
               vsize(27, hold(prog_bar)),
               vsize(27, hold(bsy_bar))
            )),
            hsize(27, vsize(150, hold(vprog_bar))),
            hsize(27, vsize(150, hold(vbsy_bar)))
         )
      );
}

int main(int argc, char* argv[])
{
   app _app("Status Bars");

   // Set up the fonts and search paths once, at startup, so the
   // first window does not pay for it.
   init_resources();
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
