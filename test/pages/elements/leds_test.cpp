/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/elements/leds.adoc and
   draws its figure. The figure lands in the build's results directory;
   copy it to docs/modules/ROOT/images/elements/ to update the page.
=============================================================================*/
#include "test_support.hpp"
#include <string>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   auto constexpr bkd_color = rgba(35, 35, 37, 255);

   void open(test_view& tv)
   {
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
      headless::open(tv.view_);
#endif
   }
}

TEST_CASE("leds page: leds", "[leds_page]")
{
   test_view tv{extent{100, 100}};
   open(tv);

   // Off at first; the off color the on color at a quarter.
   auto l = led(colors::red, 10);
   CHECK(!l.value());
   CHECK(l.on_color() == colors::red);
   CHECK(l.off_color() == colors::red.level(0.25));

   // A square twice its size, the room for its glow.
   basic_context bctx{tv.view_, tv.cnv};
   auto lim = l.limits(bctx);
   CHECK(lim.min == point{20, 20});
   CHECK(lim.max == point{20, 20});

   // value lights it and puts it out; the colors can change.
   l.value(true);
   CHECK(l.value());
   l.value(false);
   CHECK(!l.value());
   l.off_color(colors::black);
   CHECK(l.off_color() == colors::black);

   // The default size is 12, with an off color of its own.
   auto m = led(colors::green, 12, colors::dim_gray);
   CHECK(m.limits(bctx).min == point{24, 24});
   CHECK(m.off_color() == colors::dim_gray);
   CHECK(led().limits(bctx).min == point{24, 24});
}

#if !defined(ARTIST_RECORDING)
TEST_CASE("leds page: figure", "[leds_page]")
{
   test_view tv{extent{320, 120}, 2};
   open(tv);
   color const cs[] = {colors::red, colors::lime_green, colors::gold,
      colors::dodger_blue};

   htile_composite lit, unlit;
   for (auto c : cs)
   {
      auto on = share(led(c, 16));
      on->value(true);
      lit.push_back(share(hsize(80, align_center(hold(on)))));
      unlit.push_back(share(hsize(80, align_center(led(c, 16)))));
   }
   tv.view_.content(
      align_center_middle(vtile(lit, margin_top(8, unlit))),
      box(bkd_color));
   tv.view_.layout();
   tv.draw();
   tv.img.save_png(std::string{RESULTS_PATH} + "leds.png");
}
#endif
