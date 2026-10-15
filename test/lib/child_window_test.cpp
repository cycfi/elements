/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Child windows: a click on a window inside another element reaches it,
   and does not post itself again; a quick click on a window behind
   another brings it to the front and reaches it pressed, then released,
   in that order; a click on a window's body stays with the window; a
   floating element keeps its top left as it takes its subject's size.
=============================================================================*/
#include "test_support.hpp"
#include <chrono>
#include <thread>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   struct counter : element
   {
      view_limits limits(basic_context const&) const override
      {
         return {{50, 50}, {full_extent, full_extent}};
      }
      bool click(context const&, mouse_button btn) override
      {
         (btn.down? downs : ups)++;
         last_down = btn.down;
         return true;
      }
      element* hit_test(context const& ctx, point p, bool, bool) override
      {
         return ctx.bounds.includes(p)? this : nullptr;
      }
      bool wants_control() const override { return true; }
      int downs = 0, ups = 0;
      bool last_down = false;
   };

   // Poll until w has left the view, a close's fade and all: at most two
   // seconds.
   template <typename W>
   void wait_closed(view& v, W const& w)
   {
      for (int i = 0; i != 200 && v.is_open(w); ++i)
      {
         std::this_thread::sleep_for(std::chrono::milliseconds{10});
         v.poll();
      }
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
}

TEST_CASE("child window: a window that is not a layer takes the click", "[child_window]")
{
   auto c = share(counter{});
   test_view tv{extent{200, 200}};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(
      layer(child_window({10, 10, 110, 110}, hold(c)), box(colors::black)));
   tv.draw();
   tv.view_.click(left({50, 50}, true));
   for (int i = 0; i != 5; ++i)
      tv.view_.poll();
   tv.view_.click(left({50, 50}, false));
   tv.view_.poll();
   CHECK(c->downs == 1);
   CHECK(c->ups == 1);
}

TEST_CASE("child window: a quick click brings it to the front, in order", "[child_window]")
{
   auto a = share(counter{});
   auto b = share(counter{});
   test_view tv{extent{200, 200}};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   auto b_window = share(child_window({80, 80, 180, 180}, hold(b)));
   tv.view_.content(
      child_window({10, 10, 110, 110}, hold(a)),
      b_window
   );
   tv.draw();
   // b is at the back; a quick click on it, released before the poll.
   tv.view_.click(left({150, 150}, true));
   tv.view_.click(left({150, 150}, false));
   tv.view_.poll();
   tv.view_.poll();
   CHECK(b->downs == 1);
   CHECK(b->ups == 1);
   CHECK(!b->last_down);                         // released last
   CHECK(tv.view_.layers().back() == b_window);
}

TEST_CASE("child window: a click on its body does not fall through", "[child_window]")
{
   // A button behind a window, under the window's plain body.
   int clicks = 0;
   auto under = share(momentary_button(box(colors::red)));
   under->on_click = [&](bool) { ++clicks; };
   auto body = share(child_window({10, 10, 110, 110}, box(colors::blue)));
   test_view tv{extent{200, 200}};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(body, hold(under));
   tv.draw();
   tv.view_.click(left({50, 50}, true));
   tv.view_.click(left({50, 50}, false));
   CHECK(clicks == 0);
}

TEST_CASE("child window: a click on its body raises it", "[child_window]")
{
   auto back = share(child_window({10, 10, 110, 110}, box(colors::blue)));
   auto front = share(child_window({60, 60, 160, 160}, box(colors::red)));
   test_view tv{extent{200, 200}};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(front, back);
   tv.draw();
   tv.view_.click(left({20, 20}, true));
   tv.view_.click(left({20, 20}, false));
   tv.view_.poll();
   CHECK(tv.view_.layers().back() == back);
}

TEST_CASE("floating: keeps its top left", "[child_window]")
{
   // Too small for its subject's 50 by 50 minimum: it grows from its top
   // left, not around its center.
   auto c = share(counter{});
   auto fl = share(floating({100, 100, 100, 100}, hold(c)));
   test_view tv{extent{300, 300}};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(fl);
   tv.draw();
   CHECK(fl->bounds() == rect{100, 100, 150, 150});
}

TEST_CASE("resizable: the edges and corners resize the window", "[child_window]")
{
   auto c = share(counter{});                    // at least 50 by 50
   auto w = share(child_window({50, 50, 150, 150}, resizable(hold(c))));
   test_view tv{extent{300, 300}};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(w);
   tv.draw();

   auto drag = [&](point from, point to)
   {
      tv.view_.click(left(from, true));
      tv.view_.drag(left(to, true));
      tv.view_.click(left(to, false));
   };

   drag({148, 100}, {178, 100});                 // the right edge
   CHECK(w->bounds() == rect{50, 50, 180, 150});
   drag({52, 100}, {22, 100});                   // the left edge
   CHECK(w->bounds() == rect{20, 50, 180, 150});
   drag({178, 148}, {198, 178});                 // the bottom right corner
   CHECK(w->bounds() == rect{20, 50, 200, 180});
   drag({100, 52}, {100, 170});                  // the top, to the minimum
   CHECK(w->bounds() == rect{20, 130, 200, 180});
   CHECK(c->downs == 0);                         // the resizes were not its

   // Inside, the mouse goes to the subject.
   tv.view_.click(left({100, 155}, true));
   tv.view_.click(left({100, 155}, false));
   CHECK(c->downs == 1);

#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   context ctx{tv.view_, tv.cnv, w.get(), rect{0, 0, 300, 300}};
   CHECK(w->cursor(ctx, {199, 150}, cursor_tracking::hovering));
   CHECK(headless::cursor() == cursor_type::h_resize);
   CHECK(w->cursor(ctx, {100, 179}, cursor_tracking::hovering));
   CHECK(headless::cursor() == cursor_type::v_resize);
#endif
}

TEST_CASE("child window styles are resizable and still movable", "[child_window]")
{
   auto w = share(basic_child_window("Title", {50, 50, 200, 200}, element{}));
   test_view tv{extent{400, 400}};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(w);
   tv.draw();
   auto b = w->bounds();

   auto drag = [&](point from, point to)
   {
      tv.view_.click(left(from, true));
      tv.view_.drag(left(to, true));
      tv.view_.click(left(to, false));
   };

   // The right edge resizes.
   drag({b.right - 2, b.top + 60}, {b.right + 38, b.top + 60});
   CHECK(w->bounds().width() == Approx(b.width() + 40));
   CHECK(w->bounds().left == b.left);

   // The title moves it.
   b = w->bounds();
   drag({b.left + 40, b.top + 12}, {b.left + 60, b.top + 32});
   CHECK(w->bounds() == b.move(20, 20));
}

TEST_CASE("child window: the close button clears the resize grip", "[child_window]")
{
   // All of the button closes the window; the grip is past its right.
   for (float dx = -28; dx < 0; dx += 1)
   {
      auto w = share(
         closable_child_window("Title", {50, 50, 200, 200}, element{}));
      test_view tv{extent{400, 400}};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
      headless::open(tv.view_);
#endif
      tv.view_.content(w);
      tv.draw();
      auto b = w->bounds();
      point p{b.right + dx, b.top + 12};
      tv.view_.click(left(p, true));
      tv.view_.click(left(p, false));
      INFO("dx " << dx);
      CHECK(w->is_closing() == (dx < -6));       // closing: the button
      if (dx < -6)
      {
         wait_closed(tv.view_, w);
         CHECK(!tv.view_.is_open(w));
      }
   }
}

TEST_CASE("child window: closing fades it out", "[child_window]")
{
   auto w = share(
      closable_child_window("Title", {50, 50, 200, 200}, element{}));
   test_view tv{extent{400, 400}};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   auto& v = tv.view_;
   v.content(w);
   tv.draw();
   auto b = w->bounds();
   v.click(left({b.right - 16, b.top + 12}, true));
   v.click(left({b.right - 16, b.top + 12}, false));

   // Fading: still open, drawn, and no longer taking the mouse.
   CHECK(w->is_closing());
   std::this_thread::sleep_for(std::chrono::milliseconds{40});
   v.poll();
   tv.draw();
   CHECK(v.is_open(w));
   CHECK(w->is_closing());
   context ctx{v, tv.cnv, w.get(), rect{0, 0, 400, 400}};
   CHECK(!w->hit_test(ctx, {b.left + 40, b.top + 60}, true, true));

   // Gone; opened again, it is opaque.
   wait_closed(v, w);
   CHECK(!v.is_open(w));
   v.add(w);
   v.poll();
   CHECK(v.is_open(w));
   CHECK(!w->is_closing());
}
