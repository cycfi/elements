/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   A toggle icon button with two icons and two colours: each state shows
   its own icon on its own colour, at full strength.
=============================================================================*/
#include "test_support.hpp"
#include <cstdint>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

// The cases read the pixels a drawing left. The recording backend keeps a
// journal of what was drawn and rasterizes nothing.
#if !defined(ARTIST_RECORDING)

namespace
{
   // pixels() is premultiplied B, G, R, A on every backend.
   struct rgba8 { int r, g, b, a; };

   rgba8 pixel_at(cycfi::artist::image const& img, int x, int y)
   {
      auto p = reinterpret_cast<std::uint8_t const*>(img.pixels());
      p += 4 * (y * int(img.bitmap_size().x) + x);
      return {p[2], p[1], p[0], p[3]};
   }

   auto constexpr green = rgba(46, 160, 67, 255);
   auto constexpr red = rgba(200, 40, 40, 255);

   // The body's colour, read beside the icon: a few pixels in from the
   // button's left edge, half way down.
   rgba8 body_of(test_view& tv, element& b)
   {
      rgba8 c{};
      tv.view_.in_context_do(b, [&](context const& ctx)
      {
         c = pixel_at(tv.img
          , int(ctx.bounds.left + 5), int(ctx.bounds.top + ctx.bounds.height() / 2));
      });
      return c;
   }
}

TEST_CASE("toggle_icon_button: off_color while off, on_color while on")
{
   test_view tv{{200, 120}, 1};
   auto b = share(toggle_icon_button(icons::play, icons::pause, 1.5, green, red));
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(align_left_top(hold(b)));
   tv.view_.layout();

   b->value(false);
   tv.draw();
   auto off = body_of(tv, *b);
   CHECK(off.g > off.r + 40);
   CHECK(off.g > off.b + 40);

   b->value(true);
   tv.draw();
   auto on = body_of(tv, *b);
   CHECK(on.r > on.g + 40);
   CHECK(on.r > on.b + 40);
}

TEST_CASE("toggle_icon_button: two colours, both at full strength")
{
   // One colour for both states dims the body; two colours do not. With
   // the same colour as both, the two-colour body is the brighter.
   test_view tv{{200, 120}, 1};
   auto one = share(toggle_icon_button(icons::play, icons::pause, 1.5, green));
   auto two = share(toggle_icon_button(
      icons::play, icons::pause, 1.5, green, green));
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(align_left_top(htile(hold(one), hold(two))));
   tv.view_.layout();
   tv.draw();

   CHECK(body_of(tv, *two).g > body_of(tv, *one).g + 20);
}

#endif
