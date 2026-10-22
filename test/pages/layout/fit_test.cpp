/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/layout/fit.adoc and draws
   its figures. The figures land in the build's results directory; copy
   them to docs/modules/ROOT/images/layout/ to update the page.
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

   // Draws the view until the scale the fit posts has been set
   void settle(test_view& tv)
   {
      for (int i = 0; i != 4; ++i)
      {
         tv.draw();
         tv.view_.poll();
      }
   }

   // A 320 by 190 panel, the box the figures fit
   auto view_box()
   {
      return fixed_size({320, 190},
         layer(
            align_center_middle(label("320 x 190").font_size(28)),
            rbox(colors::dodger_blue.level(0.5), 8)
         )
      );
   }
}

TEST_CASE("fit page: the scale", "[fit_page]")
{
   // In a view 640 by 500 the width decides: a scale of 2, and the
   // box's proportion is the view's aspect ratio.
   test_view tv{extent{640, 500}};
   open(tv);
   tv.view_.content(fit(view_box()));
   settle(tv);
   CHECK(tv.view_.scale() == Approx(2.0f));
   CHECK(tv.view_.aspect_ratio() == Approx(320.0f / 190.0f));

   // Any size: the scale follows it.
   basic_context bctx{tv.view_, tv.cnv};
   auto l = fit(view_box()).limits(bctx);
   CHECK(l.min == point{0, 0});
   CHECK(l.max == point{full_extent, full_extent});
}

TEST_CASE("fit page: bounded fit", "[fit_page]")
{
   // A 640 by 400 box fitted from 0.75 to 1.5: the window is held
   // between 480 by 300 and 960 by 600.
   test_view tv{extent{640, 400}};
   open(tv);
   auto f = fit(0.75f, 1.5f, fixed_size({640, 400}, element{}));
   CHECK(f.min_scale() == 0.75f);
   CHECK(f.max_scale() == 1.5f);
   tv.view_.content(std::move(f));
   settle(tv);
   auto l = tv.view_.limits();
   CHECK(l.min.x == Approx(480));
   CHECK(l.min.y == Approx(300));
   CHECK(l.max.x == Approx(960));
   CHECK(l.max.y == Approx(600));
}

#if !defined(ARTIST_RECORDING)
TEST_CASE("fit page: figures", "[fit_page]")
{
   {
      test_view tv{extent{300, 260}, 2};
      open(tv);
      tv.view_.content(fit(view_box()), box(bkd_color));
      settle(tv);
      tv.img.save_png(std::string{RESULTS_PATH} + "fit_tall.png");
   }
   {
      test_view tv{extent{420, 190}, 2};
      open(tv);
      tv.view_.content(fit(view_box()), box(bkd_color));
      settle(tv);
      tv.img.save_png(std::string{RESULTS_PATH} + "fit_wide.png");
   }
}
#endif
