/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/layout/alignment.adoc.
=============================================================================*/
#include "test_support.hpp"
#include <type_traits>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   // A 40 by 20 element that records the bounds it is drawn with.
   struct probe : element
   {
      view_limits limits(basic_context const&) const override
      {
         return {{40, 20}, {40, 20}};
      }

      void draw(context const& ctx) override { drawn = ctx.bounds; }

      rect drawn = {};
   };

   template <typename E>
   void lay_out(test_view& tv, E& e, rect bounds)
   {
      context ctx{tv.view_, tv.cnv, &e, bounds};
      e.limits(ctx);
      e.layout(ctx);
      e.draw(ctx);
   }
}

TEST_CASE("alignment page: the factories set the alignment", "[alignment_page]")
{
   CHECK(halign(0.25, probe{}).align() == Approx(0.25));
   CHECK(align_left(probe{}).align() == Approx(0.0));
   CHECK(align_center(probe{}).align() == Approx(0.5));
   CHECK(align_right(probe{}).align() == Approx(1.0));

   CHECK(valign(0.25, probe{}).align() == Approx(0.25));
   CHECK(align_top(probe{}).align() == Approx(0.0));
   CHECK(align_middle(probe{}).align() == Approx(0.5));
   CHECK(align_bottom(probe{}).align() == Approx(1.0));

   using halign_type = decltype(halign(0, probe{}));
   using valign_type = decltype(valign(0, probe{}));
   CHECK(std::is_same_v<halign_type, halign_element<probe>>);
   CHECK(std::is_same_v<valign_type, valign_element<probe>>);
   CHECK(std::is_same_v<decltype(align_left(probe{})), halign_type>);
   CHECK(std::is_same_v<decltype(align_bottom(probe{})), valign_type>);
}

TEST_CASE("alignment page: halign positions the subject in X", "[alignment_page]")
{
   test_view tv{extent{400, 400}};
   rect const bounds{10, 10, 210, 60};   // X = 200

   for (float a : {0.0f, 0.25f, 0.5f, 1.0f})
   {
      probe p;
      auto e = halign(a, link(p));
      lay_out(tv, e, bounds);

      // The point at `a` of the subject's width lands at `X * a`.
      CHECK(p.drawn.left + p.drawn.width() * a == Approx(10 + 200 * a));
      CHECK(p.drawn.width() == Approx(40));
      CHECK(p.drawn.top == Approx(10));
      CHECK(p.drawn.height() == Approx(50));
   }
}

TEST_CASE("alignment page: valign positions the subject in Y", "[alignment_page]")
{
   test_view tv{extent{400, 400}};
   rect const bounds{10, 10, 110, 210};  // Y = 200

   for (float a : {0.0f, 0.25f, 0.5f, 1.0f})
   {
      probe p;
      auto e = valign(a, link(p));
      lay_out(tv, e, bounds);

      CHECK(p.drawn.top + p.drawn.height() * a == Approx(10 + 200 * a));
      CHECK(p.drawn.height() == Approx(20));
      CHECK(p.drawn.left == Approx(10));
      CHECK(p.drawn.width() == Approx(100));
   }
}

TEST_CASE("alignment page: the align setter moves the subject", "[alignment_page]")
{
   test_view tv{extent{400, 400}};
   probe p;
   auto e = align_left(link(p));
   lay_out(tv, e, rect{0, 0, 200, 50});
   CHECK(p.drawn.left == Approx(0));

   e.align(1.0);
   CHECK(e.align() == Approx(1.0));
   lay_out(tv, e, rect{0, 0, 200, 50});
   CHECK(p.drawn.left == Approx(160));
}

TEST_CASE("alignment page: combined alignment nests valign in halign", "[alignment_page]")
{
   using combined = halign_element<valign_element<probe>>;
   CHECK(std::is_same_v<decltype(align_left_top(probe{})), combined>);
   CHECK(std::is_same_v<decltype(align_right_bottom(probe{})), combined>);

   auto check = [](auto e, float h, float v)
   {
      CHECK(e.align() == Approx(h));
      CHECK(e.actual_subject().align() == Approx(v));
   };
   check(align_left_top(probe{}), 0.0, 0.0);
   check(align_center_top(probe{}), 0.5, 0.0);
   check(align_right_top(probe{}), 1.0, 0.0);
   check(align_left_middle(probe{}), 0.0, 0.5);
   check(align_center_middle(probe{}), 0.5, 0.5);
   check(align_right_middle(probe{}), 1.0, 0.5);
   check(align_left_bottom(probe{}), 0.0, 1.0);
   check(align_center_bottom(probe{}), 0.5, 1.0);
   check(align_right_bottom(probe{}), 1.0, 1.0);

   // Both setters, then a layout: the subject lands at the right-bottom.
   test_view tv{extent{400, 400}};
   probe p;
   auto e = align_left_top(link(p));
   e.align(1.0);
   e.actual_subject().align(1.0);
   lay_out(tv, e, rect{0, 0, 200, 100});
   CHECK(p.drawn == rect{160, 80, 200, 100});
}
