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

auto make_column()
{
   auto  check_box1 = check_box("Reionizing electrons");
   auto  check_box2 = check_box("The Nexus Meridian Unfolding");
   auto  check_box3 = check_box("Serenity Dreamscape Exploration");

   check_box1.value(true);
   check_box2.value(true);
   check_box3.value(true);

   auto  group0 =
         group("Text Boxes",
            margin({ 10, 10, 20, 20 },
               top_margin(35,
                  vtile(
                     left_caption(input_box("Show me the money").first, "Gimme Some"),
                     top_margin(10, left_caption(input_box("Show me more").first, "Gimme Some More"))
                  )
               )
            )
         );

   auto  group1 =
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

   auto indicator_color = get_theme().indicator_color;

   float const button_scale = 1.0/4;
   sprite power_button = sprite{ "power_180x632.png", 158*button_scale, button_scale };
   sprite phase_button = sprite{ "phase_180x632.png", 158*button_scale, button_scale };
   sprite mail_button = sprite{ "mail_180x632.png", 158*button_scale, button_scale };
   sprite transpo_button = sprite{ "transpo_180x632.png", 158*button_scale, button_scale };

   auto  group2 =
         group("Buttons",
            margin({ 10, 10, 20, 10 },
               top_margin(35,
                  vtile(
                     htile(
                        align_center(toggle_icon_button(icons::power, 1.2, indicator_color)),
                        align_center(icon_button(icons::magnifying_glass, 1.2)),
                        align_center(icon_button(icons::left_circled, 1.2)),
                        align_center(toggle_icon_button(icons::left, icons::right, 1.2))
                     ),
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

   return vtile(
      margin({ 20, 20, 20, 20 }, group0),
      margin({ 20, 20, 20, 20 }, group1),
      margin({ 20, 20, 20, 20 }, group2)
   );
}

auto make_controls(view& view_)
{
   return
      margin({ 20, 10, 20, 10 },
         htile(
            align_top(margin({ 20, 20, 20, 20 },
               pane("Scale = 1.0", make_column())
            )),
            align_top(margin({ 20, 20, 20, 20 },
               scale(0.8, pane("Scale = 0.8", make_column()))
            ))
         )
      );
}

int main(int argc, char* argv[])
{
   app _app(argc, argv, "Scale", "com.cycfi.scale");
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
