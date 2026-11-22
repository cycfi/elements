/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   What a view asks its host to repaint. A host repaints only the areas the
   view invalidates, so an element's refresh has to name that element's
   area, and a whole-view refresh the whole view; a click asks only for
   what it changes. These read the requests the headless host records.
=============================================================================*/
#include "test_support.hpp"
#include <algorithm>

#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)

using namespace cycfi::elements;
using namespace cycfi::elements::test;

TEST_CASE("refresh: an element's refresh asks for its own area", "[refresh]")
{
   auto a = std::make_shared<probe_element>();
   auto b = std::make_shared<probe_element>();

   test_view tv;
   tv.view_.content(vtile(hold(a), hold(b)));
   tv.draw();
   headless::take_invalidated(tv.view_);

   tv.view_.refresh(*b);
   tv.view_.poll();     // refresh is posted to the view's io_context
   auto areas = headless::take_invalidated(tv.view_);

   REQUIRE(areas.size() == 1);
   INFO("element " << b->last_bounds.left << ',' << b->last_bounds.top
      << ' ' << b->last_bounds.right << ',' << b->last_bounds.bottom);
   CHECK(cycfi::artist::intersects(areas[0], b->last_bounds));
   CHECK(!cycfi::artist::intersects(areas[0], a->last_bounds));
}

TEST_CASE("refresh: a whole-view refresh asks for the whole view", "[refresh]")
{
   test_view tv{extent{400, 300}};
   tv.view_.content(hold(std::make_shared<probe_element>()));
   tv.draw();
   headless::take_invalidated(tv.view_);

   tv.view_.refresh();
   tv.view_.poll();
   auto areas = headless::take_invalidated(tv.view_);

   REQUIRE(areas.size() == 1);
   CHECK(areas[0] == rect{0, 0, 400, 300});
}

namespace
{
   mouse_button left(point p, bool down)
   {
      mouse_button b{};
      b.state = mouse_button::left;
      b.down = down;
      b.num_clicks = 1;
      b.pos = p;
      return b;
   }

   void click(test_view& tv, point p)
   {
      tv.view_.click(left(p, true));
      tv.view_.poll();
      tv.view_.click(left(p, false));
      tv.view_.poll();
   }

   rect bounds_of(test_view& tv, element& e)
   {
      rect r;
      tv.view_.in_context_do(e, [&](context const& ctx) { r = ctx.bounds; });
      return r;
   }

   point center(rect r)
   {
      return {(r.left + r.right) / 2, (r.top + r.bottom) / 2};
   }

   // Takes clicks and does nothing with them: its look never changes.
   struct inert_control : probe_element
   {
      bool click(context const&, mouse_button) override { return true; }
   };
}

TEST_CASE("refresh: a click that changes nothing asks for no repaint", "[refresh]")
{
   auto e = std::make_shared<inert_control>();

   test_view tv{extent{400, 300}};
   tv.view_.content(align_center_middle(hold(e)));
   tv.draw();
   headless::take_invalidated(tv.view_);

   click(tv, center(e->last_bounds));
   CHECK(headless::take_invalidated(tv.view_).empty());
}

TEST_CASE("refresh: a click on a button repaints the button, not the view", "[refresh]")
{
   auto b = share(button("Press"));

   test_view tv{extent{400, 300}};
   tv.view_.content(align_center_middle(hsize(100, hold(b))));
   tv.draw();
   headless::take_invalidated(tv.view_);

   auto bounds = bounds_of(tv, *b);
   click(tv, center(bounds));
   auto areas = headless::take_invalidated(tv.view_);
   REQUIRE(!areas.empty());
   for (auto const& a : areas)
      CHECK(a == bounds);
}

TEST_CASE("refresh: a click that moves the focus repaints both text boxes", "[refresh]")
{
   auto a = share(basic_text_box{"Alpha"});
   auto b = share(basic_text_box{"Bravo"});

   test_view tv{extent{400, 300}};
   tv.view_.content(align_left_top(hsize(200, vtile(hold(a), hold(b)))));
   tv.draw();
   auto ra = bounds_of(tv, *a);
   auto rb = bounds_of(tv, *b);

   auto has = [](std::vector<rect> const& areas, rect r)
   {
      return std::find(areas.begin(), areas.end(), r) != areas.end();
   };
   auto whole = rect{0, 0, 400, 300};

   click(tv, center(ra));                    // the focus goes to a
   REQUIRE(a->is_focus());
   headless::take_invalidated(tv.view_);

   click(tv, center(rb));                    // and moves to b
   REQUIRE(b->is_focus());
   auto areas = headless::take_invalidated(tv.view_);
   CHECK(has(areas, ra));
   CHECK(has(areas, rb));
   CHECK(!has(areas, whole));

   click(tv, {390, 290});                    // the background takes it from b
   REQUIRE(!b->is_focus());
   areas = headless::take_invalidated(tv.view_);
   CHECK(has(areas, rb));
   CHECK(!has(areas, whole));
}

TEST_CASE("headless: resizing the view reaches its size", "[headless]")
{
   test_view tv{extent{400, 300}};
   tv.view_.size(extent{200, 100});
   CHECK(tv.view_.size() == extent{200, 100});
}

#endif
