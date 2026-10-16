/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>
#include <algorithm>
#include <array>
#include <chrono>
#include <random>

/*=============================================================================
   Sliders and knobs on styled tracks: fills in solid colors and gradients,
   glows, radial arcs, meter bands and a pan knob that fills from the
   center. Each fader drives the level meter beside it, which moves with a
   little noise, the way a mixer's meters do; the master fader, on a
   decibel scale, drives a stereo pair.
=============================================================================*/

using namespace cycfi::elements;
using cycfi::artist::rgba;
using namespace std::chrono_literals;

namespace
{
   // Main window background color
   auto constexpr bkd_color = rgba(35, 35, 37, 255);
   auto background = box(bkd_color);

   auto constexpr track_gray     = rgba(60, 60, 64, 255);
   auto constexpr meter_green    = rgba(46, 204, 64, 255);
   auto constexpr meter_yellow   = rgba(255, 210, 0, 255);
   auto constexpr meter_red      = rgba(230, 40, 40, 255);

   constexpr std::size_t num_channels = 3;

   using slider_ptr = std::shared_ptr<basic_slider_base>;
   using meter_ptr = std::shared_ptr<status_bar_base>;

   std::array<slider_ptr, num_channels> faders;
   std::array<meter_ptr, num_channels> meters;

   slider_ptr master;
   std::array<meter_ptr, 2> master_meters;      // left and right

   // Horizontal sliders: the fill in its default color, a range, solid
   // colors, a fill on a lighter track, and a gradient.
   auto make_fill_sliders()
   {
      auto row = [](auto&& e) { return align_middle(hmargin({10, 10}, e)); };

      return hmin_size(260,
         vtile(
            row(slider(basic_thumb<20>(), basic_fill_track<5>(), 0.6)),
            row(range_slider(
               basic_thumb<20>(), basic_thumb<20>(),
               basic_fill_track<5>(), {0.25, 0.7})),
            row(slider(
               basic_thumb<20>().indicator_color(colors::orange),
               basic_fill_track<5>().fill_color(colors::orange.level(0.8)),
               0.4)),
            row(slider(
               basic_thumb<20>(),
               basic_fill_track<5>()
                  .track_color(track_gray)
                  .fill_color(colors::light_sea_green),
               0.8)),
            row(slider(
               basic_thumb<20>(),
               basic_fill_track<5>().fill_gradient({
                  {0.0f, colors::light_sea_green}
                , {1.0f, colors::medium_orchid}
               }),
               0.9))
         )
      );
   }

   auto make_meter(double init)
   {
      return share(
         progress_bar(
            basic_track<8, true, track_shape::square>()
          , basic_fill_track<8, true, track_shape::square>().glow().fill_colors({
               {0.7f, meter_green}
             , {0.9f, meter_yellow}
             , {1.0f, meter_red}
            })
          , init
         )
      );
   }

   // A fader on a glowing gradient track, and the level meter it drives:
   // green, then yellow, then red near the top.
   auto make_channel(std::size_t index, double init)
   {
      faders[index] = share(
         slider(
            basic_thumb<20>(),
            basic_fill_track<5, true>().glow().fill_gradient({
               {0.0f, colors::light_sea_green}
             , {1.0f, colors::medium_orchid}
            }),
            init
         )
      );

      meters[index] = make_meter(init);

      return htile(
         hmargin({10, 6}, hold(faders[index])),
         hmargin({6, 10}, hold(meters[index]))
      );
   }

   // The master fader, on a decibel scale from -60 dB to +10 dB with its
   // marks and labels, starting at unity, and the stereo pair of meters it
   // drives.
   auto make_master()
   {
      db_scale const scale{-60, 10};
      master = share(
         slider(
            basic_thumb<20>(),
            slider_labels_db<10>(
               slider_marks_db<20>(
                  basic_fill_track<5, true>().glow().fill_color(colors::light_sea_green),
                  scale
               ),
               0.7,
               scale
            ),
            scale.position(0)
         )
      );
      master_meters[0] = make_meter(0.6);
      master_meters[1] = make_meter(0.6);

      return htile(
         hmargin({10, 6}, hsize(70, hold(master))),
         hmargin({6, 2}, hold(master_meters[0])),
         hmargin({2, 10}, hold(master_meters[1]))
      );
   }

   auto make_channels()
   {
      return vmin_size(300,
         vmargin({10, 10},
            htile(
               make_channel(0, 0.55),
               make_channel(1, 0.8),
               make_channel(2, 0.35),
               margin_left(10, make_master())
            )
         )
      );
   }

   // Knobs on radial fill tracks: plain, solid orange, a glowing gradient,
   // meter bands, marks around the arc, and a pan knob that fills from the
   // center.
   auto make_knobs()
   {
      auto knob = [](auto&& track, double init)
      {
         return align_center_middle(margin({12, 12, 12, 12}, dial(track, init)));
      };

      return vtile(
         htile(
            knob(radial_fill_track<20>(basic_knob<50>()), 0.6),
            knob(
               radial_fill_track<20>(basic_knob<50>())
                  .fill_color(colors::orange.level(0.8)),
               0.35)
         ),
         htile(
            knob(
               radial_fill_track<20, track_shape::square>(basic_knob<50>())
                  .glow()
                  .fill_gradient({
                     {0.0f, colors::light_sea_green}
                   , {1.0f, colors::medium_orchid}
                  }),
               0.8),
            knob(
               radial_fill_track<20, track_shape::square>(basic_knob<50>())
                  .glow()
                  .fill_colors({
                     {0.7f, meter_green}
                   , {0.9f, meter_yellow}
                   , {1.0f, meter_red}
                  }),
               0.95)
         ),
         htile(
            knob(radial_marks<20>(radial_fill_track<20>(basic_knob<50>())), 0.5),
            knob(
               radial_fill_track<20, track_shape::round, track_origin::center>(
                  basic_knob<50>()),
               0.25)
         )
      );
   }

   auto make_controls()
   {
      return
         margin({20, 10, 20, 10},
            htile(
               margin({10, 20, 10, 20},
                  pane("Fill Tracks", make_fill_sliders(), 0.8f)
               ),
               margin({10, 20, 10, 20},
                  pane("Faders and Meters", make_channels(), 0.8f)
               ),
               margin({10, 20, 10, 20},
                  pane("Radial Tracks", make_knobs(), 0.8f)
               )
            )
         );
   }

   // Every 40 ms, each meter moves toward its fader's level, plus a little
   // noise, so the meters look alive.
   void animate_meters(view& view_)
   {
      static std::mt19937 gen{std::random_device{}()};
      static std::uniform_real_distribution<double> noise{-0.12, 0.06};

      auto move = [&](meter_ptr const& m, double to)
      {
         auto target = std::clamp(to + noise(gen), 0.0, 1.0);
         auto level = m->value();
         m->value(level + (target - level) * 0.5);
         view_.refresh(*m);
      };

      for (std::size_t i = 0; i != num_channels; ++i)
         move(meters[i], faders[i]->value());
      for (auto const& m : master_meters)
         move(m, master->value());
      view_.post(40ms, [&view_]() { animate_meters(view_); });
   }
}

int main(int argc, char* argv[])
{
   app _app("Styled Sliders And Knobs");

   // Set up the fonts and search paths once, at startup, so the
   // first window does not pay for it.
   init_resources();
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   view_.content(
      make_controls(),
      background
   );

   animate_meters(view_);
   _app.run();
   return 0;
}
