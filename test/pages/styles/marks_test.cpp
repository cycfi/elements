/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/controls/marks.adoc and
   draws its figures. Each figure case builds the element the page shows,
   lays it out in a view and saves what it drew, so the picture on the page
   and the code on the page cannot drift apart.

   Figures land in the build's results directory. Copy them to
   docs/modules/ROOT/images/controls/ to update the page.
=============================================================================*/
#include "test_support.hpp"
#include <cmath>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   // The background the examples use.
   auto constexpr bkd_color = rgba(35, 35, 37, 255);

   auto caption(char const* text)
   {
      // align_center leaves the column free to stretch: a label's own
      // limits are fixed at its text width, and a vtile takes the
      // narrowest maximum among its children.
      return align_center(
         vmargin(
            {10, 4},
            label(text).font_size(13).font_color(rgba(150, 150, 150, 255))
         )
      );
   }

   // Lay `content` out in a view of the given size, draw it once and save
   // the result. The view is the same one the tests use, so the element
   // goes through the layout and draw it would get in an application.
   template <typename Content>
   void figure(float w, float h, char const* name, Content&& content)
   {
      test_view tv{extent{w, h}, 2};
      tv.view_.content(
         std::forward<Content>(content),
         box(bkd_color)
      );
      tv.draw();
      tv.img.save_png(std::string{RESULTS_PATH} + name);
   }
}

// The cases below read the pixels a drawing left. The recording backend
// keeps a journal of what was drawn and rasterizes nothing, so there are
// no pixels to read there.
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

   // The ink in one column over the three rows centred on y. A rule half a
   // pixel wide lands faintly on two rows, so one row alone says little;
   // the ratio between the channels over the three says which color drew
   // there.
   rgba8 ink_at(cycfi::artist::image const& img, int x, int y)
   {
      rgba8 sum{0, 0, 0, 0};
      for (int i = y - 1; i <= y + 1; ++i)
      {
         auto p = pixel_at(img, x, i);
         sum.r += p.r; sum.g += p.g; sum.b += p.b; sum.a += p.a;
      }
      return sum;
   }

   // Draw `e` over black in a 200 by 200 image and hand back the pixels.
   template <typename Element>
   cycfi::artist::image grid_render(Element&& e)
   {
      test_view tv{extent{200, 200}};
      cycfi::artist::image img{200, 200, 1};
      {
         offscreen_image offscr{img};
         canvas cnv{offscr.context()};
         cnv.fill_style(colors::black);
         cnv.fill_rect(0, 0, 200, 200);
         context ctx{tv.view_, cnv, &e, rect{0, 0, 200, 200}};
         e.draw(ctx);
      }
      return img;
   }
}

TEST_CASE("marks page: a minor rule never covers a major one", "[marks_page]")
{
   // The page says a minor division that lands on a major one is not
   // drawn, so the major rule is what shows there. The theme's major grid
   // color is a neutral grey and its minor color is blue, which is what
   // tells the two apart in the pixels.
   //
   // With four major and twenty minor divisions over 200 pixels, the
   // majors fall every 50 and the minors every 10, so y 100 carries both
   // and y 110 carries only a minor.
   {
      auto img = grid_render(vgrid_lines{4, 20});

      auto major = ink_at(img, 100, 100);
      auto minor = ink_at(img, 100, 110);

      CHECK(major.a > 0);                          // a rule is there
      CHECK(minor.a > 0);
      CHECK(minor.b > minor.r * 2);                // the minor rule is blue
      CHECK(major.b < major.r * 2);                // the major one is not
   }

   // The same across the other axis.
   {
      auto img = grid_render(hgrid_lines{4, 20});

      auto major = ink_at(img, 100, 100);          // sampled by column
      auto minor = ink_at(img, 110, 100);

      CHECK(major.a > 0);
      CHECK(minor.a > 0);
      CHECK(minor.b > minor.r * 2);
      CHECK(major.b < major.r * 2);
   }
}

TEST_CASE("marks page: a log decade's minor ticks start at 2", "[marks_page]")
{
   // A minor tick at log10(1) would sit on the major tick that opens the
   // decade, so the page says the minors run from 2. An extra tick hidden
   // under a major one barely shows, so the check is made where it does:
   // the major ticks are put on pixel centres, where a 1.5 wide stroke
   // fills one column exactly, and that column is sampled twice. Once near
   // the top edge, which only major ticks reach, and once in the middle,
   // where minor ticks draw as well. The two have to agree.
   float const size = 30;                 // minor ticks are inset size/6
   cycfi::artist::image img{220, 60, 1};
   {
      offscreen_image offscr{img};
      canvas cnv{offscr.context()};
      cnv.fill_style(colors::black);
      cnv.fill_rect(0, 0, 220, 60);

      // Two decades, with the major ticks at x 10.5, 110.5 and 210.5.
      draw_slider_marks_log(
         cnv, rect{10.5, 0, 210.5, 60}, size, 2, 10, colors::white);
   }

   auto outer = pixel_at(img, 110, 2);    // majors only
   auto inset = pixel_at(img, 110, 30);   // majors and minors

   CHECK(outer.r > 0);                    // the major tick really is there
   CHECK(std::abs(inset.r - outer.r) <= 2);

   // The nearest minor tick of the first decade is the digit 9, at
   // log10(9) of the way along, well clear of the major at 110.5.
   auto nine = pixel_at(img, 106, 30);
   CHECK(nine.r > 0);
   CHECK(pixel_at(img, 106, 2).r == 0);   // and it does not reach the edge
}

#endif // !ARTIST_RECORDING

TEST_CASE("marks page: slider marks figure", "[marks_page]")
{
   // images/controls/slider_marks.png: the three tick layouts, on the same
   // track at the same value.
   auto one =
      [](auto&& marks, char const* text)
      {
         return vtile(
            slider(basic_thumb<20>(), std::move(marks), 0.5),
            caption(text)
         );
      };

   figure(560, 250, "controls_slider_marks.png",
      margin({20, 16, 20, 16},
         vtile(
            one(slider_marks_lin<30>(basic_track<5, false>()),
               "slider_marks_lin<30>"),
            one(slider_marks_lin<30, 5, 10>(basic_track<5, false>()),
               "slider_marks_lin<30, 5, 10>"),
            one(slider_marks_log<30, 4>(basic_track<5, false>()),
               "slider_marks_log<30, 4>")
         )
      )
   );
}

TEST_CASE("marks page: slider example figure", "[marks_page]")
{
   // images/controls/slider_example.png: what the Slider Marks and Labels
   // example draws. The code here is the code on the page.
   figure(560, 100, "controls_slider_example.png",
      margin({30, 16, 30, 20},
         slider(
            basic_thumb<25>(),                              // a 25px thumb
            slider_labels<10>(                              // labels 10px out
               slider_marks_lin<40>(basic_track<5, false>()),
               0.8,                                         // label font size
               "0", "1", "2", "3", "4", "5"
            ),
            0.5                                             // initial value
         )
      )
   );
}

TEST_CASE("marks page: radial example figure", "[marks_page]")
{
   // images/controls/radial_example.png: the Radial Marks and Labels
   // example, beside the same dial with its ticks and no labels.
   figure(560, 165, "controls_radial_example.png",
      margin({20, 12, 20, 12},
         htile(
            vtile(
               vsize(110, align_center_middle(
                  dial(radial_marks<20>(basic_knob<50>()), 0.4)
               )),
               caption("radial_marks<20>")
            ),
            vtile(
               vsize(110, align_center_middle(
                  radial_labels<15>(                     // labels in a 15px ring
                     dial(
                        radial_marks<20>(                // ticks in a 20px ring
                           basic_knob<50>()              // a 50px knob
                        ),
                        0.4                              // initial value
                     ),
                     0.7,                                // label font size
                     "0", "5", "10"
                  )
               )),
               caption("radial_labels<15> around it")
            )
         )
      )
   );
}

TEST_CASE("marks page: decibel fader figure", "[marks_page]")
{
   // images/controls/db_fader.png: the decibel example at two ranges, and
   // the same marks as a grid behind a panel. The wider scale reaches
   // silence, so it carries the infinity mark at the bottom.
   auto fader =
      [](db_scale scale)
      {
         return slider(
            basic_thumb<25>(),                  // a 25px thumb
            slider_labels_db<10>(               // labels 10px out
               slider_marks_db<20>(             // ticks in a 20px band
                  basic_track<5, true>(),       // a vertical 5px track
                  scale
               ),
               0.8,                             // label font size
               scale
            ),
            scale.position(0)                   // start at unity
         );
      };

   figure(560, 360, "controls_db_fader.png",
      margin({20, 16, 20, 16},
         htile(
            vtile(
               align_center(hsize(90, fader(db_scale{-60, 10}))),
               caption("db_scale{-60, 10}")
            ),
            vtile(
               align_center(hsize(90, fader(db_scale{}))),
               caption("db_scale{} reaches silence")
            ),
            vtile(
               vgrid_lines_db{db_scale{-60, 10}},
               caption("vgrid_lines_db on the same scale")
            )
         )
      )
   );
}
