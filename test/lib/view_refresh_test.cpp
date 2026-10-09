/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   What a view asks its host to repaint. A host repaints only the areas the
   view invalidates, so an element's refresh has to name that element's
   area, and a whole-view refresh the whole view. These read the requests
   the headless host records.
=============================================================================*/
#include "test_support.hpp"

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

TEST_CASE("headless: resizing the view reaches its size", "[headless]")
{
   test_view tv{extent{400, 300}};
   tv.view_.size(extent{200, 100});
   CHECK(tv.view_.size() == extent{200, 100});
}

#endif
