/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Draws the funny slider animation of docs/modules/ROOT/pages/aspects.adoc:
   frames of the slider, its thumb sweeping down and back, one under the
   other. The strip lands in the build's results directory; turn it into
   the page's animation with docs/scripts/strip_to_webp.py.
=============================================================================*/
#include "test_support.hpp"
#include <cmath>
#include <numbers>
#include <string>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   auto constexpr bkd_color = rgba(35, 35, 37, 255);

   // The page's slider, verbatim.
   auto make_funny_slider()
   {
      auto track = hsize(10, box(colors::black));
      auto thumb =
         layer(
            margin(
               {25, 20, 25, 18},
               label("“Dogs are my favorite people”")
            ),
            margin(
               {5, 5, 5, 5},
               rbox(colors::medium_blue, 10)
            )
         );

      return slider(thumb, track);
   }
}

#if !defined(ARTIST_RECORDING)
TEST_CASE("aspects page: the funny slider animation", "[aspects_page]")
{
   // 60 frames of 430 by 255, the content of the page's window
   // screenshots, at twice the pixels. The value eases from the top to the
   // bottom and back, so the animation loops without a jump.
   int const frames = 60;
   float const w = 430, h = 255;

   test_view tv{extent{w, h * frames}, 2};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   vector_composite<vtile_element> rows;
   std::vector<std::shared_ptr<basic_slider_base>> sliders;
   for (int i = 0; i != frames; ++i)
   {
      auto s = share(make_funny_slider());
      auto t = double(i) / frames;
      s->value(0.5 - 0.5 * std::cos(2 * std::numbers::pi * t));
      sliders.push_back(s);
      rows.push_back(share(vsize(h, layer(
         align_center(margin({0, 10, 0, 10}, hold(s))),
         box(bkd_color)
      ))));
   }
   tv.view_.content(rows);
   tv.view_.layout();
   tv.draw();
   tv.img.save_png(std::string{RESULTS_PATH} + "aspects_slider.png");
   CHECK(sliders[frames / 2]->value() == Approx(1.0));
}
#endif
