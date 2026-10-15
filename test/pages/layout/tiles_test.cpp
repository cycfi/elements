/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/layout/tiles.adoc. The
   stretch claims are in stretch_test.cpp.
=============================================================================*/
#include "test_support.hpp"
#include <cstdint>
#include <type_traits>

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

   // A probe with its own stretch, unguarded, as a custom element may give.
   struct stretchy : probe
   {
      stretchy(view_limits l, view_stretch s) : probe(l), st(s) {}

      view_stretch stretch() const override { return st; }

      view_stretch st;
   };

   template <typename Tile>
   void lay_out(test_view& tv, Tile& tile, rect bounds)
   {
      context ctx{tv.view_, tv.cnv, &tile, bounds};
      tile.layout(ctx);
      tile.draw(ctx);
   }

   template <typename E>
   view_limits limits_of(test_view& tv, E& e)
   {
      basic_context ctx{tv.view_, tv.cnv};
      return e.limits(ctx);
   }

   // The pixel at (x, y); pixels() is premultiplied B, G, R, A.
   struct rgb_
   {
      int r, g, b;
   };

   rgb_ pixel_at(cycfi::artist::image const& img, int x, int y)
   {
      auto p = reinterpret_cast<std::uint8_t const*>(img.pixels())
         + 4 * (y * int(img.bitmap_size().x) + x);
      return {p[2], p[1], p[0]};
   }

   view_limits const growable = {{10, 10}, {full_extent, full_extent}};
}

TEST_CASE("tiles page: the factories and the composites", "[tiles_page]")
{
   probe a, b, c;

   auto h = htile(link(a), link(b), link(c));
   CHECK(std::is_same_v<decltype(h), array_composite<3, htile_element>>);
   CHECK(std::is_same_v<decltype(h)::container_type, std::array<element_ptr, 3>>);
   auto hs = htile_spaced(10, link(a), link(b), link(c));
   CHECK(std::is_same_v<decltype(hs), array_composite<3, htile_element>>);
   CHECK(std::is_same_v<htile_composite, vector_composite<htile_element>>);
   CHECK(std::is_base_of_v<composite_base, htile_element>);

   auto v = vtile(link(a), link(b), link(c));
   CHECK(std::is_same_v<decltype(v), array_composite<3, vtile_element>>);
   auto vs = vtile_spaced(10, link(a), link(b), link(c));
   CHECK(std::is_same_v<decltype(vs), array_composite<3, vtile_element>>);
   CHECK(std::is_same_v<vtile_composite, vector_composite<vtile_element>>);
   CHECK(std::is_base_of_v<composite_base, vtile_element>);

   CHECK(h.size() == 3);
   CHECK(&h.at(0) == h[0].get());
}

TEST_CASE("tiles page: the elements are laid out immediately next to each other", "[tiles_page]")
{
   test_view tv{extent{400, 400}};
   probe a{growable}, b{growable}, c{growable};

   // Left to right, a single row.
   auto row = htile(link(a), link(b), link(c));
   lay_out(tv, row, rect{20, 30, 170, 80});
   CHECK(a.drawn == rect{20, 30, 70, 80});
   CHECK(b.drawn == rect{70, 30, 120, 80});
   CHECK(c.drawn == rect{120, 30, 170, 80});

   // Top to bottom, a single column.
   auto col = vtile(link(a), link(b), link(c));
   lay_out(tv, col, rect{20, 30, 70, 180});
   CHECK(a.drawn == rect{20, 30, 70, 80});
   CHECK(b.drawn == rect{20, 80, 70, 130});
   CHECK(c.drawn == rect{20, 130, 70, 180});
}

TEST_CASE("tiles page: the extra space goes to the growable elements", "[tiles_page]")
{
   test_view tv{extent{400, 400}};

   SECTION("htile")
   {
      // a and c take 20 at least, d is fixed at 30: 100 extra in 170.
      probe a{{{20, 10}, {full_extent, full_extent}}};
      probe d{{{30, 10}, {30, full_extent}}};
      probe c{{{20, 10}, {full_extent, full_extent}}};
      auto row = htile(link(a), link(d), link(c));
      lay_out(tv, row, rect{0, 0, 170, 50});
      CHECK(a.drawn.width() == Approx(70));
      CHECK(d.drawn.width() == Approx(30));
      CHECK(c.drawn.width() == Approx(70));
   }

   SECTION("vtile")
   {
      probe a{{{10, 20}, {full_extent, full_extent}}};
      probe b{{{10, 30}, {full_extent, 30}}};
      probe c{{{10, 20}, {full_extent, full_extent}}};
      auto col = vtile(link(a), link(b), link(c));
      lay_out(tv, col, rect{0, 0, 50, 170});
      CHECK(a.drawn.height() == Approx(70));
      CHECK(b.drawn.height() == Approx(30));
      CHECK(c.drawn.height() == Approx(70));
   }

   SECTION("no extra space: each element gets its minimum")
   {
      probe a{{{20, 10}, {full_extent, full_extent}}};
      probe b{{{40, 10}, {full_extent, full_extent}}};
      auto row = htile(link(a), link(b));
      lay_out(tv, row, rect{0, 0, 60, 50});
      CHECK(a.drawn.width() == Approx(20));
      CHECK(b.drawn.width() == Approx(40));
   }
}

TEST_CASE("tiles page: the limits across the tile", "[tiles_page]")
{
   test_view tv{extent{400, 400}};

   SECTION("htile: the maximum of the minimums, the minimum of the maximums")
   {
      probe a{{{10, 20}, {100, 80}}}, b{{{10, 30}, {100, 60}}};
      auto row = htile(link(a), link(b));
      auto l = limits_of(tv, row);
      CHECK(l.min.y == 30);
      CHECK(l.max.y == 60);
   }

   SECTION("vtile: the maximum of the minimums, the minimum of the maximums")
   {
      probe a{{{20, 10}, {80, 100}}}, b{{{30, 10}, {60, 100}}};
      auto col = vtile(link(a), link(b));
      auto l = limits_of(tv, col);
      CHECK(l.min.x == 30);
      CHECK(l.max.x == 60);
   }

   SECTION("the maximum is not less than the minimum")
   {
      probe a{{{10, 50}, {100, 200}}}, b{{{10, 0}, {100, 20}}};
      auto row = htile(link(a), link(b));
      auto l = limits_of(tv, row);
      CHECK(l.max.y >= l.min.y);
      CHECK(l.min.y == 50);   // The minimum wins.
      CHECK(l.max.y == 50);

      probe c{{{50, 10}, {200, 100}}}, d{{{0, 10}, {20, 100}}};
      auto col = vtile(link(c), link(d));
      auto m = limits_of(tv, col);
      CHECK(m.max.x >= m.min.x);
      CHECK(m.min.x == 50);
      CHECK(m.max.x == 50);
   }
}

TEST_CASE("tiles page: the spaced tiles put space between the elements", "[tiles_page]")
{
   test_view tv{extent{400, 400}};
   probe a{growable}, b{growable}, c{growable};

   SECTION("htile_spaced")
   {
      auto row = htile_spaced(10, link(a), link(b), link(c));
      lay_out(tv, row, rect{0, 0, 170, 50});
      CHECK(a.drawn == rect{0, 0, 50, 50});
      CHECK(b.drawn == rect{60, 0, 110, 50});
      CHECK(c.drawn == rect{120, 0, 170, 50});

      // The same as htile with margin_left on all but the first.
      probe x{growable}, y{growable}, z{growable};
      auto same = htile(link(x), margin_left(10, link(y)), margin_left(10, link(z)));
      lay_out(tv, same, rect{0, 0, 170, 50});
      CHECK(x.drawn == a.drawn);
      CHECK(y.drawn == b.drawn);
      CHECK(z.drawn == c.drawn);
      CHECK(limits_of(tv, row).min == limits_of(tv, same).min);
   }

   SECTION("vtile_spaced")
   {
      auto col = vtile_spaced(10, link(a), link(b), link(c));
      lay_out(tv, col, rect{0, 0, 50, 170});
      CHECK(a.drawn == rect{0, 0, 50, 50});
      CHECK(b.drawn == rect{0, 60, 50, 110});
      CHECK(c.drawn == rect{0, 120, 50, 170});

      probe x{growable}, y{growable}, z{growable};
      auto same = vtile(link(x), margin_top(10, link(y)), margin_top(10, link(z)));
      lay_out(tv, same, rect{0, 0, 50, 170});
      CHECK(x.drawn == a.drawn);
      CHECK(y.drawn == b.drawn);
      CHECK(z.drawn == c.drawn);
      CHECK(limits_of(tv, col).min == limits_of(tv, same).min);
   }
}

TEST_CASE("tiles page: a composite tile is used like a std::vector and shared like any element", "[tiles_page]")
{
   test_view tv{extent{400, 400}};
   probe a{growable}, b{growable};

   htile_composite c;
   c.push_back(share(link(a)));
   c.push_back(share(link(b)));
   CHECK(c.size() == 2);
   lay_out(tv, c, rect{0, 0, 100, 50});
   CHECK(a.drawn == rect{0, 0, 50, 50});
   CHECK(b.drawn == rect{50, 0, 100, 50});

   vtile_composite vc;
   vc.push_back(share(link(a)));
   vc.push_back(share(link(b)));
   lay_out(tv, vc, rect{0, 0, 50, 100});
   CHECK(a.drawn == rect{0, 0, 50, 50});
   CHECK(b.drawn == rect{0, 50, 50, 100});

   // Shared into another tile.
   auto hc = std::make_shared<htile_composite>();
   hc->push_back(share(link(a)));
   hc->push_back(share(link(b)));
   probe top{growable};
   auto col = vtile(link(top), hold(hc));
   lay_out(tv, col, rect{0, 0, 100, 100});
   CHECK(top.drawn == rect{0, 0, 100, 50});
   CHECK(a.drawn == rect{0, 50, 50, 100});
   CHECK(b.drawn == rect{50, 50, 100, 100});
}

TEST_CASE("tiles page: elements that do not fit keep their minimum and overflow", "[tiles_page]")
{
   // Along the tile, past its end.
   test_view tv{extent{400, 400}};
   probe a{{{50, 10}, {100, 100}}}, b{{{50, 10}, {100, 100}}};
   auto row = htile(link(a), link(b));
   lay_out(tv, row, rect{0, 0, 60, 50});
   CHECK(a.drawn == rect{0, 0, 50, 50});
   CHECK(b.drawn == rect{50, 0, 100, 50});

   auto col = vtile(link(a), link(b));
   a.lim = b.lim = {{10, 50}, {100, 100}};
   lay_out(tv, col, rect{0, 0, 50, 60});
   CHECK(a.drawn == rect{0, 0, 50, 50});
   CHECK(b.drawn == rect{0, 50, 50, 100});
}

TEST_CASE("tiles page: a negative stretch is taken as 0", "[tiles_page]")
{
   test_view tv{extent{400, 400}};
   view_limits const l = {{10, 10}, {200, 200}};

   for (float s : {-1.0f, -0.5f})
   {
      // a takes no extra space; b takes all 180 of it.
      stretchy a{l, {s, s}};
      probe b{l};
      auto row = htile(link(a), link(b));
      lay_out(tv, row, rect{0, 0, 200, 50});
      CHECK(a.drawn == rect{0, 0, 10, 50});
      CHECK(b.drawn == rect{10, 0, 200, 50});

      auto col = vtile(link(a), link(b));
      lay_out(tv, col, rect{0, 0, 50, 200});
      CHECK(a.drawn == rect{0, 0, 50, 10});
      CHECK(b.drawn == rect{0, 10, 50, 200});
   }
}

TEST_CASE("tiles page: the example", "[tiles_page]")
{
   test_view tv{extent{300, 230}};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif

   htile_composite swatches;
   for (auto fill : {colors::red, colors::green, colors::blue})
      swatches.push_back(share(box(fill)));

   auto column = vtile(
      vsize(30, box(colors::slate_gray)),
      htile_spaced(10,
         box(colors::gold),
         hstretch(2, box(colors::steel_blue))
      ),
      swatches
   );

   tv.view_.content(column);
   tv.draw();

   // The strip is 0 to 30; the rows share the remaining 200: 30 to 130
   // and 130 to 230. In the middle row, 290 extra width is split 1 : 2
   // after the 10 of space: gold 0 to 96.67, steel blue 106.67 to 300.
   // The swatches are 100 wide each.
#if !defined(ARTIST_RECORDING)
   auto strip = pixel_at(tv.img, 150, 15);
   CHECK(strip.r > 100);
   CHECK(strip.r < 125);
   CHECK(strip.b > 130);
   CHECK(strip.b < 155);
   auto gold = pixel_at(tv.img, 50, 80);
   CHECK(gold.r > 200);
   CHECK(gold.b < 50);
   auto steel = pixel_at(tv.img, 250, 80);
   CHECK(steel.b > 150);
   CHECK(steel.r < 100);
   auto red = pixel_at(tv.img, 50, 180);
   CHECK(red.r > 200);
   CHECK(red.g < 50);
   auto green = pixel_at(tv.img, 150, 180);
   CHECK(green.g > 100);
   CHECK(green.r < 50);
   auto blue = pixel_at(tv.img, 250, 180);
   CHECK(blue.b > 200);
   CHECK(blue.r < 50);
#endif
}
