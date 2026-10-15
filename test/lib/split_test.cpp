/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Splits: laid out as a tile until a boundary is dragged; a drag trades
   space between the two children beside the boundary, within their
   limits; the shares hold as the split is resized; the boundary sets the
   resize cursor; elsewhere, the mouse goes to the children.
=============================================================================*/
#include "test_support.hpp"
#include <string>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   // Records its bounds and its clicks; at least `min` wide and high.
   struct slot : element
   {
      slot(float min_ = 20) : min{min_} {}
      view_limits limits(basic_context const&) const override
      {
         return {{min, min}, {full_extent, full_extent}};
      }
      void layout(context const& ctx) override { bounds = ctx.bounds; }
      void draw(context const& ctx) override { bounds = ctx.bounds; }
      element* hit_test(context const& ctx, point p, bool, bool) override
      {
         return ctx.bounds.includes(p)? this : nullptr;
      }
      bool wants_control() const override { return true; }
      bool click(context const&, mouse_button btn) override
      {
         if (btn.down)
            ++clicks;
         return true;
      }
      float min;
      rect bounds;
      int clicks = 0;
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

   void drag(view& v, point from, point to)
   {
      v.click(left(from, true));
      v.drag(left(to, true));
      v.click(left(to, false));
   }
}

TEST_CASE("split: laid out as a tile", "[split]")
{
   auto a = share(slot{});
   auto b = share(slot{});
   auto s = share(hsplit(hold(a), hold(b)));
   auto t = share(htile(slot{}, slot{}));
   test_view tv{extent{200, 100}};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(hold(s));
   tv.draw();
   CHECK(a->bounds == rect{0, 0, 100, 100});
   CHECK(b->bounds == rect{100, 0, 200, 100});
   CHECK(s->shares().empty());

   basic_context bctx{tv.view_, tv.cnv};
   CHECK(s->limits(bctx).min == t->limits(bctx).min);
}

TEST_CASE("split: a drag trades space within the limits", "[split]")
{
   auto a = share(slot{});
   auto b = share(slot{60});
   auto s = share(hsplit(hold(a), hold(b)));
   test_view tv{extent{200, 100}};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(hold(s));
   tv.draw();

   // As a tile: a gets 20 and b 60, their minimums, and each half the
   // remaining 120, so the boundary is at 80.
   CHECK(a->bounds.right == Approx(80));

   drag(tv.view_, {80, 50}, {110, 50});
   tv.draw();
   CHECK(a->bounds.right == Approx(110));
   CHECK(b->bounds.left == Approx(110));
   REQUIRE(s->shares().size() == 2);
   CHECK(s->shares()[0] == Approx(0.55));

   // b stops at its minimum, 60; a at its, 20.
   drag(tv.view_, {110, 50}, {190, 50});
   tv.draw();
   CHECK(b->bounds.width() == Approx(60));
   drag(tv.view_, {140, 50}, {0, 50});
   tv.draw();
   CHECK(a->bounds.width() == Approx(20));
   CHECK(a->clicks == 0);                        // the drags were the split's
   CHECK(b->clicks == 0);
}

TEST_CASE("split: the shares hold as it is resized", "[split]")
{
   auto a = share(slot{});
   auto b = share(slot{});
   auto c = share(slot{});
   auto s = share(vsplit(hold(a), hold(b), hold(c)));
   s->shares({0.5f, 0.25f, 0.25f});
   test_view tv{extent{100, 200}};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(hold(s));
   tv.draw();
   CHECK(a->bounds.height() == Approx(100));
   CHECK(b->bounds.height() == Approx(50));

   // Half as high: each keeps its share, and the minimum holds.
   context ctx{tv.view_, tv.cnv, s.get(), rect{0, 0, 100, 60}};
   s->layout(ctx);
   CHECK(b->bounds.height() == Approx(20));      // its minimum
   CHECK(c->bounds.height() == Approx(20));
   CHECK(a->bounds.height() == Approx(20));
}

TEST_CASE("split: the boundary sets the resize cursor", "[split]")
{
   auto s = share(hsplit(slot{}, slot{}));
   test_view tv{extent{200, 100}};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(hold(s));
   tv.draw();
   context ctx{tv.view_, tv.cnv, s.get(), rect{0, 0, 200, 100}};
   CHECK(s->cursor(ctx, {101, 50}, cursor_tracking::hovering));
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   CHECK(headless::cursor() == cursor_type::h_resize);
#endif
   CHECK(!s->cursor(ctx, {40, 50}, cursor_tracking::hovering));
}

TEST_CASE("split: elsewhere, the mouse goes to the children", "[split]")
{
   auto a = share(slot{});
   auto b = share(slot{});
   test_view tv{extent{200, 100}};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(hsplit(hold(a), hold(b)));
   tv.draw();
   tv.view_.click(left({40, 50}, true));
   tv.view_.click(left({40, 50}, false));
   tv.view_.click(left({160, 50}, true));
   tv.view_.click(left({160, 50}, false));
   CHECK(a->clicks == 1);
   CHECK(b->clicks == 1);
}

#if !defined(ARTIST_RECORDING)
TEST_CASE("split: the example's layout", "[split]")
{
   // The layout of examples/split, before and after a drag.
   auto make_pane = [](char const* title, float min_w, float min_h)
   {
      return hmin_size(min_w, vmin_size(min_h,
         layer(
            vtile(
               layer(
                  margin({10, 6, 10, 6}, align_left(label(title))),
                  box(rgba(60, 70, 90, 255))
               ),
               margin({10, 10, 10, 10}, align_left_top(label("...")))
            ),
            box(rgba(44, 46, 52, 255))
         )));
   };
   auto s = share(hsplit(
      make_pane("Files", 120, 0),
      vsplit(
         make_pane("Editor", 0, 120),
         make_pane("Output", 0, 80)
      )));
   test_view tv{extent{480, 300}, 2};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(hold(s), box(rgba(35, 35, 37, 255)));
   tv.draw();
   tv.img.save_png(std::string{RESULTS_PATH} + "split_before.png");

   // Find the boundary between Files and the rest, and drag it left.
   context ctx{tv.view_, tv.cnv, s.get(), rect{0, 0, 480, 300}};
   float at = 0;
   for (float x = 0; x != 480; ++x)
   {
      if (s->cursor(ctx, {x, 150}, cursor_tracking::hovering))
      {
         at = x;
         break;
      }
   }
   REQUIRE(at > 0);
   drag(tv.view_, {at, 150}, {at - 100, 150});
   tv.view_.layout();
   tv.draw();
   tv.img.save_png(std::string{RESULTS_PATH} + "split_after.png");
}
#endif
