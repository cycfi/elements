/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>
#include <cmath>
#include <memory>
#include <string>

/*=============================================================================
   Two ways to pick one of a few: an image grid menu, cells cut from one
   image, here a sheet of oscillator waveforms drawn at startup; and a
   swatch grid of drawn content, here a palette of colors. Beside each,
   a swatch cell shows the current choice, large.
=============================================================================*/

using namespace cycfi::elements;
using cycfi::artist::rgba;
using cycfi::artist::offscreen_image;

namespace
{
   // Main window background color
   auto constexpr bkd_color = rgba(35, 35, 37, 255);
   auto background = box(bkd_color);

   auto constexpr cell_color = rgba(50, 50, 54, 255);
   auto constexpr wave_color = colors::light_sea_green;

   char const* const wave_names[] = {
      "Sine", "Triangle", "Saw", "Ramp", "Square", "Pulse", "Noise", "Steps"
   };

   char const* const color_names[] = {
      "Ruby", "Amber", "Gold", "Moss", "Teal", "Sky", "Iris", "Rose"
   };

   color const palette[] = {
      rgba(200, 80, 70, 255), rgba(220, 150, 60, 255), rgba(210, 200, 80, 255),
      rgba(110, 180, 90, 255), rgba(70, 170, 170, 255), rgba(80, 120, 200, 255),
      rgba(130, 100, 190, 255), rgba(190, 100, 160, 255)
   };

   constexpr float pi = 3.14159265f;

   // One period of each waveform, from -1 to 1, at t from 0 to 1.
   float wave(int i, float t)
   {
      switch (i)
      {
         case 0: return std::sin(2 * pi * t);
         case 1: return t < 0.25f? 4 * t : t < 0.75f? 2 - 4 * t : 4 * t - 4;
         case 2: return 1 - 2 * t;
         case 3: return 2 * t - 1;
         case 4: return t < 0.5f? 1 : -1;
         case 5: return t < 0.2f? 1 : -1;
         case 6: return std::sin(91 * t) * std::sin(37 * t + 1) * std::cos(13 * t);
         default: return std::floor(t * 5) / 2 - 1;
      }
   }

   // The sheet of waveforms, four by two, drawn at twice the pixel
   // density so it stays crisp on a Retina display.
   image_ptr waveforms(float cw, float ch)
   {
      auto img = std::make_shared<cycfi::artist::image>(cw * 4, ch * 2, 2);
      offscreen_image off{*img};
      canvas cnv{off.context()};
      for (int i = 0; i != 8; ++i)
      {
         float x = (i % 4) * cw, y = (i / 4) * ch;
         cnv.fill_style(cell_color);
         cnv.fill_rect({x + 1, y + 1, x + cw - 1, y + ch - 1});

         float mid = y + ch * 0.58f, amp = ch * 0.24f;
         cnv.stroke_style(wave_color.opacity(0.25));
         cnv.line_width(1);
         cnv.move_to({x + 10, mid});
         cnv.line_to({x + cw - 10, mid});
         cnv.stroke();

         cnv.stroke_style(wave_color);
         cnv.line_width(2.5);
         int const steps = 96;
         for (int s = 0; s <= steps; ++s)
         {
            float t = float(s) / steps;
            point p{x + 10 + t * (cw - 20), mid - wave(i, t) * amp};
            if (s == 0)
               cnv.move_to(p);
            else
               cnv.line_to(p);
         }
         cnv.stroke();
      }
      return img;
   }

   auto make_waveforms(view& view_)
   {
      auto sheet = waveforms(110, 80);

      swatch_highlighter look;
      look.label = [](int i) { return std::string{wave_names[i]}; };
      auto menu = share(image_grid_menu(sheet, 4, 2, 1.0f, look));
      auto chosen = share(image_grid_cell(sheet, 4, 2, element{}));

      menu->value(0);
      chosen->value(0);
      menu->on_change =
         [chosen, &view_](int i)
         {
            chosen->value(i);
            view_.refresh(*chosen);
         };

      return htile(
         margin({10, 10, 20, 10}, align_middle(hold(menu))),
         margin({0, 10, 10, 10}, align_middle(fixed_size({165, 120}, hold(chosen))))
      );
   }

   auto make_palette(view& view_)
   {
      auto chip = [](color c) { return margin({3, 3, 3, 3}, rbox(c.level(0.75), 4)); };
      auto chips = [&]()
      {
         return vtile(
            htile(chip(palette[0]), chip(palette[1]), chip(palette[2]), chip(palette[3])),
            htile(chip(palette[4]), chip(palette[5]), chip(palette[6]), chip(palette[7]))
         );
      };

      swatch_highlighter look;
      look.label = [](int i) { return std::string{color_names[i]}; };
      // A swatch cell draws its content at the content's own size, so
      // both are given the same fixed size.
      auto const size = point{440, 160};
      auto grid = share(swatch_grid(4, 2, fixed_size(size, chips()), look));
      auto chosen = share(swatch_cell(4, 2, fixed_size(size, chips()), element{}));

      grid->value(4);
      chosen->value(4);
      grid->on_change =
         [chosen, &view_](int i)
         {
            chosen->value(i);
            view_.refresh(*chosen);
         };

      return htile(
         margin({10, 10, 20, 10}, align_middle(hold(grid))),
         margin({0, 10, 10, 10}, align_middle(fixed_size({165, 120}, hold(chosen))))
      );
   }

   auto make_controls(view& view_)
   {
      return
         margin({20, 10, 20, 10},
            vtile(
               margin({10, 20, 10, 10},
                  pane("Waveforms", make_waveforms(view_), 0.8f)
               ),
               margin({10, 10, 10, 20},
                  pane("Palette", make_palette(view_), 0.8f)
               )
            )
         );
   }
}

int main(int argc, char* argv[])
{
   app _app("Swatches");

   // Set up the fonts and search paths once, at startup, so the
   // first window does not pay for it.
   init_resources();
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   view_.content(
      make_controls(view_),
      background
   );

   _app.run();
   return 0;
}
