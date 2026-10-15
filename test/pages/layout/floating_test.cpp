/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/layout/floating.adoc.
=============================================================================*/
#include "test_support.hpp"
#include <cstdint>
#include <type_traits>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   // An element from 40 by 20 up to 100 by 60 that records the bounds it
   // is laid out and drawn with.
   struct probe : element
   {
      view_limits limits(basic_context const&) const override
      {
         return {{40, 20}, {100, 60}};
      }

      void layout(context const& ctx) override
      {
         laid_out = ctx.bounds;
      }

      void draw(context const& ctx) override
      {
         drawn = ctx.bounds;
      }

      rect laid_out = {};
      rect drawn = {};
   };

   // The floating element is given `parent` by its container.
   template <typename E>
   void lay_out(test_view& tv, E& e, rect parent)
   {
      context ctx{tv.view_, tv.cnv, &e, parent};
      e.limits(ctx);
      e.layout(ctx);
   }

   template <typename E>
   void draw(test_view& tv, E& e, rect parent)
   {
      context ctx{tv.view_, tv.cnv, &e, parent};
      e.draw(ctx);
   }

   template <typename E>
   view_limits limits_of(test_view& tv, E& e)
   {
      basic_context ctx{tv.view_, tv.cnv};
      return e.limits(ctx);
   }

   // The red of the pixel at (x, y); pixels() is premultiplied B, G, R, A.
   int red_at(cycfi::artist::image const& img, int x, int y)
   {
      auto p = reinterpret_cast<std::uint8_t const*>(img.pixels());
      return p[4 * (y * int(img.bitmap_size().x) + x) + 2];
   }

   // The subject's bounds are within its natural limits.
   bool within_limits(rect r)
   {
      return r.width() >= 40 && r.width() <= 100
         && r.height() >= 20 && r.height() <= 60;
   }
}

TEST_CASE("floating page: the factory", "[floating_page]")
{
   auto e = floating({10, 20, 90, 60}, probe{});
   CHECK(std::is_same_v<decltype(e), proxy<probe, floating_element>>);
   CHECK(std::is_base_of_v<proxy_base, floating_element>);
   CHECK(e.bounds() == rect{10, 20, 90, 60});
}

TEST_CASE("floating page: placed exactly at the specified position", "[floating_page]")
{
   test_view tv{extent{400, 400}};
   probe p;
   auto e = floating({10, 20, 90, 60}, link(p));

   // Whatever bounds its container gives it.
   lay_out(tv, e, rect{0, 0, 400, 400});
   CHECK(p.laid_out == rect{10, 20, 90, 60});
   draw(tv, e, rect{200, 200, 300, 300});
   CHECK(p.drawn == rect{10, 20, 90, 60});
}

TEST_CASE("floating page: constrained to the subject's natural limits", "[floating_page]")
{
   test_view tv{extent{400, 400}};

   SECTION("too big: the size shrinks to the maximum, from the top left")
   {
      probe p;
      auto e = floating({10, 20, 210, 220}, link(p));
      lay_out(tv, e, rect{0, 0, 400, 400});
      CHECK(p.laid_out == rect{10, 20, 110, 80});
      CHECK(within_limits(p.laid_out));
   }

   SECTION("too small: the size grows to the minimum, from the top left")
   {
      probe p;
      auto e = floating({10, 20, 15, 25}, link(p));
      lay_out(tv, e, rect{0, 0, 400, 400});
      CHECK(p.laid_out == rect{10, 20, 50, 40});
      CHECK(within_limits(p.laid_out));
   }
}

TEST_CASE("floating page: the natural limits of the subject are not violated", "[floating_page]")
{
   test_view tv{extent{400, 400}};
   probe p;
   auto e = floating({0, 0, 400, 400}, link(p));

   // The floating element takes the subject's minimum and leaves the
   // maximum open; the subject itself stays within its own limits.
   auto l = limits_of(tv, e);
   CHECK(l.min == point{40, 20});
   CHECK(l.max == point{full_extent, full_extent});

   draw(tv, e, rect{0, 0, 400, 400});
   CHECK(within_limits(p.drawn));
}

TEST_CASE("floating page: the mutator sets the bounds", "[floating_page]")
{
   test_view tv{extent{400, 400}};
   probe p;
   auto e = floating({10, 20, 90, 60}, link(p));
   lay_out(tv, e, rect{0, 0, 400, 400});

   e.bounds({100, 120, 160, 160});
   CHECK(e.bounds() == rect{100, 120, 160, 160});

   // Placed there the next time it is laid out or drawn.
   CHECK(p.laid_out == rect{10, 20, 90, 60});
   draw(tv, e, rect{0, 0, 400, 400});
   CHECK(p.drawn == rect{100, 120, 160, 160});

   // Constrained to the subject's natural limits.
   e.bounds({100, 120, 400, 400});
   lay_out(tv, e, rect{0, 0, 400, 400});
   CHECK(p.laid_out == rect{100, 120, 200, 180});
}

TEST_CASE("floating page: the accessor gets the constrained bounds", "[floating_page]")
{
   test_view tv{extent{400, 400}};
   probe p;
   auto e = floating({10, 20, 210, 220}, link(p));
   CHECK(e.bounds() == rect{10, 20, 210, 220});

   lay_out(tv, e, rect{0, 0, 400, 400});
   CHECK(e.bounds() == rect{10, 20, 110, 80});

   e.bounds({10, 20, 15, 25});
   draw(tv, e, rect{0, 0, 400, 400});
   CHECK(e.bounds() == rect{10, 20, 50, 40});
}

TEST_CASE("floating page: the example", "[floating_page]")
{
   test_view tv{extent{400, 300}};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   auto fl = share(floating({50, 50, 250, 150}, box(colors::gold)));
   tv.view_.content(
      hold(fl),
      box(colors::dark_slate_gray)
   );
   tv.draw();
   CHECK(fl->bounds() == rect{50, 50, 250, 150});

#if !defined(ARTIST_RECORDING)
   // Gold (red 255) inside the bounds, dark slate gray (red 47) outside.
   CHECK(red_at(tv.img, 60, 60) > 200);
   CHECK(red_at(tv.img, 240, 140) > 200);
   CHECK(red_at(tv.img, 40, 40) < 100);
   CHECK(red_at(tv.img, 260, 100) < 100);
   CHECK(red_at(tv.img, 100, 160) < 100);
#endif
}
