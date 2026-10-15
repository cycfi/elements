/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/layout/decks.adoc.
=============================================================================*/
#include "test_support.hpp"
#include <cstdint>
#include <type_traits>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   // An element of the given limits that records the bounds it is drawn
   // with, and how often it is drawn.
   struct probe : element
   {
      probe(view_limits l = full_limits, bool control = true)
       : lim(l), control(control) {}

      view_limits limits(basic_context const&) const override { return lim; }
      bool wants_control() const override { return control; }

      void draw(context const& ctx) override
      {
         drawn = ctx.bounds;
         ++draws;
      }

      view_limits lim;
      bool control;
      rect drawn = {};
      int draws = 0;
   };

   template <typename Deck>
   void lay_out(test_view& tv, Deck& d, rect bounds)
   {
      context ctx{tv.view_, tv.cnv, &d, bounds};
      d.layout(ctx);
      d.draw(ctx);
   }

   template <typename Deck>
   element* hit(test_view& tv, Deck& d, rect bounds, point p)
   {
      context ctx{tv.view_, tv.cnv, &d, bounds};
      return d.hit_test(ctx, p, true, true);
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

TEST_CASE("decks page: the factory and the composite", "[decks_page]")
{
   probe a, b, c;

   auto d = deck(link(a), link(b), link(c));
   CHECK(std::is_same_v<decltype(d), array_composite<3, deck_element>>);
   CHECK(std::is_same_v<decltype(d)::container_type, std::array<element_ptr, 3>>);
   CHECK(std::is_same_v<deck_composite, vector_composite<deck_element>>);
   CHECK(std::is_base_of_v<layer_element, deck_element>);
   CHECK(d.size() == 3);
}

TEST_CASE("decks page: select and selected", "[decks_page]")
{
   probe a, b, c;
   auto d = deck(link(a), link(b), link(c));

   CHECK(d.selected() == 0);                 // 0 by default.
   d.select(2);
   CHECK(d.selected() == 2);
   d.select(3);                              // Past the last element: ignored.
   CHECK(d.selected() == 2);
   d.select(0);
   CHECK(d.selected() == 0);
}

TEST_CASE("decks page: only the selected element is drawn", "[decks_page]")
{
   test_view tv{extent{400, 400}};
   probe a, b, c;
   auto d = deck(link(a), link(b), link(c));

   lay_out(tv, d, rect{20, 30, 120, 80});
   CHECK(a.draws == 1);
   CHECK(b.draws == 0);
   CHECK(c.draws == 0);
   CHECK(a.drawn == rect{20, 30, 120, 80});

   d.select(1);
   lay_out(tv, d, rect{20, 30, 120, 80});
   CHECK(a.draws == 1);
   CHECK(b.draws == 1);
   CHECK(c.draws == 0);
   CHECK(b.drawn == rect{20, 30, 120, 80});
}

TEST_CASE("decks page: only the selected element is given UI control", "[decks_page]")
{
   test_view tv{extent{400, 400}};
   rect const bounds = {0, 0, 100, 100};
   probe a, b;
   auto d = deck(link(a), link(b));

   CHECK(hit(tv, d, bounds, {50, 50}) == &a);
   d.select(1);
   CHECK(hit(tv, d, bounds, {50, 50}) == &b);

   SECTION("a selected element that takes no control passes nothing down")
   {
      probe top{full_limits, false}, bottom{full_limits, true};
      auto d2 = deck(link(top), link(bottom));
      CHECK(hit(tv, d2, bounds, {50, 50}) == nullptr);
   }
}

TEST_CASE("decks page: the limits and layout of the layers apply", "[decks_page]")
{
   test_view tv{extent{400, 400}};

   SECTION("the maximum of the minimums, the minimum of the maximums")
   {
      probe a{{{10, 30}, {100, 60}}}, b{{{20, 20}, {80, 90}}};
      auto d = deck(link(a), link(b));
      auto lim = limits_of(tv, d);
      CHECK(lim.min.x == 20);
      CHECK(lim.min.y == 30);
      CHECK(lim.max.x == 80);
      CHECK(lim.max.y == 60);
   }

   SECTION("the maximum is not less than the minimum")
   {
      probe a{{{50, 50}, {200, 200}}}, b{{{0, 0}, {20, 30}}};
      auto d = deck(link(a), link(b));
      auto lim = limits_of(tv, d);
      CHECK(lim.max.x == 50);
      CHECK(lim.max.y == 50);
   }

   SECTION("an element past its maximum is aligned to the top-left")
   {
      probe a{{{0, 0}, {40, 30}}}, b;
      auto d = deck(link(a), link(b));
      lay_out(tv, d, rect{20, 30, 120, 80});
      CHECK(a.drawn == rect{20, 30, 60, 60});
   }
}

TEST_CASE("decks page: a composite deck is used like a std::vector and shared like any element", "[decks_page]")
{
   test_view tv{extent{400, 400}};
   probe a, b;

   deck_composite c;
   c.push_back(share(link(a)));
   c.push_back(share(link(b)));
   CHECK(c.size() == 2);
   CHECK(c.selected() == 0);
   c.select(1);
   lay_out(tv, c, rect{0, 0, 100, 50});
   CHECK(a.draws == 0);
   CHECK(b.draws == 1);
   CHECK(b.drawn == rect{0, 0, 100, 50});

   // Shared into another deck.
   auto dc = std::make_shared<deck_composite>();
   dc->push_back(share(link(a)));
   probe other;
   auto d = deck(hold(dc), link(other));
   lay_out(tv, d, rect{10, 10, 60, 60});
   CHECK(a.draws == 1);
   CHECK(a.drawn == rect{10, 10, 60, 60});
   CHECK(other.draws == 0);
}

TEST_CASE("decks page: the example", "[decks_page]")
{
   test_view tv{extent{200, 150}};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   auto& view_ = tv.view_;

   auto pages = share(deck(
      box(colors::gold),
      box(colors::slate_gray)
   ));

   auto flip = button("Flip");
   flip.on_click =
      [&view_, pages](bool)
      {
         pages->select(1 - pages->selected());
         view_.refresh(*pages);
      };

   auto on_click = flip.on_click;
   auto content = vtile(
      fixed_size({200, 100}, hold(pages)),
      flip
   );

   view_.content(align_left_top(content), box(colors::white));
   tv.draw();

   // The gold box is drawn first; after a flip, the slate gray one.
   auto gold = pixel_at(tv.img, 100, 50);
   CHECK(gold.r > 200);
   CHECK(gold.b < 50);

   on_click(true);
   CHECK(pages->selected() == 1);
   tv.draw();
   auto gray = pixel_at(tv.img, 100, 50);
   CHECK(gray.r > 100);
   CHECK(gray.r < 125);
   CHECK(gray.b > 130);
   CHECK(gray.b < 155);

   on_click(true);
   CHECK(pages->selected() == 0);
}

namespace
{
   // An element that takes the focus and records the keys, other than Tab,
   // and the text it is given.
   struct key_probe : element
   {
      bool wants_control() const override { return true; }
      bool wants_focus() const override { return true; }
      void begin_focus(focus_request) override { focused = true; }
      bool end_focus() override { focused = false; return true; }
      bool key(context const&, key_info k) override
      {
         if (k.key == key_code::tab)                 // Tab moves on.
            return false;
         ++keys;
         return true;
      }
      bool text(context const&, text_info) override { ++texts; return true; }

      bool focused = false;
      int keys = 0;
      int texts = 0;
   };

   key_info const tab_key = {key_code::tab, key_action::press, 0};
   key_info const a_key = {key_code::a, key_action::press, 0};
}

TEST_CASE("decks page: only the selected element takes keys and text", "[decks_page]")
{
   test_view tv{extent{400, 400}};
   key_probe a, b;
   auto d = deck(link(a), link(b));
   context ctx{tv.view_, tv.cnv, &d, rect{0, 0, 100, 100}};

   d.begin_focus(element::from_top);
   CHECK(a.focused);
   CHECK(!b.focused);

   d.select(1);                              // The newly selected page takes the focus.
   CHECK(!a.focused);
   CHECK(b.focused);
   d.key(ctx, a_key);
   d.text(ctx, text_info{'x', 0});
   CHECK(a.keys == 0);
   CHECK(a.texts == 0);
   CHECK(b.keys == 1);
   CHECK(b.texts == 1);
}

TEST_CASE("decks page: Tab never focuses a hidden element", "[decks_page]")
{
   test_view tv{extent{400, 400}};
   key_probe a, b;
   auto d = deck(link(a), link(b));
   context ctx{tv.view_, tv.cnv, &d, rect{0, 0, 100, 100}};

   CHECK(d.key(ctx, tab_key));               // Into the selected page.
   CHECK(a.focused);
   CHECK(!b.focused);
   CHECK(!d.key(ctx, tab_key));              // Out of the deck, not to page 1.
   CHECK(!b.focused);

   SECTION("a hidden element that wants the focus is not counted")
   {
      probe plain;
      auto d2 = deck(link(plain), link(b));
      CHECK(!d2.wants_focus());
      context ctx2{tv.view_, tv.cnv, &d2, rect{0, 0, 100, 100}};
      CHECK(!d2.key(ctx2, tab_key));
      CHECK(!b.focused);
   }
}

TEST_CASE("decks page: an empty or shrunk deck_composite is safe", "[decks_page]")
{
   test_view tv{extent{400, 400}};
   rect const bounds = {0, 0, 100, 100};

   SECTION("empty")
   {
      deck_composite c;
      lay_out(tv, c, bounds);
      CHECK(hit(tv, c, bounds, {50, 50}) == nullptr);
      context ctx{tv.view_, tv.cnv, &c, bounds};
      CHECK(!c.key(ctx, tab_key));
      CHECK(!c.wants_focus());
   }

   SECTION("shrunk below the selected index")
   {
      probe a, b, x;
      deck_composite c;
      c.push_back(share(link(a)));
      c.push_back(share(link(b)));
      c.push_back(share(link(x)));
      c.select(2);
      c.pop_back();
      c.pop_back();
      lay_out(tv, c, bounds);
      CHECK(a.draws == 0);
      CHECK(hit(tv, c, bounds, {50, 50}) == nullptr);
      context ctx{tv.view_, tv.cnv, &c, bounds};
      CHECK(!c.key(ctx, a_key));
   }
}
