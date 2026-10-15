/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/styles/tracks.adoc and
   draws its figures. The figures land in the build's results directory;
   copy them to docs/modules/ROOT/images/styles/ to update the page.
=============================================================================*/
#include "test_support.hpp"
#include <elements/element/style/track.hpp>
#include <elements/element/style/status_bar.hpp>
#include <elements/element/style/slider.hpp>
#include <elements/element/style/dial.hpp>
#include <string>
#include <type_traits>
#include <utility>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   auto constexpr bkd_color = rgba(35, 35, 37, 255);

   void open(test_view& tv)
   {
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
      headless::open(tv.view_);
#endif
   }

   // The smallest size of an element
   template <typename E>
   extent min_size(E const& e)
   {
      test_view m{extent{10, 10}};
      basic_context bctx{m.view_, m.cnv};
      auto lim = e.limits(bctx);
      return {lim.min.x, lim.min.y};
   }

   // Draw content in a view of its own size, with margins that leave 20
   // around what is drawn
   template <typename E>
   void save_figure(E const& content, rect margins, std::string const& name)
   {
      auto fig = margin(margins, content);
      test_view tv{min_size(fig), 2};
      open(tv);
      tv.view_.content(fig, box(bkd_color));
      tv.view_.layout();
      tv.draw();
      tv.img.save_png(std::string{RESULTS_PATH} + name);
   }
}

TEST_CASE("tracks page: costs", "[tracks_page]")
{
   // Defaults hold no color.
   static_assert(sizeof(basic_track_styler<5>) == sizeof(element));
   static_assert(sizeof(basic_fill_track_styler<5>) == sizeof(element));
   static_assert(sizeof(decltype(basic_fill_track<5>().glow())) == sizeof(element));

   // A color given is held; each setter keeps the other's.
   auto t = basic_fill_track<5>().track_color(colors::red).fill_color(colors::green);
   CHECK(t.track_color() == colors::red);
   CHECK(t.fill_color() == colors::green);
   auto u = basic_fill_track<5>().fill_color(colors::green);
   CHECK(u.track_color() == colors::black);
   CHECK(u.fill_color() == colors::green);

   test_view tv{extent{200, 200}};
   basic_context bctx{tv.view_, tv.cnv};
   CHECK(basic_fill_track<5>().limits(bctx).min == point{64, 5});
   CHECK(basic_fill_track<5, true>().limits(bctx).min == point{5, 64});
}

TEST_CASE("tracks page: example", "[tracks_page]")
{
   auto level = slider(
      basic_thumb<20>(),
      basic_fill_track<5>()
         .track_color(rgba(60, 60, 64, 255))
         .fill_color(colors::light_sea_green),
      0.8
   );
   CHECK(level.value() == 0.8);
}

TEST_CASE("tracks page: status bars", "[tracks_page]")
{
   auto p = progress_bar<6>(0.45);
   CHECK(p.value() == 0.45);
   auto v = vu_meter<6>();
   v.value(0.5);
   CHECK(v.value() == 0.5);

   // The page's snippet.
   auto meter = progress_bar(
      basic_track<6, false, track_shape::square>()
    , basic_fill_track<6, false, track_shape::square>().fill_colors({
         {0.8f, colors::light_sea_green}
       , {1.0f, colors::orange}
      })
   );
   CHECK(meter.value() == 0.0);
}

#if !defined(ARTIST_RECORDING)
TEST_CASE("tracks page: fill track figure", "[tracks_page]")
{
   auto sliders =
            hsize(340,
               vtile_spaced(28.0,
                  slider(basic_thumb<20>(), basic_fill_track<5>(), 0.6),
                  range_slider(
                     basic_thumb<20>(), basic_thumb<20>(),
                     basic_fill_track<5>(), {0.25, 0.7}),
                  slider(
                     basic_thumb<20>().indicator_color(colors::orange),
                     basic_fill_track<5>().fill_color(colors::orange.level(0.8)),
                     0.4),
                  slider(
                     basic_thumb<20>(),
                     basic_fill_track<5>()
                        .track_color(rgba(60, 60, 64, 255))
                        .fill_color(colors::light_sea_green),
                     0.8),
                  slider(
                     basic_thumb<20>(),
                     basic_fill_track<5>().fill_gradient({
                        {0.0f, colors::light_sea_green}
                      , {1.0f, colors::medium_orchid}
                     }),
                     0.9)
               )
            );
   auto h = min_size(sliders).y;
   save_figure(
      htile(
         sliders,
         margin_left(40, vsize(h, slider(basic_thumb<20>(), basic_fill_track<5, true>(), 0.7)))
      ),
      {12.5, 20, 20, 20},  // The track starts half a thumb in
      "tracks_fill.png"
   );
}
#endif

TEST_CASE("tracks page: gradient", "[tracks_page]")
{
   auto ft = basic_fill_track<5>().fill_gradient({
      {0.0f, colors::black}
    , {1.0f, colors::white}
   });
   CHECK(ft.fill_color() == colors::white);
   gradient_fill<2> g{{{{0.0f, colors::black}, {1.0f, colors::white}}}};
   CHECK(g.at(-1.0f) == colors::black);
   CHECK(g.at(0.5f).red == Approx(0.5f));
   CHECK(g.at(2.0f) == colors::white);
}

TEST_CASE("tracks page: glow", "[tracks_page]")
{
   // A glow takes room at both sides of the track.
   test_view tv{extent{200, 100}};
   basic_context bctx{tv.view_, tv.cnv};
   CHECK(basic_fill_track<6>().glow().limits(bctx).min == point{64, 18});
   CHECK(basic_fill_track<6, true>().glow().limits(bctx).min == point{18, 64});
}

namespace
{
   template <bool vertical>
   auto glowing_vu_meter(double init)
   {
      return progress_bar(
         basic_track<8, vertical, track_shape::square>()
       , basic_fill_track<8, vertical, track_shape::square>().glow().fill_colors({
            {0.7f, rgba(46, 204, 64, 255)}
          , {0.9f, rgba(255, 210, 0, 255)}
          , {1.0f, rgba(230, 40, 40, 255)}
         })
       , init
      );
   }
}

#if !defined(ARTIST_RECORDING)
TEST_CASE("tracks page: status bars figure", "[tracks_page]")
{
   auto busy = share(busy_bar<6>(0.55, 0.3));
   auto bars =
            hsize(340,
               vtile_spaced(24.0,
                  progress_bar<6>(0.45),
                  progress_bar(
                     basic_track<6>(),
                     basic_fill_track<6>(),
                     0.7),
                  progress_bar(
                     basic_track<6>(),
                     basic_fill_track<6>().glow().fill_color(colors::orange),
                     0.3),
                  hold(busy),
                  vu_meter<6>(0.6),
                  vu_meter<6>(0.95),
                  glowing_vu_meter<false>(0.95),
                  progress_bar(
                     basic_track<8, false, track_shape::square>(),
                     basic_fill_track<8, false, track_shape::square>().glow().fill_gradient({
                        {0.0f, rgba(46, 204, 64, 255)}
                      , {0.7f, rgba(255, 210, 0, 255)}
                      , {1.0f, rgba(230, 40, 40, 255)}
                     }),
                     0.9)
               )
            );
   auto h = min_size(bars).y;
   save_figure(
      htile(
         bars,
         margin_left(30, vsize(h, htile(
            glowing_vu_meter<true>(0.5),
            glowing_vu_meter<true>(0.8),
            glowing_vu_meter<true>(0.97)
         )))
      ),
      {23, 20, 18, 20},    // Round ends go past the bounds; a glow is inside
      "tracks_status.png"
   );
}
#endif

TEST_CASE("tracks page: radial fill track", "[tracks_page]")
{
   // Defaults hold no color.
   using knob = basic_knob_styler<50>;
   static_assert(sizeof(decltype(radial_fill_track<20>(knob{}))) == sizeof(proxy<knob>));
   static_assert(sizeof(decltype(radial_fill_track<20>(knob{}).glow())) == sizeof(proxy<knob>));

   auto t = radial_fill_track<20>(basic_knob<50>())
      .track_color(colors::red)
      .fill_color(colors::green);
   CHECK(t.track_color() == colors::red);
   CHECK(t.fill_color() == colors::green);

   // The ring adds size around the subject.
   test_view tv{extent{200, 200}};
   basic_context bctx{tv.view_, tv.cnv};
   CHECK(t.limits(bctx).min == point{70, 70});

   // The page's example.
   auto gain = dial(
      radial_fill_track<20, track_shape::square>(basic_knob<50>())
         .glow()
         .fill_gradient({
            {0.0f, colors::light_sea_green}
          , {1.0f, colors::medium_orchid}
         }),
      0.8
   );
   CHECK(gain.value() == 0.8);
}

#if !defined(ARTIST_RECORDING)
TEST_CASE("tracks page: radial fill track figure", "[tracks_page]")
{
   auto meter_bands = [](auto&& t)
   {
      return t.fill_colors({
         {0.7f, rgba(46, 204, 64, 255)}
       , {0.9f, rgba(255, 210, 0, 255)}
       , {1.0f, rgba(230, 40, 40, 255)}
      });
   };

   save_figure(
      htile_spaced(30,
         dial(radial_fill_track<20>(basic_knob<50>()), 0.6),
         dial(radial_fill_track<20>(basic_knob<50>()).fill_color(colors::orange.level(0.8)), 0.35),
         dial(radial_fill_track<20, track_shape::square>(basic_knob<50>()).glow().fill_gradient({
            {0.0f, colors::light_sea_green}
          , {1.0f, colors::medium_orchid}
         }), 0.8),
         dial(meter_bands(radial_fill_track<20, track_shape::square>(basic_knob<50>()).glow()), 0.95),
         dial(radial_marks<20>(radial_fill_track<20>(basic_knob<50>())), 0.5)
      ),
      {20, 20, 20, 20},
      "tracks_radial.png"
   );
}
#endif

TEST_CASE("tracks page: radial fill track from the center", "[tracks_page]")
{
   using knob = basic_knob_styler<50>;
   using pan_type = decltype(radial_fill_track<20, track_shape::round, track_origin::center>(knob{}));
   static_assert(sizeof(pan_type) == sizeof(proxy<knob>));
   static_assert(std::is_same_v<
      decltype(std::declval<pan_type>().glow().fill_color(colors::red))
    , radial_fill_track_styler<
         20, knob, fill_track_colors<default_track_color, fixed_color>
       , track_shape::round, true, track_origin::center>
   >);
}

#if !defined(ARTIST_RECORDING)
TEST_CASE("tracks page: pan figure", "[tracks_page]")
{
   auto pan = [](double v)
   {
      return dial(
         radial_fill_track<20, track_shape::round, track_origin::center>(basic_knob<50>()),
         v);
   };

   save_figure(
      htile_spaced(30,
         pan(0.25),
         pan(0.5),
         pan(0.8),
         dial(
            radial_fill_track<20, track_shape::square, track_origin::center>(basic_knob<50>())
               .glow()
               .fill_gradient({
                  {0.0f, colors::light_sea_green}
                , {0.5f, colors::white}
                , {1.0f, colors::medium_orchid}
               }),
            0.15)
      ),
      {20, 20, 20, 20},
      "tracks_pan.png"
   );
}
#endif
