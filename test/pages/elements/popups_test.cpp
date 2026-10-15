/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/elements/popups.adoc and
   draws its figure. The figure lands in the build's results directory;
   copy it to docs/modules/ROOT/images/elements/ to update the page.
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

   mouse_button left(point p, bool down)
   {
      mouse_button b{};
      b.state = mouse_button::left;
      b.down = down;
      b.num_clicks = 1;
      b.pos = p;
      return b;
   }

   void click(view& v, point p)
   {
      v.click(left(p, true));
      v.click(left(p, false));
   }

}

TEST_CASE("popups page: popups", "[popups_page]")
{
   test_view tv{extent{200, 200}};
   open(tv);
   int under = 0;
   auto below = share(momentary_button(box(colors::red)));
   below->on_click = [&](bool) { ++under; };
   tv.view_.content(hold(below));
   tv.draw();

   // At its bounds, sized to its content; open at the next poll.
   auto p = share(basic_popup(fixed_size({60, 40}, box(colors::blue)),
      rect{20, 20, 20, 20}));
   p->open(tv.view_);
   CHECK(!p->is_open(tv.view_));
   tv.view_.poll();
   CHECK(p->is_open(tv.view_));
   CHECK(p->bounds() == rect{20, 20, 80, 60});

   // A click on its body does not reach what is under it.
   tv.draw();
   click(tv.view_, {40, 40});
   CHECK(under == 0);
   click(tv.view_, {150, 150});
   CHECK(under == 1);

   p->close(tv.view_);
   tv.view_.poll();
   CHECK(!p->is_open(tv.view_));
}

TEST_CASE("popups page: a popup menu closes on a press outside", "[popups_page]")
{
   test_view tv{extent{200, 200}};
   open(tv);
   tv.view_.content(box(colors::black));
   tv.draw();
   auto m = share(basic_popup_menu(fixed_size({60, 40}, box(colors::blue)),
      rect{20, 20, 20, 20}));
   m->open(tv.view_);
   tv.view_.poll();
   tv.draw();
   click(tv.view_, {150, 150});
   tv.view_.poll();
   CHECK(!m->is_open(tv.view_));
}

TEST_CASE("popups page: modals", "[popups_page]")
{
   // A dimmed view under a dialog: the content takes nothing; the dialog
   // takes its clicks.
   test_view tv{extent{200, 200}};
   open(tv);
   int content = 0, dialog = 0;
   auto c = share(momentary_button(box(colors::red)));
   c->on_click = [&](bool) { ++content; };
   auto d = share(momentary_button(fixed_size({60, 30}, box(colors::blue))));
   d->on_click = [&](bool) { ++dialog; };
   tv.view_.content(hold(c));
   tv.draw();

   auto block = share(layer(align_center_middle(hold(d)), ui_block()));
   tv.view_.add(block);
   tv.view_.poll();
   tv.draw();
   CHECK(tv.view_.key(key_info{key_code::a, key_action::press, 0}));
   click(tv.view_, {20, 20});
   CHECK(content == 0);
   click(tv.view_, {100, 100});
   CHECK(dialog == 1);

   tv.view_.remove(block);
   tv.view_.poll();
   click(tv.view_, {20, 20});
   CHECK(content == 1);
}

#if !defined(ARTIST_RECORDING)
TEST_CASE("popups page: example", "[popups_page]")
{
   // The dialog over a busy UI, its controls dimmed and taking no input.
   test_view tv{extent{640, 380}, 2};
   open(tv);
   auto& view_ = tv.view_;
   auto grid = check_box("Show the grid");
   grid.value(true);
   auto snap = radio_button("Snap to lines");
   snap.value(true);
   view_.content(
      margin({20, 20, 20, 20},
         align_left_top(vtile(
            htile(
               button("New"),
               margin_left(8, button("Open")),
               margin_left(8, toggle_button("Lock", 1.0, colors::red.opacity(0.4)))
            ),
            margin_top(16, align_left(check_box("Save automatically"))),
            margin_top(8, align_left(grid)),
            margin_top(16, align_left(snap)),
            margin_top(8, align_left(radio_button("Snap to points")))
         ))),
      box(bkd_color)
   );
   tv.draw();

   // The page's example, verbatim.
   auto block = share(ui_block());
   auto box = message_box1(
      view_, "Delete the file?", icons::attention,
      [&view_, block]() { view_.remove(block); }
   );
   view_.add(block);
   view_.add(box);

   view_.poll();
   view_.layout();
   tv.draw();
   tv.img.save_png(std::string{RESULTS_PATH} + "popups_example.png");

   // OK closes the box, and the block with it.
   CHECK(view_.is_open(block));
   CHECK(view_.is_open(box));
   CHECK(view_.key(key_info{key_code::enter, key_action::press, 0}));
   view_.poll();
   view_.poll();
   CHECK(!view_.is_open(box));
   CHECK(!view_.is_open(block));
}
#endif

#if !defined(ARTIST_RECORDING)
TEST_CASE("popups page: a popup", "[popups_page]")
{
   // A button, and a popup list open below it.
   auto item = [](char const* text, bool hilite = false)
   {
      return layer(
         margin({14, 5, 14, 5}, align_left(label(text))),
         box(hilite? rgba(70, 90, 130, 255) : rgba(0, 0, 0, 0))
      );
   };
   auto size_button = share(momentary_button(
      layer(
         margin({14, 6, 14, 6}, label("Size")),
         rbox(colors::slate_gray, 4)
      )));

   test_view tv{extent{300, 180}, 2};
   open(tv);
   auto& view_ = tv.view_;
   view_.content(
      margin({20, 20, 20, 20}, align_left_top(hold(size_button))),
      box(bkd_color)
   );
   tv.draw();

   rect b;
   view_.in_context_do(*size_button, [&](context const& c) { b = c.bounds; });
   auto list = share(basic_popup(
      layer(
         margin({0, 6, 0, 6},
            vtile(item("Small"), item("Medium", true), item("Large"))),
         rbox(colors::dim_gray, 6)
      ),
      rect{b.left, b.bottom + 4, b.left, b.bottom + 4}));
   list->open(view_);
   view_.poll();
   view_.layout();
   tv.draw();
   tv.img.save_png(std::string{RESULTS_PATH} + "popups_popup.png");
   CHECK(list->bounds().top == Approx(b.bottom + 4));
}
#endif
