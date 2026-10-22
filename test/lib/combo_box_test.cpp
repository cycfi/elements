/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Combo box: it starts with its text as set; the arrow opens the list; a
   pick puts the item's text in the box and calls on_enter with it; typing
   completes from the list, the completion selected, and the right arrow
   takes it.
=============================================================================*/
#include "test_support.hpp"
#include <chrono>
#include <string>
#include <thread>

using namespace cycfi::elements;
using namespace cycfi::elements::test;
using namespace std::chrono_literals;

namespace
{
   void open(test_view& tv)
   {
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
      headless::open(tv.view_);
#endif
   }

   void wait(view& v)
   {
      std::this_thread::sleep_for(1ms);
      v.poll();
   }

   void click(view& v, point p)
   {
      mouse_button b{};
      b.state = mouse_button::left;
      b.num_clicks = 1;
      b.pos = p;
      b.down = true;
      v.click(b);
      b.down = false;
      v.click(b);
   }

   std::shared_ptr<basic_popup_menu_element> menu_of(view& v)
   {
      return std::dynamic_pointer_cast<basic_popup_menu_element>(
         v.layers().back());
   }
}

TEST_CASE("combo_box: a pick sets the text and calls on_enter", "[combo_box]")
{
   test_view tv{extent{300, 300}};
   open(tv);
   auto [combo, text] = combo_box({"E4", "B3", "G3"});
   text->set_text("D2");
   std::string entered;
   text->on_enter = [&entered](std::string_view text)
   {
      entered = std::string{text};
      return true;
   };
   CHECK(text->get_utf8() == "D2");

   auto c = share(std::move(combo));
   tv.view_.content(
      margin({20, 20, 20, 20}, align_left_top(hsize(150, hold(c)))),
      box(rgba(35, 35, 37, 255)));
   tv.view_.layout();
   tv.draw();

   // The arrow is at the right end of the frame
   click(tv.view_, {160, 32});
   wait(tv.view_); wait(tv.view_);
   REQUIRE(menu_of(tv.view_));

   // Down twice, then Enter: B3
   tv.view_.key({key_code::down, key_action::press, 0});
   tv.view_.key({key_code::down, key_action::press, 0});
   tv.view_.key({key_code::enter, key_action::press, 0});
   wait(tv.view_); wait(tv.view_);
   CHECK(text->get_utf8() == "B3");
   CHECK(entered == "B3");
}

TEST_CASE("combo_box: typing completes from the list", "[combo_box]")
{
   test_view tv{extent{300, 300}};
   open(tv);
   auto [combo, text] = combo_box({"E4", "B3", "E2", "F#1"});
   auto c = share(std::move(combo));
   tv.view_.content(
      margin({20, 20, 20, 20}, align_left_top(hsize(150, hold(c)))),
      box(rgba(35, 35, 37, 255)));
   tv.view_.layout();
   tv.draw();

   // Focus the text, then type: e completes to the first E, its rest
   // selected
   click(tv.view_, {50, 32});
   wait(tv.view_);
   tv.view_.text(text_info{'e', 0});
   CHECK(text->get_utf8() == "E4");
   CHECK(text->select_start() == 1);
   CHECK(text->select_end() == 2);

   // 2 types over the completion: E2, nothing more to complete
   tv.view_.text(text_info{'2', 0});
   CHECK(text->get_utf8() == "E2");
   CHECK(text->select_start() == 2);
   CHECK(text->select_end() == 2);

   // A text no item starts with stays as typed
   text->select_all();
   tv.view_.text(text_info{'d', 0});
   CHECK(text->get_utf8() == "d");

   // f completes to F#1; the right arrow takes it, the caret at the end
   text->select_all();
   tv.view_.text(text_info{'f', 0});
   CHECK(text->get_utf8() == "F#1");
   tv.view_.key({key_code::right, key_action::press, 0});
   CHECK(text->get_utf8() == "F#1");
   CHECK(text->select_start() == 3);
   CHECK(text->select_end() == 3);
}
