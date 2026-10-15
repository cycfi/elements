/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/layout/span.adoc.
=============================================================================*/
#include "test_support.hpp"

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   // An element that records the bounds it is drawn with.
   struct probe : element
   {
      void draw(context const& ctx) override { drawn = ctx.bounds; }

      rect drawn = {};
   };

   template <typename S>
   constexpr bool has_setter_of = false;

   template <typename S, typename T>
   constexpr bool has_setter_of<std::pair<S, T>> =
      requires { static_cast<void (S::*)(T)>(&S::span); };

   template <typename Grid>
   void lay_out(test_view& tv, Grid& grid, rect bounds)
   {
      context ctx{tv.view_, tv.cnv, &grid, bounds};
      grid.limits(ctx);
      grid.layout(ctx);
      grid.draw(ctx);
   }
}

TEST_CASE("span page: an element spans one by default", "[span_page]")
{
   CHECK(probe{}.span() == 1);
   CHECK(span(3, probe{}).span() == 3);
}

TEST_CASE("span page: a span of 2 occupies two grid columns", "[span_page]")
{
   test_view tv{extent{400, 400}};

   // The page's example: four equal columns, the middle element spanning
   // two of them.
   probe a, b, c;
   auto row = hgrid(link(a), span(2, link(b)), link(c));
   lay_out(tv, row, rect{0, 0, 200, 50});
   CHECK(a.drawn.width() == Approx(50));
   CHECK(b.drawn.left == Approx(50));
   CHECK(b.drawn.width() == Approx(100));
   CHECK(c.drawn.left == Approx(150));
   CHECK(c.drawn.width() == Approx(50));

   // The same with supplied positions: the span skips one position.
   static float const positions[] = {0.2, 0.5, 0.6, 1.0};
   probe d, e, f;
   auto row2 = hgrid(positions, link(d), span(2, link(e)), link(f));
   lay_out(tv, row2, rect{0, 0, 200, 50});
   CHECK(d.drawn.width() == Approx(40));
   CHECK(e.drawn.left == Approx(40));
   CHECK(e.drawn.right == Approx(120));
   CHECK(f.drawn.left == Approx(120));
   CHECK(f.drawn.right == Approx(200));
}

TEST_CASE("span page: a span of 2 occupies two grid rows", "[span_page]")
{
   test_view tv{extent{400, 400}};
   probe a, b, c;
   auto col = vgrid(span(2, link(a)), link(b), link(c));
   lay_out(tv, col, rect{0, 0, 50, 200});
   CHECK(a.drawn.height() == Approx(100));
   CHECK(b.drawn.top == Approx(100));
   CHECK(b.drawn.height() == Approx(50));
   CHECK(c.drawn.top == Approx(150));
   CHECK(c.drawn.height() == Approx(50));
}

TEST_CASE("span page: a span is at least 1", "[span_page]")
{
   CHECK(span(0, probe{}).span() == 1);

   auto s = span(3, probe{});
   s.span(0u);
   CHECK(s.span() == 1);
   s.span(2u);
   CHECK(s.span() == 2);

   // A span of 0 lays out as a span of 1.
   test_view tv{extent{400, 400}};
   probe a, b;
   auto row = hgrid(span(0, link(a)), link(b));
   lay_out(tv, row, rect{0, 0, 200, 50});
   CHECK(a.drawn.width() == Approx(100));
   CHECK(b.drawn.left == Approx(100));
   CHECK(b.drawn.width() == Approx(100));
}

TEST_CASE("span page: the span setter takes an unsigned", "[span_page]")
{
   using span_type = decltype(span(1, probe{}));
   CHECK(has_setter_of<std::pair<span_type, unsigned>>);
   CHECK(!has_setter_of<std::pair<span_type, float>>);
}

TEST_CASE("span page: a grid lays out before its limits are asked", "[span_page]")
{
   test_view tv{extent{400, 400}};

   probe a, b, c;
   auto row = hgrid(link(a), span(2, link(b)), link(c));
   context hctx{tv.view_, tv.cnv, &row, rect{0, 0, 200, 50}};
   row.layout(hctx);
   row.draw(hctx);
   CHECK(a.drawn.width() == Approx(50));
   CHECK(b.drawn.width() == Approx(100));
   CHECK(c.drawn.left == Approx(150));

   probe d, e, f;
   auto col = vgrid(span(2, link(d)), link(e), link(f));
   context vctx{tv.view_, tv.cnv, &col, rect{0, 0, 50, 200}};
   col.layout(vctx);
   col.draw(vctx);
   CHECK(d.drawn.height() == Approx(100));
   CHECK(e.drawn.top == Approx(100));
   CHECK(f.drawn.height() == Approx(50));
}
