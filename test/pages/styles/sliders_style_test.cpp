/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/styles/sliders.adoc and
   draws its figures. The figures land in the build's results directory;
   copy them to docs/modules/ROOT/images/styles/ to update the page.
=============================================================================*/
#include "test_support.hpp"
#include <chrono>
#include <string>
#include <thread>

using namespace cycfi::elements;
using namespace cycfi::elements::test;
using namespace std::chrono_literals;

namespace
{
   void open(test_view& tv)
   {
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
      headless::open(tv.view_);
#endif
   }

   auto item(std::size_t i)
   {
      return share(fixed_size({40, 20}, element{}));
   }
}

TEST_CASE("sliders style page: probe", "[sliders_style_page]")
{
   test_view tv{extent{300, 300}};
   open(tv);
   basic_context bctx{tv.view_, tv.cnv};

   SECTION("continuous thumbwheels have a size")
   {
      auto v = basic_vthumbwheel(fixed_size({40, 100}, element{}));
      auto h = basic_hthumbwheel(fixed_size({100, 40}, element{}));
      CHECK(v.limits(bctx).min.y > 0);
      CHECK(h.limits(bctx).min.x > 0);
   }
   SECTION("one item shows, either way")
   {
      // Five items of 20 by 20: a quantize of 1/4.
      auto v = basic_vthumbwheel(fixed_size({20, 100}, element{}), 0.25);
      auto h = basic_hthumbwheel(fixed_size({100, 20}, element{}), 0.25);
      CHECK(v.limits(bctx).min.y == 20);
      CHECK(h.limits(bctx).min.x == 20);
   }
   SECTION("a quantized value gets there")
   {
      auto v = share(basic_vthumbwheel(fixed_size({20, 100}, element{}), 0.25));
      tv.view_.content(align_left_top(hold(v)));
      tv.view_.layout();
      tv.draw();
      v->value(0.5);
      for (int i = 0; i != 100; ++i)
      {
         std::this_thread::sleep_for(1ms);
         tv.view_.poll();
         tv.draw();
      }
      CHECK(v->value() == 0.5);
   }
}

namespace
{
   // The sprite dial snippet: compiled, not run, as it reads a file.
   [[maybe_unused]] auto sprite_snippet()
   {
      float const knob_scale = 1.0/3;
      sprite knob = sprite{
         "knob_sprites_white_128x128.png",
         128 * knob_scale, knob_scale
      };
      auto gain = dial(knob, 0.5);
      return gain;
   }
}

TEST_CASE("sliders style page: snippets", "[sliders_style_page]")
{
   test_view tv{extent{300, 300}};
   basic_context bctx{tv.view_, tv.cnv};

   auto hslider = slider(basic_thumb<20>(), basic_track<5>(), 0.3);
   auto vslider = slider(basic_rect_thumb<24, 12>(), basic_track<5, true>(), 0.5);
   CHECK(hslider.value() == 0.3);
   CHECK(vslider.value() == 0.5);

   auto volume = dial(basic_knob<60>(), 0.3);
   CHECK(volume.value() == 0.3);
   CHECK(basic_knob<60>().limits(bctx).min == point{60, 60});

   auto compose = [](std::size_t index)
   {
      return share(align_center_middle(label("Item no. " + std::to_string(index+1))));
   };
   auto wheel = layer(vthumbwheel(20, compose), frame{});
   CHECK(wheel.limits(bctx).min.y > 0);

   // A thumb's indicator: the theme's, held at no cost, or the one given.
   static_assert(sizeof(basic_thumb_styler<20>) < sizeof(basic_thumb_styler<20, fixed_color>));
   static_assert(sizeof(basic_thumb_styler<20>) == sizeof(element) + sizeof(color));
   auto orange = basic_thumb<20>().indicator_color(colors::orange);
   auto rect = basic_rect_thumb<12, 24>().indicator_color(colors::orange);
   auto tri = basic_tri_thumb<16, direction::down>().indicator_color(colors::orange);
   CHECK(orange.limits(bctx).min == point{20, 20});
   CHECK(rect.limits(bctx).min == point{12, 24});
   CHECK(tri.limits(bctx).min == point{16, 16});

   // A track's color: black, held at no cost, or the one given.
   static_assert(sizeof(basic_track_styler<5>) == sizeof(element));
   auto blue = basic_track<5>().track_color(colors::royal_blue);
   CHECK(blue.limits(bctx).min.y == 5);

   // A track is at least 64 long.
   CHECK(basic_track<5>().limits(bctx).min.x == 64);
}

#if !defined(ARTIST_RECORDING)
namespace
{
   auto constexpr bkd_color = rgba(35, 35, 37, 255);

   template <typename E>
   void save(extent size, E&& content, char const* name, int frames = 1)
   {
      test_view tv{size, 2};
      open(tv);
      tv.view_.content(margin({20, 20, 20, 20}, std::forward<E>(content)), box(bkd_color));
      tv.view_.layout();
      for (int i = 0; i != frames; ++i)
         tv.draw();
      tv.img.save_png(std::string{RESULTS_PATH} + name);
   }
}

TEST_CASE("sliders style page: figures", "[sliders_style_page]")
{
   save({500, 190},
      htile(
         hsize(300,
            vtile_spaced(24.0,
               slider(basic_thumb<20>(), basic_track<5>(), 0.3),
               slider(basic_rect_thumb<12, 24>(), basic_track<5>(), 0.5),
               slider(basic_tri_thumb<24, direction::down>(), basic_track<5>(), 0.7),
               slider(basic_thumb<20>().indicator_color(colors::orange), basic_track<5>(), 0.4)
            )
         ),
         margin_left(30, align_middle(dial(basic_knob<60>(), 0.3))),
         margin_left(20, align_middle(dial(basic_knob<40>(), 0.7)))
      ),
      "sliders_style_basic.png");

   std::string const res = std::string{EXAMPLES_PATH} + "sprite_sliders_and_knobs/resources/";
   auto sprite_dial = [&](double v)
   {
      float const knob_scale = 1.0/3;
      sprite knob = sprite{res + "knob_sprites_white_128x128.png", 128 * knob_scale, knob_scale};
      return align_center_middle(dial(knob, v));
   };
   save({360, 110},
      htile(sprite_dial(0.0), sprite_dial(0.5), sprite_dial(1.0)),
      "sliders_style_sprite.png");

   auto compose = [](std::size_t index)
   {
      return share(align_center_middle(label("Item no. " + std::to_string(index+1)).font_size(20)));
   };
   save({300, 120},
      layer(vthumbwheel(20, compose), frame{}),
      "sliders_style_thumbwheel.png");
}
#endif
