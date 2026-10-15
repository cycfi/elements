/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/elements/splits.adoc and
   draws its figure. The figure lands in the build's results directory;
   copy it to docs/modules/ROOT/images/elements/ to update the page.
=============================================================================*/
#include "test_support.hpp"
#include <string>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   // Records its bounds; at least `min` on each axis.
   struct slot : element
   {
      slot(float min_ = 20) : min{min_} {}
      view_limits limits(basic_context const&) const override
      {
         return {{min, min}, {full_extent, full_extent}};
      }
      void layout(context const& ctx) override { bounds = ctx.bounds; }
      void draw(context const& ctx) override { bounds = ctx.bounds; }
      float min;
      rect bounds;
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

   void open(test_view& tv)
   {
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
      headless::open(tv.view_);
#endif
   }
}

TEST_CASE("splits page: laid out as a tile, then by shares", "[splits_page]")
{
   auto a = share(slot{});
   auto b = share(slot{});
   auto s = share(hsplit(hold(a), hold(b)));
   test_view tv{extent{200, 100}};
   open(tv);
   tv.view_.content(hold(s));
   tv.draw();
   CHECK(a->bounds.right == 100);                // as a tile
   CHECK(s->shares().empty());

   drag(tv.view_, {100, 50}, {150, 50});
   REQUIRE(s->shares().size() == 2);
   CHECK(s->shares()[0] == Approx(0.75));

   // Resized, each keeps its share.
   context ctx{tv.view_, tv.cnv, s.get(), rect{0, 0, 400, 100}};
   s->layout(ctx);
   CHECK(a->bounds.right == Approx(300));
}

TEST_CASE("splits page: dragging stops at the limits", "[splits_page]")
{
   auto a = share(slot{});
   auto b = share(slot{50});
   test_view tv{extent{200, 100}};
   open(tv);
   auto s = share(hsplit(hold(a), hold(b)));
   tv.view_.content(hold(s));
   tv.draw();
   auto at = a->bounds.right;
   drag(tv.view_, {at, 50}, {200, 50});
   tv.draw();
   CHECK(b->bounds.width() == Approx(50));
}

TEST_CASE("splits page: nested splits drag on their own", "[splits_page]")
{
   auto a = share(slot{});
   auto b = share(slot{});
   auto c = share(slot{});
   test_view tv{extent{200, 200}};
   open(tv);
   tv.view_.content(hsplit(hold(a), vsplit(hold(b), hold(c))));
   tv.draw();
   REQUIRE(b->bounds.bottom == Approx(100));

   drag(tv.view_, {150, 100}, {150, 140});       // the inner boundary
   tv.draw();
   CHECK(b->bounds.bottom == Approx(140));
   CHECK(a->bounds.right == Approx(100));        // the outer did not move
}

TEST_CASE("splits page: setting shares, and a change of children", "[splits_page]")
{
   auto s = share(hsplit_composite{});
   s->push_back(share(slot{}));
   s->push_back(share(slot{}));
   s->shares({0.25f, 0.75f});
   test_view tv{extent{200, 100}};
   open(tv);
   tv.view_.content(hold(s));
   tv.draw();
   CHECK(s->bounds_of(
      context{tv.view_, tv.cnv, s.get(), rect{0, 0, 200, 100}}, 0).right
      == Approx(50));

   // One more child: laid out as a tile again.
   s->push_back(share(slot{}));
   tv.view_.layout();
   tv.draw();
   CHECK(s->shares().empty());
}

TEST_CASE("splits page: example", "[splits_page]")
{
   // The example's layout, with placeholder panes.
   auto make_pane = [](char const* title, element_ptr content)
   {
      return layer(
         vtile(
            layer(
               margin({10, 6, 10, 6}, align_left(label(title))),
               box(rgba(60, 70, 90, 255))
            ),
            margin({10, 10, 10, 10}, hold(content))
         ),
         box(rgba(44, 46, 52, 255))
      );
   };
   auto text = [](char const* t) { return share(align_left_top(label(t))); };

   test_view tv{extent{480, 300}, 2};
   open(tv);
   tv.view_.content(
      hsplit(
         hmin_size(120, make_pane("Files", text("main.cpp"))),
         vsplit(
            vmin_size(120, make_pane("Editor", text("auto layout = ..."))),
            vmin_size(80, make_pane("Output", text("Build succeeded.")))
         )
      ),
      box(rgba(35, 35, 37, 255))
   );
   tv.draw();
   tv.img.save_png(std::string{RESULTS_PATH} + "splits_example.png");
}
