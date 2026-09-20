/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   A button's press and release are one gesture, and the view is told so:
   begin_tracking on the press, end_tracking on the release. Anything
   watching on_tracking, a plugin host recording automation for instance,
   sees the click bracketed. The release that lands outside the button is
   the case worth holding onto: it still ends the gesture it began.
=============================================================================*/
#include "test_support.hpp"

#include <string>
#include <vector>

namespace cycfi::elements::test
{
   namespace
   {
      // Every tracking state the view reported, in order.
      struct tracking_log
      {
         explicit tracking_log(view& v)
         {
            v.on_tracking =
               [this](element& e, element::tracking state)
               {
                  who.push_back(&e);
                  switch (state)
                  {
                     case element::begin_tracking: states.push_back("begin"); break;
                     case element::while_tracking: states.push_back("while"); break;
                     case element::end_tracking:   states.push_back("end");   break;
                     default:                      states.push_back("?");     break;
                  }
               };
         }

         std::vector<std::string>   states;
         std::vector<element*>      who;
      };

      // A button filling the view, so a press inside it is the middle and
      // a press outside it is off the element altogether.
      template <typename Button>
      struct button_view : test_view
      {
         explicit button_view(Button b)
          : test_view{{200, 100}}
          , button{share(std::move(b))}
          , log{view_}
         {
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
            headless::open(view_);     // a host's window appearing
#endif
            view_.content(hold(button));
            view_.layout();
            draw();                    // the hit test wants laid out bounds
         }

         mouse_button press(point p, bool down) const
         {
            mouse_button btn{};
            btn.down = down;
            btn.state = mouse_button::left;
            btn.num_clicks = 1;
            btn.pos = p;
            return btn;
         }

         void click_at(point down_at, point up_at)
         {
            view_.click(press(down_at, true));
            view_.click(press(up_at, false));
         }

         // A button is only as tall as its label, and sits at the top of
         // the view, so the middle of the view is not on it.
         point inside() const  { return {100, 14}; }
         point outside() const { return {400, 400}; }

         std::shared_ptr<Button>  button;
         tracking_log             log;
      };

      auto toggle()   { return toggle_button("toggle", 1.0); }
      auto momentary(){ return button("momentary", 1.0); }
      auto latching() { return latching_button("latching", 1.0); }
   }
}

using namespace cycfi::elements;
using namespace cycfi::elements::test;

TEST_CASE("A toggle's click is one gesture, begin to end")
{
   button_view v{toggle()};
   int clicks = 0;
   v.button->on_click = [&clicks](bool) { ++clicks; };

   v.click_at(v.inside(), v.inside());

   CHECK(v.log.states == std::vector<std::string>{"begin", "end"});
   CHECK(clicks == 1);
   CHECK(v.button->value() == true);
}

TEST_CASE("A momentary button's click is one gesture, begin to end")
{
   button_view v{momentary()};
   v.click_at(v.inside(), v.inside());
   CHECK(v.log.states == std::vector<std::string>{"begin", "end"});
}

TEST_CASE("A latching button's click is one gesture, begin to end")
{
   button_view v{latching()};
   v.click_at(v.inside(), v.inside());
   CHECK(v.log.states == std::vector<std::string>{"begin", "end"});
}

TEST_CASE("Let go outside the button, the gesture still ends")
{
   // The press begins tracking; the release lands elsewhere and takes the
   // early way out of click. Left unended, the view would hold the element
   // and a host would sit with the gesture open.
   button_view v{toggle()};
   int clicks = 0;
   v.button->on_click = [&clicks](bool) { ++clicks; };

   v.view_.click(v.press(v.inside(), true));
   v.view_.drag(v.press(v.outside(), true));
   v.view_.click(v.press(v.outside(), false));

   CHECK(v.log.states == std::vector<std::string>{"begin", "end"});

   // Let go off the button, so the toggle is not flipped and nothing is
   // reported as clicked.
   CHECK(clicks == 0);
   CHECK(v.button->value() == false);
}

TEST_CASE("A momentary button let go outside still ends its gesture")
{
   button_view v{momentary()};

   v.view_.click(v.press(v.inside(), true));
   v.view_.drag(v.press(v.outside(), true));
   v.view_.click(v.press(v.outside(), false));

   CHECK(v.log.states == std::vector<std::string>{"begin", "end"});
}

TEST_CASE("The gesture names the button that was pressed")
{
   button_view v{toggle()};
   v.click_at(v.inside(), v.inside());

   REQUIRE(v.log.who.size() == 2);
   CHECK(v.log.who[0] == v.log.who[1]);
}
