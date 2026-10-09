/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   The maximum sizing proxies: max_size caps the height by the height,
   vmax_size reports its cap as the maximum height, and none of the three
   squeezes its subject below the subject's own minimum.
=============================================================================*/
#include "test_support.hpp"

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   // Records the bounds it is drawn in.
   struct recorder : element
   {
      recorder(point min_) : min{min_} {}

      view_limits limits(basic_context const&) const override
      {
         return {min, {full_extent, full_extent}};
      }

      void draw(context const& ctx) override
      {
         bounds = ctx.bounds;
      }

      point min;
      rect  bounds;
   };

   template <typename E>
   rect drawn_in(E e, extent size = {300, 200})
   {
      auto r = share(std::move(e));
      test_view tv{size, 1};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
      headless::open(tv.view_);
#endif
      tv.view_.content(hold(r));
      tv.view_.layout();
      tv.draw();
      return find_subject<recorder*>(r.get())->bounds;
   }
}

TEST_CASE("max_size caps each axis by its own size", "[max_size]")
{
   auto b = drawn_in(max_size({100, 50}, recorder{{10, 10}}));
   CHECK(b.width() == 100);
   CHECK(b.height() == 50);

   // The height is laid out past the cap, at the subject's minimum: it
   // stays there, and is not set to the width's cap.
   auto c = drawn_in(max_size({250, 50}, recorder{{10, 80}}));
   CHECK(c.height() == 80);
}

TEST_CASE("vmax_size reports its cap as the maximum height", "[max_size]")
{
   test_view tv{extent{100, 100}, 1};
   basic_context bctx{tv.view_, tv.cnv};
   auto l = vmax_size(50, recorder{{10, 10}}).limits(bctx);
   CHECK(l.max.y == 50);
   CHECK(l.max.x == full_extent);

   auto h = hmax_size(50, recorder{{10, 10}}).limits(bctx);
   CHECK(h.max.x == 50);
   CHECK(h.max.y == full_extent);
}

TEST_CASE("a cap below the subject's minimum gives way to the minimum", "[max_size]")
{
   // The subject needs 80 in height (and width); the cap asks for 50.
   auto m = drawn_in(max_size({50, 50}, recorder{{80, 80}}));
   CHECK(m.width() == 80);
   CHECK(m.height() == 80);

   auto h = drawn_in(hmax_size(50, recorder{{80, 80}}));
   CHECK(h.width() == 80);

   auto v = drawn_in(vmax_size(50, recorder{{80, 80}}));
   CHECK(v.height() == 80);

   // Above the minimum, the cap holds.
   CHECK(drawn_in(vmax_size(120, recorder{{80, 80}})).height() == 120);
}
