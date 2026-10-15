/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/styles/buttons.adoc and
   draws its figures. The figures land in the build's results directory;
   copy them to docs/modules/ROOT/images/styles/ to update the page.
=============================================================================*/
#include "test_support.hpp"
#include <string>

namespace colors = cycfi::artist::colors;

using namespace cycfi::elements;
using namespace cycfi::elements::test;

TEST_CASE("buttons style page: chain order", "[buttons_style_page]")
{
   using bs = default_button_styler;

   SECTION("icon placement, either order")
   {
      auto a = button_styler{"x"}.icon(icons::floppy).icon_left();
      auto b = button_styler{"x"}.icon_left().icon(icons::floppy);
      CHECK(a.get_icon_placement() == bs::icon_left);
      CHECK(b.get_icon_placement() == bs::icon_left);
   }
   SECTION("corner radius, either order")
   {
      auto a = button_styler{"x"}.corner_radius(4).rounded_corner_top_left(8);
      auto b = button_styler{"x"}.rounded_corner_top_left(8).corner_radius(4);
      CHECK(a.get_corner_radius_top_left() == 8);
      CHECK(a.get_corner_radius_bottom_right() == 4);
      // A later call wins for the corners it names: all four.
      CHECK(b.get_corner_radius_top_left() == 4);
      CHECK(b.get_corner_radius_bottom_right() == 4);
   }
}

TEST_CASE("buttons style page: snippets", "[buttons_style_page]")
{
   auto ok = button("OK");
   auto mute = toggle_button("Mute", 1.0, colors::red.opacity(0.4));
   auto next = momentary_button(
      button_styler{"Next"}
         .icon(icons::right_circled)
         .align_right()
         .body_color(colors::green.opacity(0.4))
   );
   CHECK(next->get_icon_placement() == default_button_styler::icon_right);
   CHECK(next->get_label_alignment() == default_button_styler::align_right);

   // The styler holds the label.
   next->set_text("Done");
   CHECK(next->get_text() == "Done");

   auto play = toggle_button(
      button_face(
         icon{icons::play},
         icon{icons::pause}
      )
   );
   CHECK(!play.value());

   // Each state on its own body color: the colors are checked in
   // test/lib/icon_button_test.cpp.
   auto transport = toggle_icon_button(
      icons::play, icons::pause, 1.2, colors::green, colors::red);
   transport.value(true);
   CHECK(transport.value());
   (void) ok;
   (void) mute;
}

#if !defined(ARTIST_RECORDING)
namespace
{
   auto constexpr bkd_color = rgba(35, 35, 37, 255);
   constexpr auto bred     = colors::red.opacity(0.4);
   constexpr auto bgreen   = colors::green.level(0.7).opacity(0.4);
   constexpr auto bblue    = colors::blue.opacity(0.4);

   void open(test_view& tv)
   {
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
      headless::open(tv.view_);
#endif
   }

   template <typename E>
   void save(extent size, E&& content, char const* name)
   {
      test_view tv{size, 2};
      open(tv);
      tv.view_.content(
         margin({20, 20, 20, 20}, std::forward<E>(content)),
         box(bkd_color)
      );
      tv.view_.layout();
      tv.draw();
      tv.img.save_png(std::string{RESULTS_PATH} + name);
   }
}

TEST_CASE("buttons style page: figures", "[buttons_style_page]")
{
   {
      auto on = toggle_button("Toggle Button, on", 1.0, bred);
      on.value(true);
      auto default_on = toggle_button("Default Color, on");
      default_on.value(true);
      auto latched = latching_button("Latching Button, on", 1.0, bgreen);
      latched.value(true);
      auto disabled = button("Disabled Button");
      disabled.enable(false);
      save({360, 380},
         align_left_top(hsize(320,
            vtile_spaced(12.0,
               button("Momentary Button"),
               toggle_button("Toggle Button, off", 1.0, bred),
               on,
               default_on,
               latched,
               button(icons::cog, "Icon Left", 1.0, bblue),
               button("Icon Right", icons::right_circled, 1.0, bblue),
               disabled
            )
         )),
         "buttons_stock.png");
   }
   {
      save({480, 170},
         align_left_top(hsize(440,
            vtile_spaced(12.0,
               hgrid(
                  momentary_button(button_styler{"Left"}.align_left().icon(icons::left_circled).icon_left().body_color(bred)),
                  momentary_button(button_styler{"Center"}.body_color(bblue)),
                  momentary_button(button_styler{"Right"}.align_right().icon(icons::right_circled).body_color(bgreen))
               ),
               hgrid(
                  momentary_button(button_styler{"Rounded Left"}.align_left().icon(icons::left_circled).icon_left().body_color(bred).rounded_left(10)),
                  momentary_button(button_styler{"Square Center"}.body_color(bblue).corner_radius(0)),
                  momentary_button(button_styler{"Rounded Right"}.align_right().icon(icons::right_circled).body_color(bgreen).corner_radius(0, 10, 10, 0))
               ),
               hgrid(
                  momentary_button(button_styler{"Large"}.size(1.4)),
                  momentary_button(button_styler{"Gold Text"}.text_color(colors::gold)),
                  momentary_button(button_styler{"Rounded Top"}.rounded_top(12))
               )
            )
         )),
         "buttons_styler.png");
   }
   {
      auto indicator_color = get_theme().indicator_color;
      auto disabled_icon_button = icon_button(icons::block, 1.2);
      disabled_icon_button.enable(false);
      auto power = toggle_icon_button(icons::power, 1.2, indicator_color);
      power.value(true);

      std::string const res = std::string{EXAMPLES_PATH} + "buttons/resources/";
      float const button_scale = 1.0/4;
      sprite power_button = sprite{res + "power_180x632.png", 158*button_scale, button_scale};
      sprite phase_button = sprite{res + "phase_180x790.png", 158*button_scale, button_scale};
      sprite mail_button = sprite{res + "mail_180x790.png", 158*button_scale, button_scale};
      sprite transpo_button = sprite{res + "transpo_180x632.png", 158*button_scale, button_scale};
      auto power_on = toggle_button(power_button);
      power_on.value(true);
      auto phase_disabled = toggle_button(phase_button);
      phase_disabled.enable(false);

      save({480, 150},
         align_left_top(hsize(440,
            vtile_spaced(20.0,
               htile(
                  align_center(power),
                  align_center(icon_button(icons::magnifying_glass, 1.2)),
                  align_center(icon_button(icons::left_circled, 1.2)),
                  align_center(toggle_icon_button(icons::left, icons::right, 1.2)),
                  align_center(disabled_icon_button)
               ),
               htile(
                  align_center(power_on),
                  align_center(toggle_button(phase_button)),
                  align_center(momentary_button(mail_button)),
                  align_center(toggle_button(transpo_button)),
                  align_center(phase_disabled)
               )
            )
         )),
         "buttons_icons_sprites.png");
   }
}
#endif
