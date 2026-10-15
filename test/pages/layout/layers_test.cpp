/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/layout/layers.adoc.
=============================================================================*/
#include "test_support.hpp"
#include <cstdint>
#include <type_traits>
#include <vector>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   // An element of the given limits that records the bounds it is drawn
   // with, and its turn in the drawing order.
   struct probe : element
   {
      probe(view_limits l = full_limits, bool control = false)
       : lim(l), control(control) {}

      view_limits limits(basic_context const&) const override { return lim; }
      bool wants_control() const override { return control; }

      bool click(context const&, mouse_button) override
      {
         ++clicks;
         return accept;
      }

      void draw(context const& ctx) override
      {
         drawn = ctx.bounds;
         if (order)
            order->push_back(this);
      }

      view_limits lim;
      bool control;
      rect drawn = {};
      std::vector<probe*>* order = nullptr;
      bool accept = true;
      int clicks = 0;
   };

   template <typename... E>
   concept layerable = requires(E&&... e) { layer(std::forward<E>(e)...); };

   template <typename... E>
   concept deckable = requires(E&&... e) { deck(std::forward<E>(e)...); };

   template <typename Layer>
   bool click_at(test_view& tv, Layer& l, rect bounds, point p)
   {
      context ctx{tv.view_, tv.cnv, &l, bounds};
      mouse_button down{true, 1, mouse_button::left, 0, p};
      return l.click(ctx, down);
   }

   template <typename Layer>
   void lay_out(test_view& tv, Layer& l, rect bounds)
   {
      context ctx{tv.view_, tv.cnv, &l, bounds};
      l.layout(ctx);
      l.draw(ctx);
   }

   template <typename Layer>
   element* hit(test_view& tv, Layer& l, rect bounds, point p)
   {
      context ctx{tv.view_, tv.cnv, &l, bounds};
      return l.hit_test(ctx, p, true, true);
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
}

TEST_CASE("layers page: the factory and the composite", "[layers_page]")
{
   probe a, b, c;

   auto l = layer(link(a), link(b), link(c));
   CHECK(std::is_same_v<decltype(l), array_composite<3, layer_element>>);
   CHECK(std::is_same_v<decltype(l)::container_type, std::array<element_ptr, 3>>);
   CHECK(std::is_same_v<layer_composite, vector_composite<layer_element>>);
   CHECK(std::is_base_of_v<composite_base, layer_element>);
   CHECK(l.size() == 3);
}

TEST_CASE("layers page: the factories take elements only", "[layers_page]")
{
   CHECK(layerable<probe, probe>);
   CHECK(!layerable<int, int>);
   CHECK(!layerable<probe, int>);
   CHECK(deckable<probe, probe>);
   CHECK(!deckable<int, int>);
   CHECK(!deckable<probe, int>);
}

TEST_CASE("layers page: the elements are drawn bottom-up", "[layers_page]")
{
   test_view tv{extent{400, 400}};
   std::vector<probe*> order;
   probe a, b, c;
   a.order = b.order = c.order = &order;

   // The first element is the top-most: it is drawn last.
   auto l = layer(link(a), link(b), link(c));
   lay_out(tv, l, rect{20, 30, 120, 80});
   REQUIRE(order.size() == 3);
   CHECK(order[0] == &c);
   CHECK(order[1] == &b);
   CHECK(order[2] == &a);

   // Each element is given the layer's bounds.
   CHECK(a.drawn == rect{20, 30, 120, 80});
   CHECK(b.drawn == rect{20, 30, 120, 80});
   CHECK(c.drawn == rect{20, 30, 120, 80});
}

TEST_CASE("layers page: UI control proceeds from top to bottom", "[layers_page]")
{
   test_view tv{extent{400, 400}};
   rect const bounds = {0, 0, 100, 100};

   SECTION("the higher-level element is given priority")
   {
      probe top{full_limits, true}, bottom{full_limits, true};
      auto l = layer(link(top), link(bottom));
      CHECK(hit(tv, l, bounds, {50, 50}) == &top);
   }

   SECTION("a higher-level element that takes no control passes it down")
   {
      probe top{full_limits, false}, bottom{full_limits, true};
      auto l = layer(link(top), link(bottom));
      CHECK(hit(tv, l, bounds, {50, 50}) == &bottom);
   }

   SECTION("a higher-level element not under the point passes it down")
   {
      probe top{{{0, 0}, {40, 40}}, true}, bottom{full_limits, true};
      auto l = layer(link(top), link(bottom));
      CHECK(hit(tv, l, bounds, {20, 20}) == &top);
      CHECK(hit(tv, l, bounds, {50, 50}) == &bottom);
   }
}

TEST_CASE("layers page: once an element is hit, the event goes to it alone", "[layers_page]")
{
   test_view tv{extent{400, 400}};
   rect const bounds = {0, 0, 100, 100};
   probe top{full_limits, true}, bottom{full_limits, true};
   top.accept = false;
   auto l = layer(link(top), link(bottom));
   CHECK(!click_at(tv, l, bounds, {50, 50}));
   CHECK(top.clicks == 1);
   CHECK(bottom.clicks == 0);
}

TEST_CASE("layers page: the limits of the layer", "[layers_page]")
{
   test_view tv{extent{400, 400}};

   SECTION("the maximum of the minimums, the minimum of the maximums")
   {
      probe a{{{10, 30}, {100, 60}}}, b{{{20, 20}, {80, 90}}};
      auto l = layer(link(a), link(b));
      auto lim = limits_of(tv, l);
      CHECK(lim.min.x == 20);
      CHECK(lim.min.y == 30);
      CHECK(lim.max.x == 80);
      CHECK(lim.max.y == 60);
   }

   SECTION("the maximum is not less than the minimum")
   {
      probe a{{{50, 50}, {200, 200}}}, b{{{0, 0}, {20, 30}}};
      auto l = layer(link(a), link(b));
      auto lim = limits_of(tv, l);
      CHECK(lim.max.x >= lim.min.x);
      CHECK(lim.max.y >= lim.min.y);
      CHECK(lim.max.x == 50);
      CHECK(lim.max.y == 50);
   }
}

TEST_CASE("layers page: an element past its maximum is aligned to the top-left", "[layers_page]")
{
   test_view tv{extent{400, 400}};
   probe a{{{0, 0}, {40, 30}}}, b;
   auto l = layer(link(a), link(b));
   lay_out(tv, l, rect{20, 30, 120, 80});
   CHECK(a.drawn == rect{20, 30, 60, 60});
   CHECK(b.drawn == rect{20, 30, 120, 80});
}

TEST_CASE("layers page: an element below its minimum overflows, without clipping", "[layers_page]")
{
   test_view tv{extent{200, 200}};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif

   // A layer 50 x 50 at the top-left; its element is at least 100 x 100.
   tv.view_.content(
      align_left_top(fixed_size({50, 50},
         layer(min_size({100, 100}, box(colors::red)))
      )),
      box(colors::white)
   );
   tv.draw();

#if !defined(ARTIST_RECORDING)
   auto in = pixel_at(tv.img, 25, 25);
   CHECK(in.r > 200);
   CHECK(in.g < 50);
   auto past = pixel_at(tv.img, 75, 75);     // Past the right and bottom edges.
   CHECK(past.r > 200);
   CHECK(past.g < 50);
   auto beyond = pixel_at(tv.img, 150, 150);
   CHECK(beyond.g > 200);
#endif
}

TEST_CASE("layers page: a composite layer is used like a std::vector and shared like any element", "[layers_page]")
{
   test_view tv{extent{400, 400}};
   probe a, b;

   layer_composite c;
   c.push_back(share(link(a)));
   c.push_back(share(link(b)));
   CHECK(c.size() == 2);
   std::vector<probe*> order;
   a.order = b.order = &order;
   lay_out(tv, c, rect{0, 0, 100, 50});
   a.order = b.order = nullptr;
   REQUIRE(order.size() == 2);
   CHECK(order[1] == &b);                   // The last element is the top-most.
   CHECK(a.drawn == rect{0, 0, 100, 50});
   CHECK(b.drawn == rect{0, 0, 100, 50});

   // Shared into another layer.
   auto lc = std::make_shared<layer_composite>();
   lc->push_back(share(link(a)));
   probe under;
   auto l = layer(hold(lc), link(under));
   lay_out(tv, l, rect{10, 10, 60, 60});
   CHECK(a.drawn == rect{10, 10, 60, 60});
   CHECK(under.drawn == rect{10, 10, 60, 60});
}

TEST_CASE("layers page: the example", "[layers_page]")
{
   test_view tv{extent{200, 150}};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif

   layer_composite badges;
   badges.push_back(share(
      align_right_top(fixed_size({20, 20}, box(colors::red)))
   ));

   auto card = layer(
      badges,
      align_center_middle(fixed_size({100, 60}, box(colors::gold))),
      box(colors::slate_gray)
   );

   tv.view_.content(card);
   tv.draw();

   // The badge is at 180 to 200, 0 to 20; the gold box at 50 to 150, 45 to
   // 105; the slate gray background fills the rest.
#if !defined(ARTIST_RECORDING)
   auto badge = pixel_at(tv.img, 190, 10);
   CHECK(badge.r > 200);
   CHECK(badge.g < 50);
   auto gold = pixel_at(tv.img, 100, 75);
   CHECK(gold.r > 200);
   CHECK(gold.b < 50);
   auto ground = pixel_at(tv.img, 20, 130);
   CHECK(ground.r > 100);
   CHECK(ground.r < 125);
   CHECK(ground.b > 130);
   CHECK(ground.b < 155);
#endif
}
