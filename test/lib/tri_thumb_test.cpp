/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   The triangular thumb: a pointer for a selector. Its size, that it draws
   where the point says, and that a selector takes it.
=============================================================================*/
#include "test_support.hpp"
#include <cstdint>
#include <string>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   // The red of the pixel at (x, y); pixels() is premultiplied B, G, R, A.
   int red_at(cycfi::artist::image const& img, int x, int y)
   {
      auto p = reinterpret_cast<std::uint8_t const*>(img.pixels());
      return p[4 * (y * int(img.bitmap_size().x) + x) + 2];
   }
}

TEST_CASE("tri thumb: size and direction")
{
   test_view tv{extent{40, 40}, 1};
   basic_context bctx{tv.view_, tv.cnv};

   auto up = basic_tri_thumb<24, direction::up>();
   CHECK(up.limits(bctx).min == point{24, 24});
   CHECK(up.limits(bctx).max == point{24, 24});
   CHECK(decltype(up)::dir == direction::up);

   auto left = basic_tri_thumb<24, direction::left>();
   CHECK(decltype(left)::dir == direction::left);
}

#if !defined(ARTIST_RECORDING)

TEST_CASE("tri thumb: the point is where the direction says")
{
   // A 40 by 40 gold triangle drawn straight on the canvas, pointing each
   // way: the pixel near the point is gold, the two corners it leaves
   // empty are background.
   auto check = [](direction dir, point near_tip, point empty1, point empty2)
   {
      test_view tv{extent{40, 40}, 1};
      tv.cnv.fill_style(colors::black);
      tv.cnv.add_rect({0, 0, 40, 40});
      tv.cnv.fill();
      draw_tri_thumb(tv.cnv, {0, 0, 40, 40}, dir, colors::gold, colors::black);
      CHECK(red_at(tv.img, int(near_tip.x), int(near_tip.y)) > 150);
      CHECK(red_at(tv.img, int(empty1.x), int(empty1.y)) < 60);
      CHECK(red_at(tv.img, int(empty2.x), int(empty2.y)) < 60);
   };
   check(direction::up,    {20, 8},  {2, 2},   {37, 2});
   check(direction::down,  {20, 31}, {2, 37},  {37, 37});
   check(direction::left,  {8, 20},  {2, 2},   {2, 37});
   check(direction::right, {31, 20}, {37, 2},  {37, 37});
}

#endif

TEST_CASE("tri thumb: on a selector")
{
   test_view tv{extent{220, 60}, 1};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   auto sel = share(selector<4>(basic_tri_thumb<20, direction::down>(),
                                basic_track<5, false>(), 0.0));
   tv.view_.content(align_middle(hold(sel)));
   tv.view_.layout();
   tv.draw();
   sel->value(0.67);
   CHECK(sel->value() == Approx(2.0 / 3));
}
