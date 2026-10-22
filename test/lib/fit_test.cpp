/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Fit: the view's scale is set so the subject's minimum size fits the
   view, the proportion is the view's aspect ratio, the subject is laid
   out from the left-top up to its maximum size, an aligned subject
   centers in the space over, and a bounded fit keeps its scale, and the
   window, within its range.
=============================================================================*/
#include "test_support.hpp"

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   void open(test_view& tv)
   {
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
      headless::open(tv.view_);
#endif
   }

   // Records the bounds it is laid out in
   struct probe : element
   {
      view_limits limits(basic_context const&) const override
      {
         return {{320, 190}, {320, 190}};
      }
      void layout(context const& ctx) override { *bounds = ctx.bounds; }
      std::shared_ptr<rect> bounds = std::make_shared<rect>();
   };

   // Draws the view until the scale the fit posts has been set
   void settle(test_view& tv)
   {
      for (int i = 0; i != 4; ++i)
      {
         tv.draw();
         tv.view_.poll();
      }
   }
}

TEST_CASE("fit: the scale fits the view box", "[fit]")
{
   test_view tv{extent{640, 500}};
   open(tv);
   probe p;
   auto bounds = p.bounds;
   tv.view_.content(fit(p));
   settle(tv);

   // 640 / 320 = 2, 500 / 190 = 2.63: the width decides
   CHECK(tv.view_.scale() == Approx(2.0f));
   CHECK(tv.view_.aspect_ratio() == Approx(320.0f / 190.0f));

   // Left-top, at its size
   CHECK(bounds->left == Approx(0));
   CHECK(bounds->top == Approx(0));
   CHECK(bounds->width() == Approx(320));
   CHECK(bounds->height() == Approx(190));
}

TEST_CASE("fit: an aligned subject centers in the space over", "[fit]")
{
   test_view tv{extent{640, 500}};
   open(tv);
   probe p;
   auto bounds = p.bounds;
   tv.view_.content(fit(align_center_middle(p)));
   settle(tv);

   // The view is 250 high at scale 2: 60 over, 30 above and below
   CHECK(tv.view_.scale() == Approx(2.0f));
   CHECK(bounds->left == Approx(0));
   CHECK(bounds->top == Approx(30));
   CHECK(bounds->height() == Approx(190));
}

TEST_CASE("fit: a bounded fit keeps its scale in range", "[fit]")
{
   // 0.75 to 1.5 of a 320 by 190 box: the limits, in the window's units,
   // are 240 by 142.5 to 480 by 285, whatever the view's scale.
   test_view tv{extent{1000, 1000}};
   open(tv);
   probe p;
   auto bounds = p.bounds;
   tv.view_.content(fit(0.75f, 1.5f, p));
   settle(tv);

   // The view is larger than the range allows: the scale stops at 1.5,
   // and the space over is left.
   CHECK(tv.view_.scale() == Approx(1.5f));
   CHECK(bounds->width() == Approx(320));

   auto l = tv.view_.limits();
   CHECK(l.min.x == Approx(240));
   CHECK(l.min.y == Approx(142.5));
   CHECK(l.max.x == Approx(480));
   CHECK(l.max.y == Approx(285));
}
