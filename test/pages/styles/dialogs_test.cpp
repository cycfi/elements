/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/styles/dialogs.adoc and
   draws its figures. The figures land in the build's results directory;
   copy them to docs/modules/ROOT/images/styles/ to update the page.
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
   auto constexpr bkd_color = rgba(35, 35, 37, 255);

   void open(test_view& tv)
   {
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
      headless::open(tv.view_);
#endif
   }

   void wait(view& v, std::chrono::milliseconds t)
   {
      std::this_thread::sleep_for(t);
      v.poll();
   }

   bool press(view& v, key_code k)
   {
      return v.key(key_info{k, key_action::press, 0});
   }

   // Open `d` in `tv`, over a plain background.
   void show(test_view& tv, element_ptr d)
   {
      tv.view_.content(box(bkd_color));
      tv.view_.layout();
      tv.draw();
      open_popup(d, tv.view_);
      wait(tv.view_, 20ms);
      wait(tv.view_, 1ms);
      tv.view_.layout();
      tv.draw();
   }

   bool is_open(view& v, element_ptr d)
   {
      auto const& l = v.layers();
      return std::find(l.begin(), l.end(), d) != l.end();
   }
}

TEST_CASE("dialogs page: keys", "[dialogs_page]")
{
   SECTION("dialog1: Enter is OK")
   {
      test_view tv{extent{500, 400}};
      open(tv);
      int ok = 0;
      auto d = dialog1(tv.view_, label("Hello"), [&]{ ++ok; });
      show(tv, d);
      REQUIRE(is_open(tv.view_, d));
      press(tv.view_, key_code::escape);       // the modal takes it
      wait(tv.view_, 1ms);
      CHECK(is_open(tv.view_, d));
      CHECK(ok == 0);
      CHECK(press(tv.view_, key_code::enter));
      wait(tv.view_, 1ms); wait(tv.view_, 1ms);
      CHECK(ok == 1);
      CHECK(!is_open(tv.view_, d));
   }
   SECTION("dialog2: Enter is OK, Escape is Cancel")
   {
      for (auto k : {key_code::enter, key_code::escape})
      {
         test_view tv{extent{500, 400}};
         open(tv);
         int ok = 0, cancel = 0;
         auto d = dialog2(tv.view_, label("Hello"), [&]{ ++ok; }, [&]{ ++cancel; });
         show(tv, d);
         CHECK(press(tv.view_, k));
         wait(tv.view_, 1ms); wait(tv.view_, 1ms);
         CHECK(ok == (k == key_code::enter));
         CHECK(cancel == (k == key_code::escape));
         CHECK(!is_open(tv.view_, d));
      }
   }
   SECTION("dialog2r: both keys are Cancel")
   {
      for (auto k : {key_code::enter, key_code::escape})
      {
         test_view tv{extent{500, 400}};
         open(tv);
         int ok = 0, cancel = 0;
         auto d = dialog2r(tv.view_, label("Hello"), [&]{ ++ok; }, [&]{ ++cancel; });
         show(tv, d);
         CHECK(press(tv.view_, k));
         wait(tv.view_, 1ms); wait(tv.view_, 1ms);
         CHECK(ok == 0);
         CHECK(cancel == 1);
         CHECK(!is_open(tv.view_, d));
      }
   }
   SECTION("dialog0: no keys")
   {
      test_view tv{extent{500, 400}};
      open(tv);
      auto d = dialog0(label("Hello"));
      show(tv, d);
      press(tv.view_, key_code::enter);
      press(tv.view_, key_code::escape);
      wait(tv.view_, 1ms);
      CHECK(is_open(tv.view_, d));
   }
}

TEST_CASE("dialogs page: buttons", "[dialogs_page]")
{
   // A click on a button calls its function, then closes the dialog.
   test_view tv{extent{500, 400}};
   open(tv);
   int ok = 0, cancel = 0;
   auto cancel_btn = share(button("Cancel"));
   auto ok_btn = share(button("OK"));
   cancel_btn->on_click = [&](bool) { ++cancel; };
   ok_btn->on_click = [&](bool) { ++ok; };
   auto d = dialog2(tv.view_, label("Hello"), ok_btn, cancel_btn);
   show(tv, d);
   cancel_btn->on_click(true);
   wait(tv.view_, 1ms); wait(tv.view_, 1ms);
   CHECK(cancel == 1);
   CHECK(ok == 0);
   CHECK(!is_open(tv.view_, d));
}

TEST_CASE("dialogs page: message box size", "[dialogs_page]")
{
   // The text box is the theme's message_textbox_size.
   test_view tv{extent{500, 400}};
   basic_context bctx{tv.view_, tv.cnv};
   auto sz = get_theme().message_textbox_size;
   auto lim = message_box0("Hello", icons::info)->limits(bctx);
   CHECK(lim.min.x > sz.x);
   CHECK(lim.min.y >= sz.y);
}

#if !defined(ARTIST_RECORDING)
namespace
{
   // The page's example: examples/dialogs, its content verbatim.
   auto dialog_content()
   {
      auto  check_box1 = check_box("Alpha Stone");
      auto  check_box2 = check_box("The Zorane Gambit");
      auto  check_box3 = check_box("Cael Vosburgh's Exploration");

      check_box1.value(true);
      check_box2.value(true);
      check_box3.value(true);

      return
         hsize(300, simple_heading(
            margin({10, 10, 10, 10},
               vtile(
                  margin_top(10, align_left(check_box1)),
                  margin_top(10, align_left(check_box2)),
                  margin_top(10, align_left(check_box3))
               )
            ),
            "The Thraxian Legacy",
            1.1
         ));
   }

   void save(test_view& tv, char const* name)
   {
      tv.img.save_png(std::string{RESULTS_PATH} + name);
   }
}

TEST_CASE("dialogs page: figures", "[dialogs_page]")
{
   {
      test_view tv{extent{560, 190}, 2};
      open(tv);
      auto content = [](char const* text)
      {
         return margin({20, 20, 20, 20}, align_left_top(label(text)));
      };
      auto w1 = share(basic_child_window("Basic", rect{20, 20, 260, 170}, content("No close button")));
      auto w2 = share(closable_child_window("Closable", rect{300, 20, 540, 170}, content("A close button")));
      tv.view_.content({w2, w1, share(box(bkd_color))});
      tv.view_.layout();
      tv.draw();
      save(tv, "dialogs_child_windows.png");
   }
   {
      test_view tv{extent{520, 330}, 2};
      open(tv);
      auto d = dialog2(tv.view_, dialog_content(), []{}, []{});
      show(tv, d);
      save(tv, "dialogs_example.png");
   }
   {
      test_view tv{extent{560, 330}, 2};
      open(tv);
      auto d = message_box2r(tv.view_,
         "Delete the selected files? This cannot be undone.",
         icons::attention, []{}, []{}, "Delete");
      show(tv, d);
      save(tv, "dialogs_message_box.png");
   }
}
#endif
