/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/layout/grids.adoc. The span
   claims are in span_test.cpp.
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

   template <typename Grid>
   void lay_out(test_view& tv, Grid& grid, rect bounds)
   {
      context ctx{tv.view_, tv.cnv, &grid, bounds};
      grid.limits(ctx);
      grid.layout(ctx);
      grid.draw(ctx);
   }

   template <typename E>
   view_limits limits_of(test_view& tv, E& e)
   {
      basic_context ctx{tv.view_, tv.cnv};
      return e.limits(ctx);
   }

   // An element that records the bounds it is laid out and drawn with.
   struct layout_probe : probe
   {
      void layout(context const& ctx) override { laid = ctx.bounds; }

      rect laid = {};
   };

   // Positions that die when the function returns. They are computed at
   // run time, to be on the stack.
   [[gnu::noinline]] auto make_row(probe& a, probe& b, float first)
   {
      std::array<float, 2> const positions = {first, 1.0f};
      return hgrid(positions, link(a), link(b));
   }

   [[gnu::noinline]] auto make_braced_row(probe& a, probe& b, float first)
   {
      return hgrid({first, 1.0f}, link(a), link(b));
   }

   // Overwrites the stack below the caller.
   [[gnu::noinline]] void clobber()
   {
      volatile float junk[1024];
      for (auto& j : junk)
         j = 0.9f;
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
}

TEST_CASE("grids page: the factories and the composites", "[grids_page]")
{
   static float const positions[] = {0.25, 0.5, 1.0};
   static std::array<float, 3> const array_positions = {0.25, 0.5, 1.0};
   probe a, b, c;

   auto h = hgrid(positions, link(a), link(b), link(c));
   CHECK(std::is_same_v<decltype(h), array_composite<3, range_grid<hgrid_element>>>);
   CHECK(std::is_same_v<decltype(h)::container_type, std::array<element_ptr, 3>>);
   auto ha = hgrid(array_positions, link(a), link(b), link(c));
   CHECK(std::is_same_v<decltype(ha), array_composite<3, range_grid<hgrid_element>>>);
   auto he = hgrid(link(a), link(b), link(c));
   CHECK(std::is_same_v<decltype(he), array_composite<3, equal_grid<hgrid_element>>>);
   CHECK(std::is_same_v<hgrid_composite,
      vector_composite<container_grid<std::vector<float>, hgrid_element>>>);
   CHECK(std::is_base_of_v<grid_base, hgrid_element>);

   auto v = vgrid(positions, link(a), link(b), link(c));
   CHECK(std::is_same_v<decltype(v), array_composite<3, range_grid<vgrid_element>>>);
   auto va = vgrid(array_positions, link(a), link(b), link(c));
   CHECK(std::is_same_v<decltype(va), array_composite<3, range_grid<vgrid_element>>>);
   auto ve = vgrid(link(a), link(b), link(c));
   CHECK(std::is_same_v<decltype(ve), array_composite<3, equal_grid<vgrid_element>>>);
   CHECK(std::is_same_v<vgrid_composite,
      vector_composite<container_grid<std::vector<float>, vgrid_element>>>);
   CHECK(std::is_base_of_v<grid_base, vgrid_element>);

   CHECK(std::is_base_of_v<composite_base, grid_base>);
}

TEST_CASE("grids page: the elements are laid out at the supplied positions", "[grids_page]")
{
   test_view tv{extent{400, 400}};
   static float const positions[] = {0.25, 0.5, 1.0};
   probe a, b, c;

   // A single row, left to right, with no space between: the first at the
   // left, the second at positions[0], the third at positions[1].
   auto row = hgrid(positions, link(a), link(b), link(c));
   lay_out(tv, row, rect{20, 30, 220, 80});
   CHECK(a.drawn == rect{20, 30, 70, 80});
   CHECK(b.drawn == rect{70, 30, 120, 80});
   CHECK(c.drawn == rect{120, 30, 220, 80});

   // A single column, top to bottom.
   auto col = vgrid(positions, link(a), link(b), link(c));
   lay_out(tv, col, rect{20, 30, 70, 230});
   CHECK(a.drawn == rect{20, 30, 70, 80});
   CHECK(b.drawn == rect{20, 80, 70, 130});
   CHECK(c.drawn == rect{20, 130, 70, 230});

   // The positions in a std::array: the same.
   static std::array<float, 3> const array_positions = {0.25, 0.5, 1.0};
   probe d, e, f;
   auto arow = hgrid(array_positions, link(d), link(e), link(f));
   lay_out(tv, arow, rect{20, 30, 220, 80});
   CHECK(d.drawn == rect{20, 30, 70, 80});
   CHECK(e.drawn == rect{70, 30, 120, 80});
   CHECK(f.drawn == rect{120, 30, 220, 80});
}

TEST_CASE("grids page: without positions, the columns and rows are equal", "[grids_page]")
{
   test_view tv{extent{400, 400}};
   probe a, b, c, d;

   auto row = hgrid(link(a), link(b), link(c), link(d));
   lay_out(tv, row, rect{0, 0, 200, 50});
   CHECK(a.drawn == rect{0, 0, 50, 50});
   CHECK(b.drawn == rect{50, 0, 100, 50});
   CHECK(c.drawn == rect{100, 0, 150, 50});
   CHECK(d.drawn == rect{150, 0, 200, 50});

   auto col = vgrid(link(a), link(b), link(c), link(d));
   lay_out(tv, col, rect{0, 0, 50, 200});
   CHECK(a.drawn == rect{0, 0, 50, 50});
   CHECK(b.drawn == rect{0, 50, 50, 100});
   CHECK(c.drawn == rect{0, 100, 50, 150});
   CHECK(d.drawn == rect{0, 150, 50, 200});
}

TEST_CASE("grids page: the limits across the grid", "[grids_page]")
{
   test_view tv{extent{400, 400}};

   SECTION("hgrid: the maximum of the minimums, the minimum of the maximums")
   {
      probe a{{{10, 20}, {100, 80}}}, b{{{10, 30}, {100, 60}}};
      auto row = hgrid(link(a), link(b));
      auto l = limits_of(tv, row);
      CHECK(l.min.y == 30);
      CHECK(l.max.y == 60);
   }

   SECTION("vgrid: the maximum of the minimums, the minimum of the maximums")
   {
      probe a{{{20, 10}, {80, 100}}}, b{{{30, 10}, {60, 100}}};
      auto col = vgrid(link(a), link(b));
      auto l = limits_of(tv, col);
      CHECK(l.min.x == 30);
      CHECK(l.max.x == 60);
   }

   SECTION("the maximum is not less than the minimum")
   {
      probe a{{{10, 50}, {100, 200}}}, b{{{10, 0}, {100, 20}}};
      auto row = hgrid(link(a), link(b));
      auto l = limits_of(tv, row);
      CHECK(l.min.y == 50);
      CHECK(l.max.y == 50);

      probe c{{{50, 10}, {200, 100}}}, d{{{0, 10}, {20, 100}}};
      auto col = vgrid(link(c), link(d));
      auto m = limits_of(tv, col);
      CHECK(m.min.x == 50);
      CHECK(m.max.x == 50);
   }
}

TEST_CASE("grids page: a composite grid follows its external positions", "[grids_page]")
{
   test_view tv{extent{400, 400}};
   std::vector<float> positions = {0.25, 1.0};
   probe a, b;

   hgrid_composite c{positions};
   c.push_back(share(link(a)));
   c.push_back(share(link(b)));
   CHECK(c.size() == 2);
   lay_out(tv, c, rect{0, 0, 200, 50});
   CHECK(a.drawn == rect{0, 0, 50, 50});
   CHECK(b.drawn == rect{50, 0, 200, 50});

   // The grid keeps no copy: a change to the vector moves the elements.
   positions[0] = 0.5;
   lay_out(tv, c, rect{0, 0, 200, 50});
   CHECK(a.drawn == rect{0, 0, 100, 50});
   CHECK(b.drawn == rect{100, 0, 200, 50});

   vgrid_composite vc{positions};
   vc.push_back(share(link(a)));
   vc.push_back(share(link(b)));
   lay_out(tv, vc, rect{0, 0, 50, 200});
   CHECK(a.drawn == rect{0, 0, 50, 100});
   CHECK(b.drawn == rect{0, 100, 50, 200});

   // Shared into another grid, like any element.
   auto hc = std::make_shared<hgrid_composite>(positions);
   hc->push_back(share(link(a)));
   hc->push_back(share(link(b)));
   probe top;
   auto col = vgrid(link(top), hold(hc));
   lay_out(tv, col, rect{0, 0, 200, 100});
   CHECK(top.drawn == rect{0, 0, 200, 50});
   CHECK(a.drawn == rect{0, 50, 100, 100});
   CHECK(b.drawn == rect{100, 50, 200, 100});
}

TEST_CASE("grids page: make_equal_grid", "[grids_page]")
{
   auto p = make_equal_grid<4>();
   CHECK(std::is_same_v<decltype(p), std::array<float, 4>>);
   CHECK(p[0] == Approx(0.25));
   CHECK(p[1] == Approx(0.5));
   CHECK(p[2] == Approx(0.75));
   CHECK(p[3] == 1.0f);

   // The same layout as the grid without positions.
   test_view tv{extent{400, 400}};
   static auto const positions = make_equal_grid<4>();
   probe a, b, c, d;
   auto row = hgrid(positions, link(a), link(b), link(c), link(d));
   lay_out(tv, row, rect{0, 0, 200, 50});
   CHECK(a.drawn == rect{0, 0, 50, 50});
   CHECK(b.drawn == rect{50, 0, 100, 50});
   CHECK(c.drawn == rect{100, 0, 150, 50});
   CHECK(d.drawn == rect{150, 0, 200, 50});
}

TEST_CASE("grids page: positions in a non-const array", "[grids_page]")
{
   test_view tv{extent{400, 400}};
   static float positions[] = {0.25, 0.5, 1.0};
   std::array<float, 3> array_positions = {0.25, 0.5, 1.0};
   probe a, b, c;

   auto row = hgrid(positions, link(a), link(b), link(c));
   lay_out(tv, row, rect{0, 0, 200, 50});
   CHECK(a.drawn == rect{0, 0, 50, 50});
   CHECK(b.drawn == rect{50, 0, 100, 50});
   CHECK(c.drawn == rect{100, 0, 200, 50});

   auto arow = hgrid(array_positions, link(a), link(b), link(c));
   lay_out(tv, arow, rect{0, 0, 200, 50});
   CHECK(a.drawn == rect{0, 0, 50, 50});
   CHECK(c.drawn == rect{100, 0, 200, 50});

   auto col = vgrid(positions, link(a), link(b), link(c));
   lay_out(tv, col, rect{0, 0, 50, 200});
   CHECK(a.drawn == rect{0, 0, 50, 50});
   CHECK(c.drawn == rect{0, 100, 50, 200});

   auto acol = vgrid(array_positions, link(a), link(b), link(c));
   lay_out(tv, acol, rect{0, 0, 50, 200});
   CHECK(b.drawn == rect{0, 50, 50, 100});
}

TEST_CASE("grids page: the maximum is not less than the minimum along the grid", "[grids_page]")
{
   test_view tv{extent{400, 400}};
   static float const positions[] = {0.25, 1.0};

   // Each fixed at 10: the first needs 40 for its quarter.
   probe a{{{10, 10}, {10, 10}}}, b{{{10, 10}, {10, 10}}};
   auto row = hgrid(positions, link(a), link(b));
   auto l = limits_of(tv, row);
   CHECK(l.min.x == 40);
   CHECK(l.max.x == 40);

   auto col = vgrid(positions, link(a), link(b));
   auto m = limits_of(tv, col);
   CHECK(m.min.y == 40);
   CHECK(m.max.y == 40);
}

TEST_CASE("grids page: the children are laid out where they are drawn", "[grids_page]")
{
   test_view tv{extent{400, 400}};
   layout_probe a, b;

   auto row = hgrid(link(a), link(b));
   lay_out(tv, row, rect{100, 30, 200, 80});
   CHECK(a.laid == rect{100, 30, 150, 80});
   CHECK(b.laid == rect{150, 30, 200, 80});
   CHECK(a.laid == a.drawn);
   CHECK(b.laid == b.drawn);

   auto col = vgrid(link(a), link(b));
   lay_out(tv, col, rect{30, 100, 80, 200});
   CHECK(a.laid == rect{30, 100, 80, 150});
   CHECK(b.laid == rect{30, 150, 80, 200});
   CHECK(a.laid == a.drawn);
   CHECK(b.laid == b.drawn);
}

TEST_CASE("grids page: a std::array or a braced list of positions is copied", "[grids_page]")
{
   test_view tv{extent{400, 400}};
   probe a, b;

   auto row = make_row(a, b, 0.25f);
   clobber();
   lay_out(tv, row, rect{0, 0, 200, 50});
   CHECK(a.drawn == rect{0, 0, 50, 50});
   CHECK(b.drawn == rect{50, 0, 200, 50});

   probe c, d;
   auto braced = make_braced_row(c, d, 0.25f);
   clobber();
   lay_out(tv, braced, rect{0, 0, 200, 50});
   CHECK(c.drawn == rect{0, 0, 50, 50});
   CHECK(d.drawn == rect{50, 0, 200, 50});

   // make_equal_grid in place.
   probe e, f;
   auto equal = hgrid(make_equal_grid<2>(), link(e), link(f));
   lay_out(tv, equal, rect{0, 0, 200, 50});
   CHECK(e.drawn == rect{0, 0, 100, 50});
   CHECK(f.drawn == rect{100, 0, 200, 50});
}

TEST_CASE("grids page: the example", "[grids_page]")
{
   test_view tv{extent{300, 210}};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif

   static float const columns[] = {0.25, 0.5, 1.0};

   auto table = vgrid(
      hgrid(columns,
         box(colors::red),
         box(colors::green),
         box(colors::blue)
      ),
      hgrid(columns,
         box(colors::gold),
         span(2, box(colors::steel_blue))
      ),
      hgrid(box(colors::slate_gray), box(colors::white))
   );

   tv.view_.content(table);
   tv.draw();

   // Rows 0 to 70, 70 to 140, 140 to 210. Columns 0 to 75, 75 to 150 and
   // 150 to 300; the last row is split at 150.
#if !defined(ARTIST_RECORDING)
   auto red = pixel_at(tv.img, 37, 35);
   CHECK(red.r > 200);
   CHECK(red.g < 50);
   auto green = pixel_at(tv.img, 112, 35);
   CHECK(green.g > 100);
   CHECK(green.r < 50);
   auto blue = pixel_at(tv.img, 225, 35);
   CHECK(blue.b > 200);
   CHECK(blue.r < 50);
   auto gold = pixel_at(tv.img, 37, 105);
   CHECK(gold.r > 200);
   CHECK(gold.b < 50);
   for (int x : {112, 225})
   {
      auto steel = pixel_at(tv.img, x, 105);
      CHECK(steel.b > 150);
      CHECK(steel.r < 100);
   }
   auto slate = pixel_at(tv.img, 75, 175);
   CHECK(slate.r > 100);
   CHECK(slate.r < 125);
   CHECK(slate.b > 130);
   CHECK(slate.b < 155);
   auto white = pixel_at(tv.img, 225, 175);
   CHECK(white.r > 240);
   CHECK(white.g > 240);
   CHECK(white.b > 240);
#endif
}
