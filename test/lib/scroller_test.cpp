/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   The scroller: a click on the horizontal track pages horizontally; the
   keys report through on_scroll and need a bar; scrolling into view keeps
   clear of the bar on its own edge; the pointer redraws only around the
   bars; and content that fits does not scroll.
=============================================================================*/
#include "test_support.hpp"

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   // Content of a fixed size.
   struct content : element
   {
      content(point s) : size{s} {}
      view_limits limits(basic_context const&) const override { return {size, size}; }
      point size;
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

   // A 100 by 100 view holding a scroller over content of `size`.
   struct scroller_view : test_view
   {
      scroller_view(point size)
       : test_view{extent{100, 100}, 1}
       , sc{share(scroller(content{size}))}
      {
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
         headless::open(view_);
#endif
         sc->on_scroll = [this](point p) { ++calls; last = p; };
         view_.content(hold(sc));
         view_.layout();
         draw();
      }

      context ctx() { return {view_, cnv, sc.get(), rect{0, 0, 100, 100}}; }

      bool key(key_code k)
      {
         auto c = ctx();
         return sc->key(c, key_info{k, key_action::press, 0});
      }

      std::shared_ptr<proxy<content, scroller_base>> sc;
      int calls = 0;
      point last;
   };
}

TEST_CASE("scroller: a click on the horizontal track pages horizontally", "[scroller]")
{
   // 400 wide in 100: a horizontal bar, its thumb a quarter of the track.
   scroller_view sv{{400, 80}};
   auto c = sv.ctx();
   sv.sc->click(c, left({90, 95}, true));        // right of the thumb
   sv.sc->click(c, left({90, 95}, false));
   CHECK(sv.sc->halign() == Approx(0.25));       // one page, not to the end
   CHECK(sv.sc->valign() == 0);
   CHECK(sv.calls == 1);
}

TEST_CASE("scroller: the keys report through on_scroll", "[scroller]")
{
   scroller_view sv{{80, 400}};                  // a vertical bar
   CHECK(sv.key(key_code::end));
   CHECK(sv.sc->valign() == 1);
   CHECK(sv.calls == 1);
   CHECK(sv.last.y == 1);
   CHECK(sv.key(key_code::page_up));
   CHECK(sv.sc->valign() == Approx(0.75));
   CHECK(sv.calls == 2);
}

TEST_CASE("scroller: without a vertical bar, the keys are left alone", "[scroller]")
{
   scroller_view sv{{400, 80}};                  // a horizontal bar only
   CHECK(!sv.key(key_code::page_down));
   CHECK(!sv.key(key_code::end));
   CHECK(sv.sc->valign() == 0);
   CHECK(sv.calls == 0);
}

TEST_CASE("scroller: scrolling into view keeps clear of the bar's own edge", "[scroller]")
{
   // A vertical bar only, at the right: the bottom edge is clear, so a
   // rectangle near the bottom is in view as it is.
   scroller_view sv{{80, 400}};
   auto c = sv.ctx();
   CHECK(!sv.sc->scroll_into_view(c, rect{10, 92, 20, 98}));
   CHECK(sv.sc->valign() == 0);

   // Under the bar, at the right, it is not: but the content is no wider
   // than the port, so there is nowhere to scroll to.
   CHECK(!sv.sc->scroll_into_view(c, rect{92, 10, 98, 20}));
}

TEST_CASE("scroller: the pointer redraws only around the bars", "[scroller]")
{
   scroller_view sv{{80, 400}};                  // a vertical bar, at x 90 to 100
   auto c = sv.ctx();
   headless::take_invalidated(sv.view_);

   // Away from the bar: nothing to redraw.
   sv.sc->cursor(c, {30, 30}, cursor_tracking::hovering);
   sv.sc->cursor(c, {40, 50}, cursor_tracking::hovering);
   sv.view_.poll();
   CHECK(headless::take_invalidated(sv.view_).empty());

   // Over the bar, and on leaving it: the bar is redrawn.
   CHECK(sv.sc->cursor(c, {95, 30}, cursor_tracking::hovering));
   sv.view_.poll();
   CHECK(!headless::take_invalidated(sv.view_).empty());
   sv.sc->cursor(c, {40, 30}, cursor_tracking::hovering);
   sv.view_.poll();
   CHECK(!headless::take_invalidated(sv.view_).empty());

   // Then away from it again: nothing.
   sv.sc->cursor(c, {30, 40}, cursor_tracking::hovering);
   sv.view_.poll();
   CHECK(headless::take_invalidated(sv.view_).empty());
}

TEST_CASE("scroller: content that fits does not scroll", "[scroller]")
{
   scroller_view sv{{80, 80}};
   auto c = sv.ctx();
   sv.sc->scroll(c, {-5, -5}, {50, 50});
   CHECK(sv.sc->halign() == 0);
   CHECK(sv.sc->valign() == 0);
   CHECK(sv.calls == 0);

   // Exactly the size of the port: the same, and no division by zero.
   scroller_view exact{{100, 100}};
   auto e = exact.ctx();
   exact.sc->scroll(e, {-5, -5}, {50, 50});
   CHECK(exact.sc->halign() == 0);
   CHECK(exact.sc->valign() == 0);
   CHECK(exact.calls == 0);
}
