/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <photon.hpp>

using namespace cycfi::photon;

// Main window background color
auto constexpr bkd_color = rgba(35, 35, 37, 255);

struct background : element
{
   void draw(context const& ctx)
   {
      auto& cnv = ctx.canvas;
      cnv.fill_style(bkd_color);
      cnv.fill_rect(ctx.bounds);
   }
};

auto make_buttons()
{
   auto  check_box1 = check_box("Reionizing electrons");
   auto  check_box2 = check_box("The Nexus Meridian Unfolding");
   auto  check_box3 = check_box("Serenity Dreamscape Exploration");

   check_box1.value(true);
   check_box2.value(true);
   check_box3.value(true);

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

   auto  group2 =
         group("Icon Toggles",
            margin({ 10, 10, 20, 20 },
               vtile(
                  top_margin(45,
                     htile(
                        align_center(icon_button(icons::power, 24)),
                        align_center(icon_button(icons::magnifying_glass, 24)),
                        align_center(icon_button(icons::left_circled, 24))
                     )
                  )
               )
            )
         );

   return vtile(
      margin({ 20, 20, 20, 20 }, group1),
      margin({ 20, 20, 20, 20 }, group2)
   );
}

auto make_popup_menu()
{
   auto popup  = dropdown_menu("Dropdown Menu");

   auto menu =
      layer(
         vtile(
               menu_item("Quantum Feedback Loop"),
               menu_item("Psionic Wave Oscillator"),
               menu_item("Gaia Abiogenesis"),
               menu_item_spacer(),
               menu_item("Chaotic Synchronicity"),
               menu_item("Omega Quadrant"),
               menu_item("Photonic Mesh"),
               menu_item("Antimatter Soup"),
               menu_item("Dark Beta Quarks"),
               menu_item("Cosmic Infrared Shift")
         ),
         menu_background{}
      );

   popup.menu(menu);

   return popup;
}

auto make_more_buttons(view& view_)
{
   constexpr auto bred     = colors::red.opacity(0.4);
   constexpr auto bgreen   = colors::green.level(0.7).opacity(0.4);
   constexpr auto bblue    = colors::blue.opacity(0.4);
   constexpr auto brblue   = colors::royal_blue.opacity(0.4);

   auto mbutton            = button("Momentary Button");
   auto tbutton            = toggle_button("Toggle Button", bred);
   auto lbutton            = share(latching_button("Latching Button", bgreen));
   auto reset              = button("Clear Latch", icons::lock_open, bblue);
   auto note               = button(icons::cog, "Setup", brblue);

   reset.on_click =
      [lbutton, &view_](bool) mutable
      {
         lbutton->value(0);
         view_.refresh(*lbutton);
      };

   return
      margin({ 20, 0, 20, 20 },
         vtile(
            top_margin(20, make_popup_menu()),
            top_margin(20, mbutton),
            top_margin(20, tbutton),
            top_margin(20, link(lbutton)),
            top_margin(20, reset),
            top_margin(20, note)
         )
      );
}

auto make_controls(view& view_)
{
   return
      margin({ 20, 10, 20, 10 },
         htile(
            margin({ 20, 20, 20, 20 }, pane("Buttons", make_buttons(), 0.8f)),
            margin({ 20, 20, 20, 20 }, pane("More Buttons", make_more_buttons(view_), 0.8f))
         )
      );
}

int main(int argc, const char* argv[])
{
   app _app(argc, argv);
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win.host());

   view_.content(
      {
         share(background{}),
         share(make_controls(view_))
      }
   );

   _app.run();
   return 0;
}
