/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/styles/captions.adoc and
   draws its figure. The figure lands in the build's results directory;
   copy it to docs/modules/ROOT/images/styles/ to update the page.
=============================================================================*/
#include "test_support.hpp"
#include <elements/element/style/caption.hpp>
#include <elements/element/style/dial.hpp>
#include <elements/element/style/slider.hpp>
#include <elements/element/style/track.hpp>
#include <elements/element/style/misc.hpp>
#include <cstdint>
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

   template <typename E>
   point min_size(basic_context const& bctx, E const& e)
   {
      return e.limits(bctx).min;
   }

   // The red of the pixel at (x, y); pixels() is premultiplied B, G, R, A.
   int red_at(cycfi::artist::image const& img, int x, int y)
   {
      auto p = reinterpret_cast<std::uint8_t const*>(img.pixels())
         + 4 * (y * int(img.bitmap_size().x) + x);
      return p[2];
   }
}

TEST_CASE("captions page: spacing", "[captions_page]")
{
   test_view tv{extent{300, 300}};
   basic_context bctx{tv.view_, tv.cnv};

   auto content = fixed_size({100, 40}, box(colors::red));
   auto lbl = min_size(bctx, label("Gain"));
   auto hd = min_size(bctx, heading("Gain"));

   // A caption below or above the content, 5 between them.
   CHECK(min_size(bctx, caption(content, "Gain")).y == 40 + 5 + lbl.y);
   CHECK(min_size(bctx, top_caption(content, "Gain")).y == 40 + 5 + lbl.y);
   CHECK(min_size(bctx, simple_heading(content, "Gain")).y == 40 + 5 + hd.y);

   // A caption at the left, 10 between them.
   CHECK(min_size(bctx, left_caption(content, "Gain")).x == lbl.x + 10 + 100);

   // A size relative to the theme's label font size.
   CHECK(min_size(bctx, caption(content, "Gain", 0.5)).y
      < min_size(bctx, caption(content, "Gain")).y);
}

TEST_CASE("captions page: example", "[captions_page]")
{
   auto knob = []{ return dial(basic_knob<50>(), 0.5); };

   auto eq = simple_heading(
      htile_spaced(20,
         caption(knob(), "Low"),
         caption(knob(), "Mid"),
         caption(knob(), "High")
      ),
      "EQ"
   );

   auto drive = top_caption(align_center(knob()), "Drive");

   test_view tv{extent{300, 300}};
   basic_context bctx{tv.view_, tv.cnv};
   CHECK(eq.limits(bctx).min.x >= 3 * 50 + 2 * 20);
   CHECK(drive.limits(bctx).min.y > 50);
}

#if !defined(ARTIST_RECORDING)
TEST_CASE("captions page: order", "[captions_page]")
{
   // top_caption puts the label above the content, caption below it.
   test_view tv{extent{100, 200}};
   open(tv);
   basic_context bctx{tv.view_, tv.cnv};
   auto lbl_h = label("Gain").limits(bctx).min.y;
   auto content = fixed_size({100, 40}, box(colors::red));

   tv.view_.content(align_top(top_caption(content, "Gain")), box(colors::black));
   tv.view_.layout();
   tv.draw();
   CHECK(red_at(tv.img, 2, 2) < 100);                    // The label's row
   CHECK(red_at(tv.img, 50, int(lbl_h + 5 + 20)) > 200);   // The content

   tv.view_.content(align_top(caption(content, "Gain")), box(colors::black));
   tv.view_.layout();
   tv.draw();
   CHECK(red_at(tv.img, 50, 20) > 200);                  // The content
   CHECK(red_at(tv.img, 2, int(40 + 5 + lbl_h / 2)) < 100);    // The label's row
}

TEST_CASE("captions page: figure", "[captions_page]")
{
   auto knob = []{ return dial(basic_knob<50>(), 0.5); };

   auto fig =
      margin({20, 20, 20, 20},
         htile_spaced(40,
            simple_heading(
               htile_spaced(20,
                  caption(knob(), "Low"),
                  caption(knob(), "Mid"),
                  caption(knob(), "High")
               ),
               "EQ"
            ),
            vtile_spaced(20,
               top_caption(align_center(knob()), "Drive"),
               left_caption(
                  hsize(150, slider(basic_thumb<16>(), basic_fill_track<4>(), 0.6)),
                  "Level"
               )
            )
         )
      );

   point sz;
   {
      test_view m{extent{10, 10}};
      sz = fig.limits(basic_context{m.view_, m.cnv}).min;
   }
   test_view tv{extent{sz.x, sz.y}, 2};
   open(tv);
   tv.view_.content(fig, box(bkd_color));
   tv.view_.layout();
   tv.draw();
   tv.img.save_png(std::string{RESULTS_PATH} + "captions.png");
}
#endif
