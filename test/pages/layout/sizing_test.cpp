/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/layout/sizing.adoc.
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
}

TEST_CASE("sizing page: min_size gives the subject its minimum", "[sizing_page]")
{
   // Given less room than its minimum, the subject is given its minimum
   // width and its minimum height.
   test_view tv{extent{200, 200}};
   probe p;
   auto ms = min_size({50, 80}, link(p));
   context ctx{tv.view_, tv.cnv, &ms, rect{0, 0, 20, 30}};
   ms.draw(ctx);
   CHECK(p.drawn.width() == 50);
   CHECK(p.drawn.height() == 80);
}

TEST_CASE("sizing page: limit keeps within the subject's own limits", "[sizing_page]")
{
   test_view tv{extent{200, 200}};
   basic_context bctx{tv.view_, tv.cnv};

   // A subject from 20 by 10 up to 300 by 100.
   view_limits const sl = {{20, 10}, {300, 100}};

   // Within the subject's limits, limit narrows them.
   auto in = limit({{30, 20}, {200, 80}}, probe{sl}).limits(bctx);
   CHECK(in.min == point{30, 20});
   CHECK(in.max == point{200, 80});

   // Past them, it stops at them: the minimum width asked for is above the
   // subject's maximum, and the maximum height below its minimum.
   auto out = limit({{400, 0}, {full_extent, 5}}, probe{sl}).limits(bctx);
   CHECK(out.min.x == 300);
   CHECK(out.max.x == 300);
   CHECK(out.min.y == 10);
   CHECK(out.max.y == 10);
}

TEST_CASE("sizing page: the min_size family keeps within the subject's limits", "[sizing_page]")
{
   // A subject from 20 by 10 up to 100 by 100, asked for a minimum past its
   // maximum, is laid out no larger than its maximum.
   test_view tv{extent{400, 400}};
   view_limits const sl = {{20, 10}, {100, 100}};

   probe p1{sl};
   auto ms = min_size({200, 300}, link(p1));
   context c1{tv.view_, tv.cnv, &ms, rect{0, 0, 10, 10}};
   ms.layout(c1);
   ms.draw(c1);
   CHECK(p1.drawn.width() == 100);
   CHECK(p1.drawn.height() == 100);

   probe p2{sl};
   auto hs = hmin_size(200, link(p2));
   context c2{tv.view_, tv.cnv, &hs, rect{0, 0, 10, 50}};
   hs.layout(c2);
   hs.draw(c2);
   CHECK(p2.drawn.width() == 100);

   probe p3{sl};
   auto vs = vmin_size(300, link(p3));
   context c3{tv.view_, tv.cnv, &vs, rect{0, 0, 50, 10}};
   vs.layout(c3);
   vs.draw(c3);
   CHECK(p3.drawn.height() == 100);
}

TEST_CASE("sizing page: hmin_pad widens the subject by the pad", "[sizing_page]")
{
   // A subject at least 40 wide, padded by 30, is given at least 70.
   test_view tv{extent{400, 400}};
   probe p{{{40, 10}, {300, 100}}};
   auto hp = hmin_pad(30, link(p));
   context ctx{tv.view_, tv.cnv, &hp, rect{0, 0, 10, 50}};
   hp.layout(ctx);
   hp.draw(ctx);
   CHECK(p.drawn.width() == 70);
}

TEST_CASE("sizing page: a min_size family setter keeps within the subject's limits", "[sizing_page]")
{
   // A subject from 20 by 10 up to 100 by 100. Each proxy is laid out with a
   // minimum of 50, then set to a minimum past the subject's maximum. Drawn
   // before the next layout, the subject keeps its previous size; after it,
   // the subject is no larger than its maximum.
   test_view tv{extent{400, 400}};
   view_limits const sl = {{20, 10}, {100, 100}};

   probe p1{sl};
   auto ms = min_size({50, 50}, link(p1));
   context c1{tv.view_, tv.cnv, &ms, rect{0, 0, 10, 10}};
   ms.layout(c1);
   ms.min_size({200, 300});
   CHECK(ms.min_size() == point{200, 300});
   ms.draw(c1);
   CHECK(p1.drawn.width() == 50);
   CHECK(p1.drawn.height() == 50);
   ms.layout(c1);
   ms.draw(c1);
   CHECK(p1.drawn.width() == 100);
   CHECK(p1.drawn.height() == 100);

   probe p2{sl};
   auto hs = hmin_size(50, link(p2));
   context c2{tv.view_, tv.cnv, &hs, rect{0, 0, 10, 50}};
   hs.layout(c2);
   hs.hmin_size(200);
   CHECK(hs.hmin_size() == 200);
   hs.draw(c2);
   CHECK(p2.drawn.width() == 50);
   hs.layout(c2);
   hs.draw(c2);
   CHECK(p2.drawn.width() == 100);

   probe p3{sl};
   auto vs = vmin_size(50, link(p3));
   context c3{tv.view_, tv.cnv, &vs, rect{0, 0, 50, 10}};
   vs.layout(c3);
   vs.vmin_size(300);
   CHECK(vs.vmin_size() == 300);
   vs.draw(c3);
   CHECK(p3.drawn.height() == 50);
   vs.layout(c3);
   vs.draw(c3);
   CHECK(p3.drawn.height() == 100);
}

TEST_CASE("sizing page: a max_size family setter keeps within the subject's limits", "[sizing_page]")
{
   // A subject at least 80 by 80. Each proxy is laid out with a maximum of
   // 120, then set to a maximum below the subject's minimum. Drawn before
   // the next layout, the subject keeps its previous size; after it, the
   // subject is no smaller than its minimum.
   test_view tv{extent{400, 400}};
   view_limits const sl = {{80, 80}, {300, 300}};

   probe p1{sl};
   auto ms = max_size({120, 120}, link(p1));
   context c1{tv.view_, tv.cnv, &ms, rect{0, 0, 300, 300}};
   ms.layout(c1);
   ms.max_size({50, 50});
   CHECK(ms.max_size() == point{50, 50});
   ms.draw(c1);
   CHECK(p1.drawn.width() == 120);
   CHECK(p1.drawn.height() == 120);
   ms.layout(c1);
   ms.draw(c1);
   CHECK(p1.drawn.width() == 80);
   CHECK(p1.drawn.height() == 80);

   probe p2{sl};
   auto hs = hmax_size(120, link(p2));
   context c2{tv.view_, tv.cnv, &hs, rect{0, 0, 300, 300}};
   hs.layout(c2);
   hs.hmax_size(50);
   CHECK(hs.hmax_size() == 50);
   hs.draw(c2);
   CHECK(p2.drawn.width() == 120);
   hs.layout(c2);
   hs.draw(c2);
   CHECK(p2.drawn.width() == 80);

   probe p3{sl};
   auto vs = vmax_size(120, link(p3));
   context c3{tv.view_, tv.cnv, &vs, rect{0, 0, 300, 300}};
   vs.layout(c3);
   vs.vmax_size(50);
   CHECK(vs.vmax_size() == 50);
   vs.draw(c3);
   CHECK(p3.drawn.height() == 120);
   vs.layout(c3);
   vs.draw(c3);
   CHECK(p3.drawn.height() == 80);
}

TEST_CASE("sizing page: a negative hmin_size width clamps like vmin_size", "[sizing_page]")
{
   // A width below the subject's minimum, negative included, gives the
   // subject's minimum, as a height does for vmin_size.
   test_view tv{extent{200, 200}};
   basic_context bctx{tv.view_, tv.cnv};
   view_limits const sl = {{40, 30}, {300, 300}};

   CHECK(hmin_size(-20, probe{sl}).limits(bctx).min.x == 40);
   CHECK(vmin_size(-20, probe{sl}).limits(bctx).min.y == 30);
}

TEST_CASE("sizing page: hmin_pad keeps the pad apart from the width", "[sizing_page]")
{
   // A subject at least 40 wide, padded by 30, is given at least 70. The pad
   // reads back as the pad; no width is given, so the width reads as zero.
   test_view tv{extent{400, 400}};
   basic_context bctx{tv.view_, tv.cnv};
   probe p{{{40, 10}, {300, 100}}};
   auto hp = hmin_pad(30, link(p));
   CHECK(hp.pad() == 30);
   CHECK(hp.hmin_size() == 0);
   CHECK(hp.limits(bctx).min.x == 70);

   context ctx{tv.view_, tv.cnv, &hp, rect{0, 0, 10, 50}};
   hp.layout(ctx);
   hp.draw(ctx);
   CHECK(p.drawn.width() == 70);

   // A new pad takes effect at the next layout.
   hp.pad(50);
   CHECK(hp.pad() == 50);
   hp.draw(ctx);
   CHECK(p.drawn.width() == 70);
   hp.layout(ctx);
   hp.draw(ctx);
   CHECK(p.drawn.width() == 90);
}

TEST_CASE("sizing page: vmin_pad keeps the pad apart from the height", "[sizing_page]")
{
   // A subject at least 40 high, padded by 30, is given at least 70. The
   // pad reads back as the pad; no height is given, so the height reads as
   // zero.
   test_view tv{extent{400, 400}};
   basic_context bctx{tv.view_, tv.cnv};
   probe p{{{10, 40}, {100, 300}}};
   auto vp = vmin_pad(30, link(p));
   CHECK(vp.pad() == 30);
   CHECK(vp.vmin_size() == 0);
   CHECK(vp.limits(bctx).min.y == 70);

   context ctx{tv.view_, tv.cnv, &vp, rect{0, 0, 50, 10}};
   vp.layout(ctx);
   vp.draw(ctx);
   CHECK(p.drawn.height() == 70);

   // A new pad takes effect at the next layout.
   vp.pad(50);
   CHECK(vp.pad() == 50);
   vp.draw(ctx);
   CHECK(p.drawn.height() == 70);
   vp.layout(ctx);
   vp.draw(ctx);
   CHECK(p.drawn.height() == 90);
}

TEST_CASE("sizing page: hmin and vmin fix the subject at its natural minimum", "[sizing_page]")
{
   // A subject from 20 by 40 up to 300 by 300, in a layer of 200 by 200.
   test_view tv{extent{400, 400}};
   basic_context bctx{tv.view_, tv.cnv};
   view_limits const sl = {{20, 40}, {300, 300}};

   auto hl = hmin(probe{sl}).limits(bctx);
   CHECK(hl.min == point{20, 40});
   CHECK(hl.max == point{20, 300});

   auto vl = vmin(probe{sl}).limits(bctx);
   CHECK(vl.min == point{20, 40});
   CHECK(vl.max == point{300, 40});

   probe p1{sl};
   auto h = layer(hmin(link(p1)));
   context c1{tv.view_, tv.cnv, &h, rect{0, 0, 200, 200}};
   h.layout(c1);
   h.draw(c1);
   CHECK(p1.drawn.width() == 20);
   CHECK(p1.drawn.height() == 200);

   probe p2{sl};
   auto v = layer(vmin(link(p2)));
   context c2{tv.view_, tv.cnv, &v, rect{0, 0, 200, 200}};
   v.layout(c2);
   v.draw(c2);
   CHECK(p2.drawn.width() == 200);
   CHECK(p2.drawn.height() == 40);
}
