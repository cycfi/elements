/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/support/theme.adoc and
   draws its figures. Figures land in the build's results directory; copy
   them to docs/modules/ROOT/images/support/ to update the page.
=============================================================================*/
#include "test_support.hpp"

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   auto constexpr bkd_color = rgba(35, 35, 37, 255);

   // Every case leaves the global theme as it found it.
   struct restore_theme
   {
      ~restore_theme() { set_theme(theme{}); }
   };

   auto controls()
   {
      return margin({20, 16, 20, 16},
         vtile(
            label("Volume"),
            top_margin(8,
               slider(basic_thumb<20>(),
                  slider_marks_lin<20>(basic_track<5, false>()), 0.6)),
            top_margin(12, button("Play")),
            top_margin(12, check_box("Loop"))
         )
      );
   }

   void figure(char const* name)
   {
      test_view tv{extent{160, 165}, 2};
      tv.view_.content(align_center_middle(controls()), box(bkd_color));
      tv.draw();
      tv.img.save_png(std::string{RESULTS_PATH} + name);
   }
}

TEST_CASE("theme page: get_theme and set_theme", "[theme_page]")
{
   restore_theme guard;

   // The defaults the page lists.
   theme t;
   CHECK(t.frame_corner_radius == 3.0f);
   CHECK(t.scrollbar_width == 10.0f);
   CHECK(t.label_font._size == 14.0f);
   CHECK(t.disabled_opacity == Approx(0.45f));
   CHECK(t.input_box_text_limit == 1024);

   // set_theme replaces the global theme; get_theme reads it.
   t.frame_corner_radius = 8;
   set_theme(t);
   CHECK(get_theme().frame_corner_radius == 8.0f);

   set_theme(theme{});
   CHECK(get_theme().frame_corner_radius == 3.0f);
}

TEST_CASE("theme page: override_theme", "[theme_page]")
{
   restore_theme guard;
   auto before = get_theme().frame_corner_radius;
   {
      auto ov = override_theme(&theme::frame_corner_radius, 12.0f);
      CHECK(get_theme().frame_corner_radius == 12.0f);
   }
   CHECK(get_theme().frame_corner_radius == before);
}

TEST_CASE("theme page: a moved override", "[theme_page]")
{
   // The moved-to override puts back the value the first one saved, not
   // the override in force when it was moved.
   restore_theme guard;
   auto before = get_theme().frame_corner_radius;
   {
      auto a = override_theme(&theme::frame_corner_radius, 12.0f);
      auto b = std::move(a);
   }
   CHECK(get_theme().frame_corner_radius == before);
}

TEST_CASE("theme page: figures", "[theme_page]")
{
   restore_theme guard;
   figure("support_theme_default.png");

   theme t;
   t.indicator_color = rgba(67, 160, 71, 230);
   t.indicator_bright_color = t.indicator_color.level(1.5);
   t.indicator_hilite_color = t.indicator_color.level(2.0);
   t.label_font_color = rgba(255, 200, 80, 230);
   set_theme(t);
   figure("support_theme_custom.png");
}
