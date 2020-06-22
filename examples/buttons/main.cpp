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
constexpr auto brblue   = colors::royal_blue.opacity(0.4);
constexpr auto pgold   = colors::gold.opacity(0.8);

auto make_buttons(view& view_)
{
   auto mbutton         = button("Momentary Button");
   auto tbutton         = toggle_button("Toggle Button", 1.0, bred);
   auto lbutton         = share(latching_button("Latching Button", 1.0, bgreen));
   auto reset           = button("Clear Latch", icons::lock_open, 1.0, bblue);
   auto note            = button(icons::cog, "Setup", 1.0, brblue);
   auto prog_bar        = share(progress_bar(rbox(colors::black), rbox(pgold)));
   auto prog_advance    = button("Advance Progress Bar");

   reset.on_click =
      [lbutton, &view_](bool) mutable
      {
         lbutton->value(0);
         view_.refresh(*lbutton);
      };

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

   return
      margin({ 20, 0, 20, 20 },
         vtile(
            top_margin(20, mbutton),
            top_margin(20, tbutton),
            top_margin(20, hold(lbutton)),
            top_margin(20, reset),
            top_margin(20, note),
            top_margin(20, vsize(25, hold(prog_bar))),
            top_margin(20, prog_advance)
         )
      );
}

auto make_controls(view& view_)
{
   auto  check_box1 = check_box("Reionizing electrons");
   auto  check_box2 = check_box("The Nexus Meridian Unfolding");
   auto  check_box3 = check_box("Serenity Dreamscape Exploration");

   check_box1.value(true);
   check_box2.value(true);
   check_box3.value(true);

   auto  check_boxes =
         group("Check boxes",
            margin({ 10, 10, 20, 20 },
               top_margin(25,
                  vtile(
                     top_margin(10, align_left(check_box1)),
                     top_margin(10, align_left(check_box2)),
                     top_margin(10, align_left(check_box3))
                  )
               )
            )
         );

   auto  radio_button1 = radio_button("Eons from now");
   auto  radio_button2 = radio_button("Ultra-sentient particles");
   auto  radio_button3 = radio_button("The stratosphere is electrified");

   radio_button1.select(true);

   auto  radio_buttons =
         group("Radio Buttons",
            margin({ 10, 10, 20, 20 },
               top_margin(25,
                  vtile(
                     top_margin(10, align_left(radio_button1)),
                     top_margin(10, align_left(radio_button2)),
                     top_margin(10, align_left(radio_button3))
                  )
               )
            )
         );

   auto indicator_color = get_theme().indicator_color;

   auto  icon_buttons =
         group("Icon Buttons",
            margin({ 10, 10, 20, 10 },
               vtile(
                  top_margin(35,
                     htile(
                        align_center(toggle_icon_button(icons::power, 1.2, indicator_color)),
                        align_center(icon_button(icons::magnifying_glass, 1.2)),
                        align_center(icon_button(icons::left_circled, 1.2)),
                        align_center(toggle_icon_button(icons::left, icons::right, 1.2))
                     )
                  )
               )
            )
         );

   float const button_scale = 1.0/4;
   sprite power_button = sprite{ "power_180x632.png", 158*button_scale, button_scale };
   sprite phase_button = sprite{ "phase_180x632.png", 158*button_scale, button_scale };
   sprite mail_button = sprite{ "mail_180x632.png", 158*button_scale, button_scale };
   sprite transpo_button = sprite{ "transpo_180x632.png", 158*button_scale, button_scale };

   auto  sprite_buttons =
         group("Sprite Buttons",
            margin({ 10, 10, 20, 10 },
               vtile(
                  top_margin(35,
                     htile(
                        align_center(toggle_button(power_button)),
                        align_center(toggle_button(phase_button)),
                        align_center(momentary_button(mail_button)),
                        align_center(toggle_button(transpo_button))
                     )
                  )
               )
            )
         );

   return
      vtile(
         htile(
            make_buttons(view_),
            vtile(
               margin({ 20, 20, 20, 20 }, check_boxes),
               margin({ 20, 20, 20, 20 }, radio_buttons)
            )
         ),
         htile(
            hmin_size(250, margin({ 20, 20, 20, 20 }, icon_buttons)),
            hmin_size(250, margin({ 20, 20, 20, 20 }, sprite_buttons))
         )
      );
}

int main(int argc, char* argv[])
{
   app _app(argc, argv, "Buttons", "com.cycfi.buttons");
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
