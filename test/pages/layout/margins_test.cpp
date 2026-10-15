/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/layout/margins.adoc.
=============================================================================*/
#include "test_support.hpp"
#include <type_traits>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   // An element with the given limits that records the bounds it is drawn
   // with.
   struct probe : element
   {
      probe(view_limits lim = {{40, 20}, {40, 20}}) : lim(lim) {}

      view_limits limits(basic_context const&) const override
      {
         return lim;
      }

      void draw(context const& ctx) override { drawn = ctx.bounds; }

      view_limits lim;
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

   template <typename E>
   view_limits limits_of(test_view& tv, E& e)
   {
      basic_context ctx{tv.view_, tv.cnv};
      return e.limits(ctx);
   }

   // The subject of `e` (a link to `p`) is laid out in `bounds` shrunk by
   // `m`, and the limits of `e` are those of `p` grown by `m`.
   template <typename E>
   void check_margin(E& e, probe& p, rect m)
   {
      test_view tv{extent{400, 400}};
      rect const bounds{10, 10, 210, 110};

      lay_out(tv, e, bounds);
      CHECK(p.drawn.left == Approx(bounds.left + m.left));
      CHECK(p.drawn.top == Approx(bounds.top + m.top));
      CHECK(p.drawn.right == Approx(bounds.right - m.right));
      CHECK(p.drawn.bottom == Approx(bounds.bottom - m.bottom));

      auto l = limits_of(tv, e);
      CHECK(l.min.x == Approx(p.lim.min.x + m.left + m.right));
      CHECK(l.max.x == Approx(p.lim.max.x + m.left + m.right));
      CHECK(l.min.y == Approx(p.lim.min.y + m.top + m.bottom));
      CHECK(l.max.y == Approx(p.lim.max.y + m.top + m.bottom));
   }
}

TEST_CASE("margins page: each factory returns a margin_element", "[margins_page]")
{
   CHECK(std::is_same_v<decltype(margin({1, 2, 3, 4}, probe{})), margin_element<rect, probe>>);
   CHECK(std::is_same_v<decltype(margin_left(1, probe{})), margin_element<margin_left_rect, probe>>);
   CHECK(std::is_same_v<decltype(margin_right(1, probe{})), margin_element<margin_right_rect, probe>>);
   CHECK(std::is_same_v<decltype(margin_top(1, probe{})), margin_element<margin_top_rect, probe>>);
   CHECK(std::is_same_v<decltype(margin_bottom(1, probe{})), margin_element<margin_bottom_rect, probe>>);
   CHECK(std::is_same_v<decltype(hmargin(1, probe{})), margin_element<hmargin_rect, probe>>);
   CHECK(std::is_same_v<decltype(margin_left_right(1, probe{})), margin_element<hmargin_rect, probe>>);
   CHECK(std::is_same_v<decltype(vmargin(1, probe{})), margin_element<vmargin_rect, probe>>);
   CHECK(std::is_same_v<decltype(margin_top_bottom(1, probe{})), margin_element<vmargin_rect, probe>>);
   CHECK(std::is_same_v<decltype(margin_left_top({1, 2}, probe{})), margin_element<margin_left_top_rect, probe>>);
   CHECK(std::is_same_v<decltype(margin_left_bottom({1, 2}, probe{})), margin_element<margin_left_bottom_rect, probe>>);
   CHECK(std::is_same_v<decltype(margin_right_top({1, 2}, probe{})), margin_element<margin_right_top_rect, probe>>);
   CHECK(std::is_same_v<decltype(margin_right_bottom({1, 2}, probe{})), margin_element<margin_right_bottom_rect, probe>>);

   // The three-argument forms return the same types as the brace forms.
   CHECK(std::is_same_v<decltype(hmargin(1, 2, probe{})), margin_element<hmargin_rect, probe>>);
   CHECK(std::is_same_v<decltype(margin_left_right(1, 2, probe{})), margin_element<hmargin_rect, probe>>);
   CHECK(std::is_same_v<decltype(vmargin(1, 2, probe{})), margin_element<vmargin_rect, probe>>);
   CHECK(std::is_same_v<decltype(margin_top_bottom(1, 2, probe{})), margin_element<vmargin_rect, probe>>);
   CHECK(std::is_same_v<decltype(margin_left_top(1, 2, probe{})), margin_element<margin_left_top_rect, probe>>);
   CHECK(std::is_same_v<decltype(margin_left_bottom(1, 2, probe{})), margin_element<margin_left_bottom_rect, probe>>);
   CHECK(std::is_same_v<decltype(margin_right_top(1, 2, probe{})), margin_element<margin_right_top_rect, probe>>);
   CHECK(std::is_same_v<decltype(margin_right_bottom(1, 2, probe{})), margin_element<margin_right_bottom_rect, probe>>);

   CHECK(concepts::Rect<rect>);
   CHECK(concepts::Rect<static_empty_rect>);
   CHECK(concepts::Rect<margin_left_rect>);
   CHECK(concepts::Rect<hmargin_rect>);
   CHECK(concepts::Rect<margin_left_bottom_rect>);
   CHECK(concepts::Rect<margin_right_top_rect>);
   CHECK(concepts::Rect<margin_right_bottom_rect>);
}

TEST_CASE("margins page: the other sides of a static_empty_rect are zero", "[margins_page]")
{
   margin_left_rect r{5};
   CHECK(r.left == 5);
   CHECK(r.right == 0);
   CHECK(r.top == 0);
   CHECK(r.bottom == 0);

   margin_left_top_rect lt{5, 6};
   CHECK(lt.left == 5);
   CHECK(lt.top == 6);
   CHECK(lt.right == 0);
   CHECK(lt.bottom == 0);
}

TEST_CASE("margins page: margin adds space on all sides", "[margins_page]")
{
   probe p;
   auto e = margin({1, 2, 3, 4}, link(p));
   check_margin(e, p, {1, 2, 3, 4});
}

TEST_CASE("margins page: a margin on one side", "[margins_page]")
{
   {
      probe p;
      auto e = margin_left(5, link(p));
      check_margin(e, p, {5, 0, 0, 0});
   }
   {
      probe p;
      auto e = margin_right(5, link(p));
      check_margin(e, p, {0, 0, 5, 0});
   }
   {
      probe p;
      auto e = margin_top(5, link(p));
      check_margin(e, p, {0, 5, 0, 0});
   }
   {
      probe p;
      auto e = margin_bottom(5, link(p));
      check_margin(e, p, {0, 0, 0, 5});
   }
}

TEST_CASE("margins page: hmargin and margin_left_right", "[margins_page]")
{
   {
      probe p;
      auto e = hmargin({3, 7}, link(p));
      check_margin(e, p, {3, 0, 7, 0});
   }
   {
      probe p;
      auto e = hmargin(3, 7, link(p));
      check_margin(e, p, {3, 0, 7, 0});
   }
   {
      probe p;
      auto e = hmargin(6, link(p));
      check_margin(e, p, {6, 0, 6, 0});
   }
   {
      probe p;
      auto e = margin_left_right({3, 7}, link(p));
      check_margin(e, p, {3, 0, 7, 0});
   }
   {
      probe p;
      auto e = margin_left_right(3, 7, link(p));
      check_margin(e, p, {3, 0, 7, 0});
   }
   {
      probe p;
      auto e = margin_left_right(6, link(p));
      check_margin(e, p, {6, 0, 6, 0});
   }
}

TEST_CASE("margins page: vmargin and margin_top_bottom", "[margins_page]")
{
   {
      probe p;
      auto e = vmargin({3, 7}, link(p));
      check_margin(e, p, {0, 3, 0, 7});
   }
   {
      probe p;
      auto e = vmargin(3, 7, link(p));
      check_margin(e, p, {0, 3, 0, 7});
   }
   {
      probe p;
      auto e = vmargin(6, link(p));
      check_margin(e, p, {0, 6, 0, 6});
   }
   {
      probe p;
      auto e = margin_top_bottom({3, 7}, link(p));
      check_margin(e, p, {0, 3, 0, 7});
   }
   {
      probe p;
      auto e = margin_top_bottom(3, 7, link(p));
      check_margin(e, p, {0, 3, 0, 7});
   }
   {
      probe p;
      auto e = margin_top_bottom(6, link(p));
      check_margin(e, p, {0, 6, 0, 6});
   }
}

TEST_CASE("margins page: margins on two adjacent sides", "[margins_page]")
{
   {
      probe p;
      auto e = margin_left_top({3, 7}, link(p));
      check_margin(e, p, {3, 7, 0, 0});
   }
   {
      probe p;
      auto e = margin_left_top(3, 7, link(p));
      check_margin(e, p, {3, 7, 0, 0});
   }
   {
      probe p;
      auto e = margin_left_bottom({3, 7}, link(p));
      check_margin(e, p, {3, 0, 0, 7});
   }
   {
      probe p;
      auto e = margin_left_bottom(3, 7, link(p));
      check_margin(e, p, {3, 0, 0, 7});
   }
   {
      probe p;
      auto e = margin_right_top({3, 7}, link(p));
      check_margin(e, p, {0, 7, 3, 0});
   }
   {
      probe p;
      auto e = margin_right_top(3, 7, link(p));
      check_margin(e, p, {0, 7, 3, 0});
   }
   {
      probe p;
      auto e = margin_right_bottom({3, 7}, link(p));
      check_margin(e, p, {0, 0, 3, 7});
   }
   {
      probe p;
      auto e = margin_right_bottom(3, 7, link(p));
      check_margin(e, p, {0, 0, 3, 7});
   }
}

TEST_CASE("margins page: the subject's limits and resizability are kept", "[margins_page]")
{
   test_view tv{extent{400, 400}};

   // A resizable subject stays resizable, by the same range.
   {
      probe p{{{40, 20}, {100, 50}}};
      auto e = margin({1, 2, 3, 4}, link(p));
      check_margin(e, p, {1, 2, 3, 4});
      auto l = limits_of(tv, e);
      CHECK(l.max.x - l.min.x == Approx(60));
      CHECK(l.max.y - l.min.y == Approx(30));
   }

   // A full maximum stays full.
   {
      probe p{{{40, 20}, {full_extent, full_extent}}};
      auto e = margin({1, 2, 3, 4}, link(p));
      auto l = limits_of(tv, e);
      CHECK(l.min == point{44, 26});
      CHECK(l.max.x == full_extent);
      CHECK(l.max.y == full_extent);
   }
}
