/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   The quantized thumbwheel: it settles on the nearest step, and its
   value is where it has settled.
=============================================================================*/
#include "test_support.hpp"
#include <string>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

TEST_CASE("a quantized thumbwheel reads where it has settled", "[thumbwheel]")
{
   // vthumbwheel: a list of N items under a vertical port, quantized to
   // 1 / (N - 1). Each draw passes the value to the wheel, which posts a
   // step toward the nearest item and refreshes, until it lands there.
   auto tw = share(vthumbwheel(5,
      [](std::size_t i) { return share(label("Item " + std::to_string(i + 1))); }));

   test_view tv{extent{140, 60}, 1};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(align_center_middle(hold(tw)));
   tv.view_.layout();
   tv.draw();

   auto* wheel = find_subject<basic_vthumbwheel_element*>(tw.get());
   REQUIRE(wheel);

   tw->value({0, 0.3f});
   for (int i = 0; i != 200 && wheel->align() != 0.25; ++i)
   {
      tv.draw();
      tv.view_.poll();
   }
   CHECK(wheel->align() == Approx(0.25));     // landed on the second item
   CHECK(wheel->value() == Approx(0.25));     // and reads it
   CHECK(tw->value().y == Approx(0.3f));      // the thumbwheel keeps its own
}
