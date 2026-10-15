/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   The view's layers and scale: an element added twice before the poll is
   added once; moving a layer in an empty view does nothing; and a change
   of scale updates the limits at once.
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
}

TEST_CASE("view: an element added twice before the poll is added once", "[view]")
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

TEST_CASE("view: moving a layer in an empty view does nothing", "[view]")
{
   // Under the address sanitizer, a look at the front or back of the empty
   // layers would be reported.
   test_view tv{extent{100, 100}};
   open(tv);
   auto e = share(element{});
   tv.view_.move_to_front(e);
   tv.view_.move_to_back(e);
   tv.view_.poll();
   CHECK(tv.view_.layers().empty());
}

TEST_CASE("view: a change of scale updates the limits at once", "[view]")
{
   test_view tv{extent{100, 100}};
   open(tv);
   view_limits got;
   tv.view_.on_change_limits = [&](view_limits l) { got = l; };
   tv.view_.content(fixed_size({30, 20}, element{}));
   tv.view_.scale(2);
   CHECK(tv.view_.limits().min == point{60, 40});
   CHECK(got.min == point{60, 40});
}
