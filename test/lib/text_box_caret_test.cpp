/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   The text box's caret blinks on a step posted to the view. A step posted
   before the box is gone, as when a dialog holding a focused text box
   closes, does nothing.
=============================================================================*/
#include "test_support.hpp"
#include <chrono>
#include <thread>

using namespace cycfi::elements;
using namespace cycfi::elements::test;
using namespace std::chrono_literals;

TEST_CASE("text box: a caret blink after the box is gone does nothing", "[text_box]")
{
   test_view tv{extent{200, 40}, 1};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   {
      auto box = share(basic_text_box{"Hello"});
      tv.view_.content(hold(box));
      tv.view_.layout();
      box->begin_focus(element::from_top);   // a caret at the start
      REQUIRE(box->select_start() == box->select_end());
      tv.draw();                             // posts the caret's blink
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
      REQUIRE(headless::text_caret(tv.view_));
#endif
      tv.view_.content(element{});           // the view lets go of the box
   }                                         // and so does the test: it is destroyed

   // The blink is due 500 ms after the draw; under the address sanitizer,
   // a blink that used the box would be reported.
   auto const end = std::chrono::steady_clock::now() + 700ms;
   while (std::chrono::steady_clock::now() < end)
   {
      std::this_thread::sleep_for(10ms);
      tv.view_.poll();
   }
   SUCCEED();
}
