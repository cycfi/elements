/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/elements/child_windows.adoc
   and draws its figure. The figure lands in the build's results
   directory; copy it to docs/modules/ROOT/images/elements/ to update the
   page.
=============================================================================*/
#include "test_support.hpp"
#include <chrono>
#include <string>
#include <thread>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   auto constexpr bkd_color = rgba(35, 35, 37, 255);

   // The page's example, verbatim.
   auto make_window(rect bounds, std::string title)
   {
      auto title_bar = movable(
         layer(
            margin({10, 6, 10, 6}, label(std::move(title))),
            box(rgba(60, 70, 90, 255))
         ));

      auto close = closable(
         momentary_button(
            layer(
               margin({14, 4, 14, 4}, label("Close")),
               rbox(colors::dim_gray, 4)
            )));

      return child_window(bounds,
         layer(
            vtile(
               title_bar,
               margin({12, 12, 12, 12},
                  vtile(
                     label("Drag the title to move."),
                     margin_top(12, align_right(close))
                  ))
            ),
            box(rgba(40, 42, 48, 255))
         ));
   }

   using window_ptr = std::shared_ptr<decltype(make_window({}, ""))>;

   // Content of a fixed minimum size.
   struct sized : element
   {
      view_limits limits(basic_context const&) const override
      {
         return {{60, 40}, {full_extent, full_extent}};
      }
   };

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

   struct windows_view : test_view
   {
      windows_view()
       : test_view{extent{370, 210}, 2}
       , first{share(make_window({20, 20, 20, 20}, "First"))}
       , second{share(make_window({170, 85, 170, 85}, "Second"))}
      {
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
         headless::open(view_);
#endif
         view_.content(first, second, box(bkd_color));
         view_.layout();
         draw();
      }

      window_ptr first, second;
   };
}

TEST_CASE("child windows page: placing", "[child_windows_page]")
{
   // Clamped to the subject's limits, keeping its top left; a size of
   // zero gives the minimum size at the point.
   auto w = share(child_window({100, 100, 100, 100}, sized{}));
   test_view tv{extent{300, 300}};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(w);
   tv.view_.layout();
   tv.draw();
   CHECK(w->bounds() == rect{100, 100, 160, 140});
}

TEST_CASE("child windows page: front and back", "[child_windows_page]")
{
   windows_view wv;
   auto& v = wv.view_;
   REQUIRE(v.layers().back() == wv.first);       // the first is on top

   // A press on the second's body, where the first does not cover it,
   // raises it.
   click(v, {200, 185});
   v.poll();
   CHECK(v.layers().back() == wv.second);
}

TEST_CASE("child windows page: moving", "[child_windows_page]")
{
   windows_view wv;
   auto& v = wv.view_;
   auto before = wv.first->bounds();

   // Drag the first window's title by 30, 20.
   v.click(left({60, 30}, true));
   v.drag(left({90, 50}, true));
   v.click(left({90, 50}, false));
   CHECK(wv.first->bounds() == before.move(30, 20));

   // Not past the view's edges.
   before = wv.first->bounds();
   v.click(left({90, 50}, true));
   v.drag(left({90, -500}, true));
   v.click(left({90, -500}, false));
   CHECK(wv.first->bounds().top >= 0);
}

TEST_CASE("child windows page: closing", "[child_windows_page]")
{
   windows_view wv;
   auto& v = wv.view_;

   // The first window's Close button, at its bottom right.
   auto b = wv.first->bounds();
   click(v, {b.right - 30, b.bottom - 20});
   CHECK(wv.first->is_closing());                // fading out
   CHECK(v.is_open(wv.first));
   for (int i = 0; i != 200 && v.is_open(wv.first); ++i)
   {
      std::this_thread::sleep_for(std::chrono::milliseconds{10});
      v.poll();
   }
   CHECK(!v.is_open(wv.first));

   // Closed, not destroyed: open it again.
   v.add(wv.first);
   v.poll();
   CHECK(v.is_open(wv.first));
}

TEST_CASE("child windows page: example", "[child_windows_page]")
{
   windows_view wv;
   wv.img.save_png(std::string{RESULTS_PATH} + "child_windows_example.png");
}

TEST_CASE("child windows page: resizing", "[child_windows_page]")
{
   // An edge moves, the opposite stays; a corner moves two; the limits
   // stop it.
   auto w = share(child_window({50, 50, 150, 150}, resizable(sized{})));
   test_view tv{extent{300, 300}};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(w);
   tv.view_.layout();
   tv.draw();
   auto& v = tv.view_;
   auto drag = [&](point from, point to)
   {
      v.click(left(from, true));
      v.drag(left(to, true));
      v.click(left(to, false));
   };

   drag({52, 100}, {32, 100});                   // the left edge
   CHECK(w->bounds() == rect{30, 50, 150, 150});
   drag({148, 148}, {168, 128});                 // the bottom right corner
   CHECK(w->bounds() == rect{30, 50, 170, 130});
   drag({168, 100}, {0, 100});                   // to the minimum width, 60
   CHECK(w->bounds().width() == Approx(60));
}

#if !defined(ARTIST_RECORDING)
TEST_CASE("child windows page: the stock windows", "[child_windows_page]")
{
   // examples/child_window: two stock windows over a background, as it
   // opens, then with the second resized, moved and raised.
   std::string const space =
      std::string{EXAMPLES_PATH} + "child_window/resources/deep_space.jpg";
   auto make = [&](rect bounds, char const* title)
   {
      return share(closable_child_window(
         title, bounds, scroller(image{space.c_str()})));
   };
   auto first = make({10, 10, 300, 200}, "Child Window 1");
   auto second = make({60, 60, 350, 250}, "Child Window 2");

   test_view tv{extent{420, 300}, 2};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   auto& v = tv.view_;
   v.content(first, second, box(bkd_color));
   v.layout();
   tv.draw();
   tv.img.save_png(std::string{RESULTS_PATH} + "child_windows_open.png");

   // Resize the second by its bottom right corner, move it by its title,
   // and bring it to the front.
   auto b = second->bounds();
   v.click(left({b.right - 2, b.bottom - 2}, true));
   v.drag(left({b.right + 48, b.bottom + 8}, true));
   v.click(left({b.right + 48, b.bottom + 8}, false));
   b = second->bounds();
   auto const title = point{b.right - 60, b.top + 10};  // clear of the first
   v.click(left(title, true));
   v.drag(left({title.x + 10, title.y + 10}, true));
   v.click(left({title.x + 10, title.y + 10}, false));
   CHECK(second->bounds() == b.move(10, 10));
   v.poll();
   v.layout();
   tv.draw();
   tv.img.save_png(std::string{RESULTS_PATH} + "child_windows_moved.png");
   CHECK(v.layers().back() == second);
}
#endif
