/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/support/draw_utils.adoc
   and draws its figure: each drawing utility, on a canvas, with the
   arguments the page gives. The figure lands in the build's results
   directory; copy it to docs/modules/ROOT/images/support/ to update the
   page.
=============================================================================*/
#include "test_support.hpp"
#include <cstdint>
#include <string>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   auto constexpr bkd_color = rgba(35, 35, 37, 255);

   // The page's gallery: one cell per utility, drawn straight on the canvas.
   void gallery(canvas& cnv)
   {
      auto const body = rgba(88, 118, 153, 255);      // a control's body color
      auto const ind = colors::lawn_green;     // its indicator color

      draw_panel(cnv, {20, 20, 140, 90}, rgba(55, 55, 60, 255), 4);
      draw_box_vgradient(cnv, {160, 20, 280, 90});
      draw_button(cnv, {300, 35, 420, 75}, body, true, {4, 4, 4, 4});
      draw_button(cnv, {440, 35, 560, 75}, body, false, {4, 4, 4, 4});

      draw_track(cnv, {20, 140, 140, 146});
      draw_thumb(cnv, {90, 143, 14}, body, ind);
      draw_rect_thumb(cnv, {160, 128, 190, 158}, 4, body, ind);
      draw_tri_thumb(cnv, {210, 128, 240, 158}, direction::up, body, ind);
      draw_indicator(cnv, {260, 138, 290, 148}, ind);

      cnv.fill_style(colors::dim_gray);
      draw_round_rect(cnv, {300, 120, 420, 165}, {0, 12, 0, 12});
      cnv.fill();

      draw_knob(cnv, {90, 250, 40}, body);
      draw_radial_indicator(cnv, {90, 250, 40}, 0.6f, ind);
      draw_radial_marks(cnv, {240, 250, 40}, 8, colors::light_gray);
      std::string const labels[] = {"0", "25", "50", "75", "100"};
      draw_radial_labels(cnv, {400, 250, 52}, 0.8f, labels, 5);
   }
}

namespace
{
   // The page's example.
   struct big_thumb : element
   {
      view_limits limits(basic_context const&) const override
      {
         return {{32, 32}, {32, 32}};
      }

      void draw(context const& ctx) override
      {
         auto const& thm = get_theme();
         draw_thumb(ctx.canvas, circle{center_point(ctx.bounds), 16}
          , thm.default_button_color, thm.indicator_color);
      }
   };
}

TEST_CASE("draw_utils page: example", "[draw_utils_page]")
{
   test_view tv{extent{200, 60}, 1};
   auto s = share(slider(big_thumb{}, basic_track<5, false>(), 0.5));
   tv.view_.content(align_middle(hold(s)));
   tv.view_.layout();
   tv.draw();
   CHECK(s->value() == 0.5);
}

TEST_CASE("draw_utils page: corner_radii arithmetic", "[draw_utils_page]")
{
   corner_radii r{4, 4, 8, 8};
   auto s = r + 1;
   CHECK(s.top_left == 5);
   CHECK(s.bottom_right == 9);
   auto t = r - 1;
   CHECK(t.top_left == 3);
   CHECK(t.bottom_left == 7);
}

TEST_CASE("draw_utils page: radial_consts", "[draw_utils_page]")
{
   // A dial travels 83% of the circle, leaving a symmetric gap at the
   // bottom: the start angle is half the gap, past the bottom.
   CHECK(radial_consts::travel == 0.83);
   CHECK(radial_consts::range == Approx(2 * cycfi::pi * 0.83));
   CHECK(radial_consts::start_angle == Approx(2 * cycfi::pi * 0.085));
   CHECK(radial_consts::offset == Approx(radial_consts::start_angle));
}

TEST_CASE("draw_utils page: figure", "[draw_utils_page]")
{
   test_view tv{extent{580, 320}, 2};
   tv.cnv.fill_style(bkd_color);
   tv.cnv.add_rect({0, 0, 580, 320});
   tv.cnv.fill();
   gallery(tv.cnv);
   tv.img.save_png(std::string{RESULTS_PATH} + "draw_utils_gallery.png");

#if !defined(ARTIST_RECORDING)
   // draw_round_rect only builds the path: nothing is drawn until the
   // caller fills or strokes it. The page's claim, checked on the pixels
   // of an untouched spot inside a path that was built and left.
   test_view probe{extent{100, 100}, 1};
   probe.cnv.fill_style(bkd_color);
   probe.cnv.add_rect({0, 0, 100, 100});
   probe.cnv.fill();
   probe.cnv.fill_style(colors::white);
   draw_round_rect(probe.cnv, {10, 10, 90, 90}, {8, 8, 8, 8});
   auto red = [&]
   {
      // pixels() is premultiplied B, G, R, A on every backend.
      auto p = reinterpret_cast<std::uint8_t const*>(probe.img.pixels());
      return int(p[4 * (50 * int(probe.img.bitmap_size().x) + 50) + 2]);
   };
   CHECK(red() < 60);
   probe.cnv.fill();
   CHECK(red() > 240);
#endif
}
