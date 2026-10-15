/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Progress and busy bars: a vertical bar fills from the bottom; the busy
   bar's segment keeps its width as it sweeps the whole bar; the parts are
   forwarded as given; and an animation step posted to the view after the
   bar is gone does nothing.
=============================================================================*/
#include "test_support.hpp"
#include <chrono>
#include <thread>

using namespace cycfi::elements;
using namespace cycfi::elements::test;
using namespace std::chrono_literals;

namespace
{
   // Records whether it has been moved from.
   struct part : element
   {
      part() = default;
      part(part const&) = default;
      part(part&& rhs) : element{} { rhs.moved_from = true; }
      bool moved_from = false;
   };

   template <typename Bar>
   rect fg(test_view& tv, Bar& bar, rect bounds)
   {
      context ctx{tv.view_, tv.cnv, &bar, bounds};
      return bar.foreground_bounds(ctx);
   }
}

TEST_CASE("status bar: horizontal fills from the left, vertical from the bottom", "[status_bar]")
{
   test_view tv{extent{200, 200}, 1};
   auto bar = progress_bar(part{}, part{});

   bar.value(0.25);
   CHECK(fg(tv, bar, {0, 0, 100, 20}) == rect{0, 0, 25, 20});
   CHECK(fg(tv, bar, {0, 0, 20, 100}) == rect{0, 75, 20, 100});

   bar.value(0.0);
   CHECK(fg(tv, bar, {0, 0, 20, 100}).height() == 0);
   bar.value(1.0);
   CHECK(fg(tv, bar, {0, 0, 20, 100}) == rect{0, 0, 20, 100});
}

TEST_CASE("status bar: the busy segment keeps its width across the bar", "[status_bar]")
{
   test_view tv{extent{200, 200}, 1};
   auto bar = busy_bar(part{}, part{});
   CHECK(bar.animation_width() == Approx(0.2));

   // What each animation step sets: start at s, end at s + width, both
   // clipped to the bar. Inside the bar the segment is 20 wide; it enters
   // at the left and leaves at the right.
   auto at = [&](double s, rect b)
   {
      bar.start_pos(s);
      bar.value(s + bar.animation_width());
      return fg(tv, bar, b);
   };
   for (double s : {0.2, 0.4, 0.6, 0.8})
   {
      auto r = at(s, {0, 0, 100, 20});
      CHECK(r.left == Approx(100 * s));
      CHECK(r.width() == Approx(20));
   }
   CHECK(at(-0.1, {0, 0, 100, 20}) == rect{0, 0, 10, 20});
   CHECK(at(0.9, {0, 0, 100, 20}) == rect{90, 0, 100, 20});

   // Vertical: up from the bottom.
   auto v = at(0.4, {0, 0, 20, 100});
   CHECK(v.bottom == Approx(60));
   CHECK(v.top == Approx(40));
}

TEST_CASE("status bar: the animation sweeps and stops", "[status_bar]")
{
   auto bar = share(busy_bar(part{}, part{}));
   test_view tv{extent{120, 40}, 1};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(margin({10, 10, 10, 10}, hold(bar)));
   tv.view_.layout();

   bar->start(tv.view_, 1ms);
   CHECK(!bar->is_stopped());
   double last = bar->start_pos();
   bool moved = false;
   for (int i = 0; i != 50; ++i)
   {
      std::this_thread::sleep_for(2ms);
      tv.view_.poll();
      if (bar->start_pos() > last)
         moved = true;
      last = bar->start_pos();
   }
   CHECK(moved);

   bar->stop(tv.view_);
   CHECK(bar->is_stopped());
   CHECK(bar->value() == 0.0);
}

TEST_CASE("status bar: the parts are forwarded as given", "[status_bar]")
{
   part bg;
   auto bar = progress_bar(bg, part{});         // bg named, the foreground a temporary
   CHECK(!bg.moved_from);                       // copied, not moved from
}

TEST_CASE("status bar: an animation step after the bar is gone does nothing", "[status_bar]")
{
   test_view tv{extent{120, 40}, 1};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   {
      auto bar = share(busy_bar(part{}, part{}));
      tv.view_.content(hold(bar));
      tv.view_.layout();
      bar->start(tv.view_, 1ms);
      tv.view_.content(element{});             // the view lets go of the bar
   }                                            // and so does the test: it is destroyed

   // The step already posted runs now; under the address sanitizer, a
   // step that used the bar would be reported.
   for (int i = 0; i != 5; ++i)
   {
      std::this_thread::sleep_for(2ms);
      tv.view_.poll();
   }
   SUCCEED();
}
