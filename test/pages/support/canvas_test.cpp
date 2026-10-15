/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/support/canvas.adoc.
=============================================================================*/
#include "test_support.hpp"
#include <type_traits>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   // Draws clipped to its bounds, through a state, and records the clip
   // it was given and the one it left.
   struct clipper : element
   {
      void draw(context const& ctx) override
      {
         before = ctx.canvas.clip_extent();
         {
            auto state = ctx.canvas.new_state();
            ctx.canvas.add_rect(ctx.bounds);
            ctx.canvas.clip();
            inside = ctx.canvas.clip_extent();
         }
         after = ctx.canvas.clip_extent();
         bounds = ctx.bounds;
      }

      rect before, inside, after, bounds;
   };
}

TEST_CASE("canvas page: the names", "[canvas_page]")
{
   static_assert(std::is_same_v<canvas, cycfi::artist::canvas>);
   static_assert(std::is_same_v<rect, cycfi::artist::rect>);
   static_assert(std::is_same_v<point, cycfi::artist::point>);
   static_assert(std::is_same_v<extent, cycfi::artist::extent>);
   static_assert(std::is_same_v<color, cycfi::artist::color>);
   CHECK(rgba(255, 0, 0, 255) == colors::red);
}

TEST_CASE("canvas page: one canvas, view coordinates", "[canvas_page]")
{
   // Every element draws on the one canvas the view was given, at its
   // bounds in the view.
   auto a = share(probe_element{{40, 20}});
   auto b = share(probe_element{{40, 20}});
   test_view tv{extent{100, 100}};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(
      align_left_top(margin({10, 10, 0, 0}, vtile(hold(a), hold(b)))));
   tv.view_.layout();
   tv.draw();
   CHECK(a->last_bounds == rect{10, 10, 50, 30});
   CHECK(b->last_bounds == rect{10, 30, 50, 50});
}

TEST_CASE("canvas page: a state leaves the canvas as found", "[canvas_page]")
{
   auto c = share(clipper{});
   test_view tv{extent{100, 100}};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(margin({20, 20, 20, 20}, hold(c)));
   tv.view_.layout();
   tv.draw();
   CHECK(c->inside == c->bounds);
   CHECK(c->after == c->before);
}
