/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/elements/status_bars.adoc
   and draws its figures. The figures land in the build's results
   directory; copy them to docs/modules/ROOT/images/elements/ to update the
   page.
=============================================================================*/
#include "test_support.hpp"
#include <string>

using namespace cycfi::elements;
using namespace cycfi::elements::test;
using namespace std::chrono_literals;

namespace
{
   auto constexpr bkd_color = rgba(35, 35, 37, 255);

   // The example's colours.
   auto constexpr pgold = colors::gold.opacity(0.8);
   auto constexpr bgreen = colors::green.level(0.7).opacity(0.8);

   template <typename Bar>
   rect fg(test_view& tv, Bar& bar, rect bounds)
   {
      context ctx{tv.view_, tv.cnv, &bar, bounds};
      return bar.foreground_bounds(ctx);
   }

   // A horizontal and a vertical bar side by side, as the example has them.
   template <typename Bar>
   auto pair(Bar h, Bar v)
   {
      return margin({20, 12, 20, 12},
         htile_spaced(20,
            align_middle(vsize(18, std::move(h))),
            hsize(18, vsize(96, std::move(v)))
         ));
   }

   // The progress bars at `p`.
   auto progress_frame(double p)
   {
      return pair(
         progress_bar(rbox(colors::black), rbox(pgold), p),
         progress_bar(rbox(colors::black), rbox(pgold), p));
   }

   // The busy bars, their segment starting at `s`.
   auto busy_frame(double s)
   {
      auto h = busy_bar(rbox(colors::black), rbox(bgreen));
      auto v = busy_bar(rbox(colors::black), rbox(bgreen));
      for (auto* b : {&h, &v})
      {
         b->start_pos(s);
         b->value(s + b->animation_width());
      }
      return pair(std::move(h), std::move(v));
   }

   // `frames` frames of 260 by 120, one under the other, saved as a strip.
   template <typename Frame>
   void strip(int frames, Frame frame, char const* name)
   {
      float const w = 260, h = 120;
      test_view tv{extent{w, h * frames}, 2};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
      headless::open(tv.view_);
#endif
      vector_composite<vtile_element> rows;
      for (int i = 0; i != frames; ++i)
         rows.push_back(share(vsize(h, frame(double(i) / frames))));
      tv.view_.content(rows, box(bkd_color));
      tv.view_.layout();
      tv.draw();
      tv.img.save_png(std::string{RESULTS_PATH} + name);
   }
}

TEST_CASE("status bars page: progress", "[status_bars_page]")
{
   test_view tv{extent{200, 200}, 1};
   auto bar = progress_bar(box(colors::black), box(colors::gold), 0.25);
   CHECK(bar.value() == Approx(0.25));
   CHECK(fg(tv, bar, {0, 0, 100, 20}) == rect{0, 0, 25, 20});
   CHECK(fg(tv, bar, {0, 0, 20, 100}) == rect{0, 75, 20, 100});

   bar.value(3.0);                               // clamped
   CHECK(bar.value() == 1.0);

   // A receiver of a double, as a slider is, so a model can drive it.
   receiver<double>& r = bar;
   r.value(0.5);
   CHECK(bar.value() == 0.5);
}

TEST_CASE("status bars page: busy", "[status_bars_page]")
{
   test_view tv{extent{200, 200}, 1};
   auto bar = busy_bar(box(colors::black), box(colors::gold));
   CHECK(bar.is_stopped());
   CHECK(bar.animation_width() == Approx(0.2));

   bar.animation_width(0.3);
   bar.start_pos(0.5);
   bar.value(0.8);
   CHECK(fg(tv, bar, {0, 0, 100, 20}).left == Approx(50));
   CHECK(fg(tv, bar, {0, 0, 100, 20}).right == Approx(80));
}

TEST_CASE("status bars page: start and stop", "[status_bars_page]")
{
   auto bar = share(busy_bar(box(colors::black), box(colors::gold)));
   test_view tv{extent{120, 40}, 1};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(hold(bar));
   tv.view_.layout();
   bar->start(tv.view_, 10ms);
   CHECK(!bar->is_stopped());
   bar->stop(tv.view_);
   CHECK(bar->is_stopped());
   CHECK(bar->value() == 0.0);
}

TEST_CASE("status bars page: animations", "[status_bars_page]")
{
   // The progress bars fill, then rest full before the loop starts again.
   strip(60, [](double t) { return progress_frame(std::min(1.0, t * 1.25)); },
      "status_bars_progress.png");

   // The busy segment enters at one end and leaves at the other, twice a
   // loop, so the strip loops without a jump.
   strip(60, [](double t) { return busy_frame(-0.2 + std::fmod(t * 2, 1.0) * 1.2); },
      "status_bars_busy.png");
}
