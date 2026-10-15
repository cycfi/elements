/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/view.adoc.
=============================================================================*/
#include "test_support.hpp"
#include <chrono>
#include <thread>

using namespace cycfi::elements;
using namespace cycfi::elements::test;
using namespace std::chrono_literals;

namespace
{
   // Content of a fixed size that records its bounds.
   struct content : element
   {
      content(point s) : size{s} {}
      view_limits limits(basic_context const&) const override
      {
         return {size, {full_extent, full_extent}};
      }
      void layout(context const& ctx) override { bounds = ctx.bounds; }
      void draw(context const& ctx) override { drawn = ctx.bounds; }
      point size;
      rect bounds;
      rect drawn;
   };

   void open(test_view& tv)
   {
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
      headless::open(tv.view_);
#endif
   }
}

TEST_CASE("view page: the first layer is on top", "[view_page]")
{
   test_view tv{extent{100, 100}};
   open(tv);
   auto a = share(element{});
   auto b = share(element{});
   tv.view_.content(a, b);
   REQUIRE(tv.view_.layers().size() == 2);
   CHECK(tv.view_.layers().back() == a);         // drawn last
   CHECK(tv.view_.layers().front() == b);
}

TEST_CASE("view page: layers change at the next poll", "[view_page]")
{
   test_view tv{extent{100, 100}};
   open(tv);
   auto base = share(element{});
   auto top = share(element{});
   tv.view_.content(base);
   tv.draw();

   tv.view_.add(top);
   CHECK(!tv.view_.is_open(top));
   tv.view_.poll();
   CHECK(tv.view_.is_open(top));
   CHECK(tv.view_.layers().back() == top);

   tv.view_.move_to_back(top);
   tv.view_.poll();
   CHECK(tv.view_.layers().front() == top);

   tv.view_.remove(top);
   CHECK(tv.view_.is_open(top));
   tv.view_.poll();
   CHECK(!tv.view_.is_open(top));
}

TEST_CASE("view page: an add twice before the poll adds once", "[view_page]")
{
   test_view tv{extent{100, 100}};
   open(tv);
   auto top = share(element{});
   tv.view_.content(share(element{}));
   tv.draw();
   tv.view_.add(top);
   tv.view_.add(top);
   tv.view_.poll();
   CHECK(tv.view_.layers().size() == 2);
}

TEST_CASE("view page: limits", "[view_page]")
{
   test_view tv{extent{100, 100}};
   open(tv);
   view_limits got;
   tv.view_.on_change_limits = [&](view_limits l) { got = l; };
   tv.view_.content(content{{80, 60}});
   CHECK(tv.view_.limits().min == point{80, 60});
   CHECK(got.min == point{80, 60});
}

TEST_CASE("view page: scale", "[view_page]")
{
   // Content that fills the view.
   auto c = share(content{{10, 10}});
   test_view tv{extent{100, 100}};
   open(tv);
   tv.view_.content(hold(c));
   tv.draw();
   CHECK(c->bounds == rect{0, 0, 100, 100});

   // At twice the scale, the content is laid out in a view half as big,
   // in its own units, and its limits double.
   tv.view_.scale(2);
   CHECK(tv.view_.limits().min == point{20, 20});
   tv.draw();
   CHECK(c->bounds == rect{0, 0, 50, 50});
   CHECK(c->drawn == rect{0, 0, 50, 50});
}

#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
// The repainted areas are reported by the headless host only.
TEST_CASE("view page: refresh", "[view_page]")
{
   test_view tv{extent{100, 100}};
   open(tv);
   std::vector<std::shared_ptr<content>> cs;
   vector_composite<vtile_element> rows;
   for (int i = 0; i != 5; ++i)
   {
      cs.push_back(share(content{{100, 20}}));
      rows.push_back(share(hold(cs.back())));
   }
   tv.view_.content(rows);
   tv.draw();
   tv.view_.poll();
   headless::take_invalidated(tv.view_);

   // A few, each where it is.
   tv.view_.refresh(*cs[1]);
   tv.view_.refresh(*cs[3]);
   tv.view_.refresh(*cs[1]);                     // once only
   tv.view_.poll();
   auto r = headless::take_invalidated(tv.view_);
   REQUIRE(r.size() == 2);
   CHECK(r[0] == rect{0, 20, 100, 40});
   CHECK(r[1] == rect{0, 60, 100, 80});

   // More than four: the whole view, once.
   for (auto& c : cs)
      tv.view_.refresh(*c);
   tv.view_.poll();
   r = headless::take_invalidated(tv.view_);
   REQUIRE(r.size() == 1);
   CHECK(r[0] == rect{0, 0, 100, 100});

   // From another thread: an area, and an element. The redraw is on
   // this thread, at the poll.
   std::thread{[&] { tv.view_.refresh(rect{10, 10, 20, 20}); }}.join();
   tv.view_.poll();
   r = headless::take_invalidated(tv.view_);
   REQUIRE(r.size() == 1);
   CHECK(r[0] == rect{10, 10, 20, 20});

   std::thread{[&] { tv.view_.refresh(*cs[2]); }}.join();
   CHECK(headless::take_invalidated(tv.view_).empty());
   tv.view_.poll();
   r = headless::take_invalidated(tv.view_);
   REQUIRE(r.size() == 1);
   CHECK(r[0] == rect{0, 40, 100, 60});
}
#endif

TEST_CASE("view page: post", "[view_page]")
{
   test_view tv{extent{100, 100}};
   open(tv);
   int now = 0, later = 0;
   std::thread{[&] { tv.view_.post([&] { ++now; }); }}.join();
   auto timer = tv.view_.post(5ms, [&] { ++later; });
   CHECK(now == 0);
   tv.view_.poll();
   CHECK(now == 1);
   CHECK(later == 0);
   std::this_thread::sleep_for(10ms);
   tv.view_.poll();
   CHECK(later == 1);

   // A timed post from another thread runs here, at a poll.
   std::thread{[&] { tv.view_.post(5ms, [&] { ++later; }); }}.join();
   std::this_thread::sleep_for(10ms);
   tv.view_.poll();
   CHECK(later == 2);

   // Cancelled through its timer: it does not run.
   timer = tv.view_.post(5ms, [&] { ++later; });
   timer->cancel();
   std::this_thread::sleep_for(10ms);
   tv.view_.poll();
   CHECK(later == 2);
}

TEST_CASE("view page: undo and redo", "[view_page]")
{
   test_view tv{extent{100, 100}};
   int value = 0;
   auto set = [&](int to)
   {
      int from = value;
      value = to;
      tv.view_.add_undo({[&, from] { value = from; return true; },
                         [&, to] { value = to; return true; }});
   };
   set(1);
   set(2);
   CHECK(tv.view_.undo());
   CHECK(value == 1);
   CHECK(tv.view_.has_redo());
   CHECK(tv.view_.redo());
   CHECK(value == 2);
   CHECK(tv.view_.undo());
   set(3);                                       // the redo is gone
   CHECK(!tv.view_.has_redo());
   CHECK(tv.view_.undo());
   CHECK(value == 1);
   CHECK(tv.view_.undo());
   CHECK(value == 0);
   CHECK(!tv.view_.undo());
}

TEST_CASE("view page: a task that no longer applies is dropped", "[view_page]")
{
   test_view tv{extent{100, 100}};
   int value = 0;
   bool stale = false;
   tv.view_.add_undo({[&] { value = 1; return true; },
                      [&] { value = 2; return true; }});
   tv.view_.add_undo({[&] { return !stale; },
                      [&] { return !stale; }});

   // The latest no longer applies: one undo goes on to the one before
   stale = true;
   CHECK(tv.view_.undo());
   CHECK(value == 1);
   CHECK(!tv.view_.has_undo());

   // and it is gone from redo too
   CHECK(tv.view_.redo());
   CHECK(value == 2);
   CHECK(!tv.view_.has_redo());

   // Nothing that applies: false
   tv.view_.add_undo({[] { return false; }, [] { return false; }});
   CHECK(tv.view_.undo());                       // the first task, still live
   CHECK(!tv.view_.undo());
}

TEST_CASE("view page: tracking", "[view_page]")
{
   test_view tv{extent{100, 100}};
   element e;
   std::vector<element::tracking> got;
   tv.view_.on_tracking = [&](element&, element::tracking t)
   {
      got.push_back(t);
   };

   // A change with no beginning gets one; a gesture ends when it says so.
   tv.view_.manage_on_tracking(e, element::tracking::while_tracking);
   tv.view_.manage_on_tracking(e, element::tracking::end_tracking);
   REQUIRE(got.size() == 3);
   CHECK(got[0] == element::tracking::begin_tracking);
   CHECK(got[1] == element::tracking::while_tracking);
   CHECK(got[2] == element::tracking::end_tracking);

   // Or after a second of no change.
   got.clear();
   tv.view_.manage_on_tracking(e, element::tracking::while_tracking);
   std::this_thread::sleep_for(1100ms);
   tv.view_.poll();
   REQUIRE(got.size() == 3);
   CHECK(got[2] == element::tracking::end_tracking);
}

TEST_CASE("view page: drawing is clipped to the area", "[view_page]")
{
   // Five rows; a repaint clipped to the second draws only it.
   std::vector<std::shared_ptr<probe_element>> ps;
   vector_composite<vtile_element> rows;
   for (int i = 0; i != 5; ++i)
   {
      ps.push_back(share(probe_element{{100, 20}}));
      rows.push_back(share(hold(ps.back())));
   }
   test_view tv{extent{100, 100}};
   open(tv);
   tv.view_.content(rows);
   tv.draw();
   for (auto& p : ps)
      CHECK(p->draws == 1);

   tv.draw(rect{0, 25, 100, 35});
   CHECK(ps[1]->draws == 2);
   CHECK(ps[0]->draws == 1);
   CHECK(ps[2]->draws == 1);
}

TEST_CASE("view page: a click on the background ends the focus", "[view_page]")
{
   auto in = share(basic_input_box());
   test_view tv{extent{200, 100}};
   open(tv);
   tv.view_.content(align_left_top(hsize(100, hold(in))));
   tv.draw();

   mouse_button btn{};
   btn.state = mouse_button::left;
   btn.num_clicks = 1;
   btn.down = true;
   btn.pos = {10, 5};                            // on the input box
   tv.view_.click(btn);
   btn.down = false;
   tv.view_.click(btn);
   REQUIRE(in->is_focus());

   btn.pos = {150, 80};                          // on nothing
   btn.down = true;
   tv.view_.click(btn);
   btn.down = false;
   tv.view_.click(btn);
   CHECK(!in->is_focus());
}
