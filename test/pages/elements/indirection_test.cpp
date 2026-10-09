/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/elements/indirection.adoc.
=============================================================================*/
#include "test_support.hpp"
#include <string>
#include <vector>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   // An element that records the bounds it was drawn with.
   struct bounds_probe : element
   {
      view_limits limits(basic_context const&) const override
      {
         return {{20, 20}, {full_extent, full_extent}};
      }

      void draw(context const& ctx) override
      {
         last_bounds = ctx.bounds;
         ++draws;
      }

      bool wants_control() const override { return true; }

      rect  last_bounds = {};
      int   draws = 0;
   };

   template <typename Content>
   void show(test_view& tv, Content&& content)
   {
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
      headless::open(tv.view_);
#endif
      tv.view_.content(std::forward<Content>(content));
      tv.view_.layout();
      tv.draw();
   }

   mouse_button left_button(point p, bool down)
   {
      mouse_button btn{};
      btn.down = down;
      btn.state = mouse_button::left;
      btn.num_clicks = 1;
      btn.pos = p;
      return btn;
   }
}

TEST_CASE("indirection page: hold and link share one element", "[indirection_page]")
{
   // One element, in two places, through hold and link; each place gives
   // it its own bounds.
   auto probe = share(bounds_probe{});
   std::vector<rect> seen;

   test_view tv{extent{200, 100}, 1};
   show(tv, htile(hold(probe), link(*probe)));

   // The second draw's bounds are what the probe keeps; the first draw
   // was the left half.
   CHECK(probe->draws == 2);
   CHECK(probe->last_bounds == rect{100, 0, 200, 100});

   // The indirects forward their protocol to the one element.
   auto h = hold(probe);
   basic_context bctx{tv.view_, tv.cnv};
   CHECK(h.limits(bctx).min == probe->limits(bctx).min);
   CHECK(h.wants_control());
   CHECK(&h.get() == probe.get());
}

TEST_CASE("indirection page: one slider in two places", "[indirection_page]")
{
   // The page's second example.
   auto vol = share(slider(basic_thumb<20>(), basic_track<5, false>(), 0.5));

   test_view tv{extent{200, 200}, 1};
   show(tv,
      vtile(
         hold(vol),
         margin({20, 20, 20, 20}, hold(vol))
      )
   );
   vol->value(0.75);
   CHECK(vol->value() == Approx(0.75));
   tv.draw();                          // both places draw the one slider
}
