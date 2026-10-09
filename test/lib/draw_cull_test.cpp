/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Drawing is culled against the canvas clip, so a host that repaints only the
   invalidated area pays only for what lies inside it. Event dispatch must NOT
   be culled that way: an event context carries a 1x1 offscreen canvas, whose
   clip would otherwise cull every element.
=============================================================================*/
#include "test_support.hpp"

using namespace cycfi::elements;
using namespace cycfi::elements::test;

TEST_CASE("draw: everything visible is drawn when nothing clips", "[draw_cull]")
{
   auto a = std::make_shared<probe_element>();
   auto b = std::make_shared<probe_element>();
   auto c = std::make_shared<probe_element>();

   test_view tv;
   tv.view_.content(vtile(hold(a), hold(b), hold(c)));
   REQUIRE(tv.view_.size().x > 0);   // the host window really was made
   tv.draw();

   CHECK(a->draws == 1);
   CHECK(b->draws == 1);
   CHECK(c->draws == 1);
}

TEST_CASE("draw: a clip culls the elements outside it", "[draw_cull]")
{
   auto a = std::make_shared<probe_element>();
   auto b = std::make_shared<probe_element>();
   auto c = std::make_shared<probe_element>();

   test_view tv;
   tv.view_.content(vtile(hold(a), hold(b), hold(c)));

   // Find where the middle one landed, then repaint just that.
   tv.draw();
   auto mid = b->last_bounds;
   REQUIRE(mid.width() > 0);
   a->draws = b->draws = c->draws = 0;

   tv.draw(mid);

   CHECK(b->draws == 1);      // inside the clip
   CHECK(a->draws == 0);      // above it
   CHECK(c->draws == 0);      // below it
}

TEST_CASE("draw: a clip that covers everything culls nothing", "[draw_cull]")
{
   auto a = std::make_shared<probe_element>();
   auto b = std::make_shared<probe_element>();

   test_view tv{extent{400, 300}};
   tv.view_.content(vtile(hold(a), hold(b)));
   tv.draw(rect{0, 0, 400, 300});

   CHECK(a->draws == 1);
   CHECK(b->draws == 1);
}

TEST_CASE("hit testing ignores the canvas clip", "[draw_cull]")
{
   // An event context is built on a 1x1 offscreen canvas (see view.cpp), so
   // culling events the way drawing is culled would find nothing, anywhere.
   auto a = std::make_shared<probe_element>();
   auto b = std::make_shared<probe_element>();

   test_view tv;
   tv.view_.content(vtile(hold(a), hold(b)));
   tv.draw();

   auto mid = b->last_bounds;
   REQUIRE(mid.width() > 0);
   auto p = point{mid.left + mid.width() / 2, mid.top + mid.height() / 2};

   // hit_test runs through a context built on a 1x1 canvas, exactly as the
   // view does for a real click.
   cycfi::artist::image tiny{1, 1, 1};
   offscreen_image offscr{tiny};
   canvas cnv{offscr.context()};
   auto sz = tv.view_.size();
   context ctx{tv.view_, cnv, &tv.view_.main_element(), rect{0, 0, sz.x, sz.y}};

   CHECK(tv.view_.main_element().hit_test(ctx, p, true, false) == b.get());
}

#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS) \
 || defined(ELEMENTS_HOST_UI_LIBRARY_WIN32)
TEST_CASE("view: a view made with only a size has that size", "[view]")
{
   // On Windows this once made a child window with no parent, which Windows
   // does not create, so the view had no window and size() was garbage.
   view v{extent{320, 200}};
   CHECK(v.size() == extent{320, 200});
}
#endif
