/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/layout/stretch.adoc.
=============================================================================*/
#include "test_support.hpp"

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   // An element of the given limits that records the bounds it is drawn
   // with.
   struct probe : element
   {
      probe(view_limits l = full_limits) : lim(l) {}

      view_limits limits(basic_context const&) const override { return lim; }
      void draw(context const& ctx) override { drawn = ctx.bounds; }

      view_limits lim;
      rect drawn = {};
   };

   template <typename Tile>
   void lay_out(test_view& tv, Tile& tile, rect bounds)
   {
      context ctx{tv.view_, tv.cnv, &tile, bounds};
      tile.layout(ctx);
      tile.draw(ctx);
   }
}

TEST_CASE("stretch page: hstretch and vstretch set one axis only", "[stretch_page]")
{
   CHECK(hstretch(2, probe{}).stretch().x == 2);
   CHECK(hstretch(2, probe{}).stretch().y == 1);
   CHECK(vstretch(2, probe{}).stretch().x == 1);
   CHECK(vstretch(2, probe{}).stretch().y == 2);
   CHECK(no_hstretch(probe{}).stretch().x == 0);
   CHECK(no_hstretch(probe{}).stretch().y == 1);
   CHECK(no_vstretch(probe{}).stretch().x == 1);
   CHECK(no_vstretch(probe{}).stretch().y == 0);
}

TEST_CASE("stretch page: a stretch of 2 takes twice the extra space", "[stretch_page]")
{
   // Three elements, 10 wide at least, in a tile 160 wide: 130 extra,
   // shared 1 : 1 : 2.
   test_view tv{extent{400, 400}};
   view_limits const l = {{10, 10}, {full_extent, full_extent}};
   probe a{l}, b{l}, c{l};
   auto row = htile(link(a), link(b), hstretch(2, link(c)));
   lay_out(tv, row, rect{0, 0, 160, 50});
   CHECK(a.drawn.width() == Approx(42.5));
   CHECK(b.drawn.width() == Approx(42.5));
   CHECK(c.drawn.width() == Approx(75));

   // The same with 0.5, 1.0 and 1.5, in a vtile.
   probe d{l}, e{l}, f{l};
   auto col = vtile(vstretch(0.5, link(d)), link(e), vstretch(1.5, link(f)));
   lay_out(tv, col, rect{0, 0, 50, 120});
   CHECK(d.drawn.height() == Approx(25));
   CHECK(e.drawn.height() == Approx(40));
   CHECK(f.drawn.height() == Approx(55));
}

TEST_CASE("stretch page: stretch does not affect a fixed size", "[stretch_page]")
{
   test_view tv{extent{400, 400}};
   probe a{{{30, 10}, {30, full_extent}}}, b;
   auto row = htile(hstretch(4, link(a)), link(b));
   lay_out(tv, row, rect{0, 0, 200, 50});
   CHECK(a.drawn.width() == Approx(30));
   CHECK(b.drawn.width() == Approx(170));

   probe c{{{10, 30}, {full_extent, 30}}}, d;
   auto col = vtile(vstretch(4, link(c)), link(d));
   lay_out(tv, col, rect{0, 0, 50, 200});
   CHECK(c.drawn.height() == Approx(30));
   CHECK(d.drawn.height() == Approx(170));
}

TEST_CASE("stretch page: no_hstretch and no_vstretch keep the minimum", "[stretch_page]")
{
   test_view tv{extent{400, 400}};
   view_limits const l = {{40, 40}, {full_extent, full_extent}};

   probe a{l}, b{l};
   auto row = htile(no_hstretch(link(a)), link(b));
   lay_out(tv, row, rect{0, 0, 200, 50});
   CHECK(a.drawn.width() == Approx(40));
   CHECK(b.drawn.width() == Approx(160));

   probe c{l}, d{l};
   auto col = vtile(no_vstretch(link(c)), link(d));
   lay_out(tv, col, rect{0, 0, 50, 200});
   CHECK(c.drawn.height() == Approx(40));
   CHECK(d.drawn.height() == Approx(160));
}

TEST_CASE("stretch page: a negative stretch is taken as 0", "[stretch_page]")
{
   CHECK(hstretch(-0.5, probe{}).stretch().x == 0);
   CHECK(vstretch(-0.5, probe{}).stretch().y == 0);

   auto h = hstretch(1, probe{});
   h.hstretch(-0.5);
   CHECK(h.hstretch() == 0);
   auto v = vstretch(1, probe{});
   v.vstretch(-0.5);
   CHECK(v.vstretch() == 0);

   // Stretches of 1 and -0.5 in a tile 100 long: both keep their minimum
   // of 10, and the first takes all of the extra space.
   test_view tv{extent{400, 400}};
   view_limits const l = {{10, 10}, {full_extent, full_extent}};

   probe a{l}, b{l};
   auto row = htile(link(a), hstretch(-0.5, link(b)));
   lay_out(tv, row, rect{0, 0, 100, 50});
   CHECK(a.drawn.width() == Approx(90));
   CHECK(b.drawn.width() == Approx(10));

   probe c{l}, d{l};
   auto col = vtile(link(c), vstretch(-0.5, link(d)));
   lay_out(tv, col, rect{0, 0, 50, 100});
   CHECK(c.drawn.height() == Approx(90));
   CHECK(d.drawn.height() == Approx(10));
}
