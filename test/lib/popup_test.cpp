/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Popups and tooltips: a tooltip destroyed with its tip's step still
   posted is not touched by it; the cursor over a popup refreshes the
   popup, not the view; a tip that does not fit on its side goes to the
   other, and stays in the view; a bubble's pointer is on the side facing
   the element.
=============================================================================*/
#include "test_support.hpp"
#include <chrono>
#include <thread>

using namespace cycfi::elements;
using namespace cycfi::elements::test;
using namespace std::chrono_literals;

namespace
{
   void open(test_view& tv)
   {
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
      headless::open(tv.view_);
#endif
   }

   void wait(view& v, std::chrono::milliseconds t)
   {
      std::this_thread::sleep_for(t);
      v.poll();
   }

   // The tip a tooltip opened: the view's top layer.
   std::shared_ptr<basic_popup_element> tip_of(view& v)
   {
      return std::dynamic_pointer_cast<basic_popup_element>(v.layers().back());
   }

   // A tooltip with a 40 by 20 tip, its element at `at`.
   rect tip_bounds_at(rect at, tooltip_position pos)
   {
      test_view tv{extent{200, 200}};
      open(tv);
      auto t = share(tooltip(element{}, fixed_size({40, 20}, element{}),
         1ms, pos));
      tv.view_.content(hold(t));
      tv.draw();
      context ctx{tv.view_, tv.cnv, t.get(), at};
      t->cursor(ctx, center_point(at), cursor_tracking::entering);
      wait(tv.view_, 10ms);
      wait(tv.view_, 1ms);
      auto tip = tip_of(tv.view_);
      return tip? tip->bounds() : rect{};
   }

   // The red of the pixel at (x, y); pixels() is premultiplied B, G, R, A.
   int red_at(cycfi::artist::image const& img, int x, int y)
   {
      auto p = reinterpret_cast<std::uint8_t const*>(img.pixels());
      return p[4 * (y * int(img.bitmap_size().x) + x) + 2];
   }
}

TEST_CASE("tooltip: destroyed with its step posted", "[popup]")
{
   // Under the address sanitizer, a step that touched the tooltip would be
   // reported.
   test_view tv{extent{200, 200}};
   open(tv);
   {
      auto t = share(tooltip(box(colors::red), label("tip"), 5ms));
      tv.view_.content(align_center_middle(hsize(50, vsize(50, hold(t)))));
      tv.draw();
      context ctx{tv.view_, tv.cnv, t.get(), rect{75, 75, 125, 125}};
      t->cursor(ctx, {100, 100}, cursor_tracking::entering);
      tv.view_.content(element{});            // the view lets go of it
   }                                           // and so does the test
   wait(tv.view_, 20ms);                       // the step runs now
   CHECK(tv.view_.layers().size() == 1);       // and opened no tip
}

#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
// The repainted areas are reported by the headless host only.
TEST_CASE("popup: the cursor refreshes the popup alone", "[popup]")
{
   test_view tv{extent{400, 400}};
   open(tv);
   auto b = share(momentary_button(box(colors::red)));
   auto p = share(basic_popup(hold(b), rect{10, 10, 60, 60}));
   tv.view_.content(box(colors::black));
   tv.draw();
   p->open(tv.view_);
   tv.view_.poll();
   tv.draw();
   headless::take_invalidated(tv.view_);

   tv.view_.cursor({30, 30}, cursor_tracking::hovering);
   tv.view_.poll();
   auto areas = headless::take_invalidated(tv.view_);
   REQUIRE(!areas.empty());
   for (auto r : areas)
      CHECK(r.width() <= 50);
}
#endif

TEST_CASE("tooltip: the tip stays in the view", "[popup]")
{
   // Above the element by default.
   CHECK(tip_bounds_at({80, 100, 120, 120}, tooltip_position::top)
      == rect{80, 80, 120, 100});

   // At the top of the view: no room above, so below.
   CHECK(tip_bounds_at({80, 0, 120, 20}, tooltip_position::top)
      == rect{80, 20, 120, 40});

   // At the right edge: moved left, into the view.
   CHECK(tip_bounds_at({185, 100, 200, 120}, tooltip_position::top).right
      == 200);
}

#if !defined(ARTIST_RECORDING)
TEST_CASE("tooltip: a bubble points at its element", "[popup]")
{
   // A bubble tip on a black view, for the element at `at`; the red of the
   // pixel at `p` once it is open.
   auto red_after = [](rect at, point p)
   {
      test_view tv{extent{200, 200}, 1};
      open(tv);
      auto t = share(tooltip(element{}, tooltip_bubble("tip"), 1ms));
      tv.view_.content(layer(hold(t), box(colors::black)));
      tv.draw();
      context ctx{tv.view_, tv.cnv, t.get(), at};
      t->cursor(ctx, center_point(at), cursor_tracking::entering);
      wait(tv.view_, 10ms);
      wait(tv.view_, 1ms);
      tv.draw();
      return red_at(tv.img, int(p.x), int(p.y));
   };

   // Above the element: the pointer, just above its top edge.
   CHECK(red_after({80, 100, 120, 120}, {100, 97}) > 200);

   // At the top of the view, flipped below: the pointer, just below its
   // bottom edge.
   CHECK(red_after({80, 0, 120, 20}, {100, 23}) > 200);
}
#endif

namespace
{
   void click_at(view& v, point p)
   {
      mouse_button b{};
      b.state = mouse_button::left;
      b.num_clicks = 1;
      b.pos = p;
      b.down = true;
      v.click(b);
      b.down = false;
      v.click(b);
      wait(v, 2ms);
   }
}

TEST_CASE("tooltip: a tip does not take the focus", "[popup]")
{
   // The cursor stays over a text box being typed in, and its tip opens:
   // typing still goes to the text box, even when the tip could take the
   // focus itself.
   test_view tv{extent{300, 200}};
   open(tv);
   auto in = share(basic_input_box("Name"));
   auto t = share(tooltip(input_box(hold(in)), basic_text_box("The tip"), 1ms));
   tv.view_.content(align_left_top(hsize(200, hold(t))), box(colors::black));
   tv.view_.layout();
   tv.draw();
   auto const base = tv.view_.layers().size();

   point const at{100, 15};
   click_at(tv.view_, at);
   tv.view_.text(text_info{'a', 0});
   REQUIRE(in->get_text() == U"a");

   tv.view_.cursor(at, cursor_tracking::hovering);
   wait(tv.view_, 10ms);
   wait(tv.view_, 2ms);
   REQUIRE(tv.view_.layers().size() == base + 1);

   tv.view_.text(text_info{'b', 0});
   CHECK(in->get_text() == U"ab");
}

TEST_CASE("tooltip: a tip whose height follows its width is placed once laid out", "[popup]")
{
   // Wrapped text is one line tall until it is laid out. A tip above its
   // element ends at the element's top the first time it opens too, not
   // only once it has been laid out before.
   test_view tv{extent{300, 300}};
   open(tv);
   auto t = share(tooltip(
      fixed_size({100, 20}, element{}),
      hsize(200, static_text_box(
         "A tip long enough to wrap onto several lines, so its height "
         "is known only once it is laid out at its width.")),
      1ms, tooltip_position::top));
   tv.view_.content(align_center_bottom(hold(t)), box(colors::black));
   tv.view_.layout();
   tv.draw();

   rect const at{100, 280, 200, 300};
   context ctx{tv.view_, tv.cnv, t.get(), at};
   t->cursor(ctx, center_point(at), cursor_tracking::entering);
   wait(tv.view_, 10ms);
   wait(tv.view_, 2ms);
   auto tip = tip_of(tv.view_);
   REQUIRE(tip);
   tv.draw();
   CHECK(tip->bounds().height() > 40);
   CHECK(tip->bounds().bottom <= at.top);
}

TEST_CASE("tooltip: disarmed, no tip shows", "[popup]")
{
   // Two tooltips share one switch. Disarmed, hovering either opens
   // nothing; armed again, the tip opens. Disarmed during the delay, the
   // tip does not open either.
   test_view tv{extent{200, 200}};
   open(tv);
   auto a = share(tooltip(element{}, label("a"), 1ms));
   auto b = share(tooltip(element{}, label("b"), 1ms));
   b->armed = a->armed;
   tv.view_.content(hold(a));
   tv.draw();
   auto const base = tv.view_.layers().size();
   rect const at{80, 100, 120, 120};
   context ctx{tv.view_, tv.cnv, a.get(), at};

   auto hover = [&](auto& t)
   {
      t->cursor(ctx, center_point(at), cursor_tracking::entering);
      wait(tv.view_, 10ms);
      wait(tv.view_, 1ms);
      auto n = tv.view_.layers().size();
      t->cursor(ctx, {0, 0}, cursor_tracking::leaving);
      wait(tv.view_, 1ms);
      return n;
   };

   *a->armed = false;
   CHECK(hover(a) == base);
   CHECK(hover(b) == base);

   *a->armed = true;
   CHECK(hover(a) == base + 1);
   CHECK(hover(b) == base + 1);

   a->cursor(ctx, center_point(at), cursor_tracking::entering);
   *a->armed = false;
   wait(tv.view_, 10ms);
   wait(tv.view_, 1ms);
   CHECK(tv.view_.layers().size() == base);
}
