/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Draws the figures of docs/modules/ROOT/pages/controls/marks.adoc. Each
   case builds the element the page shows, lays it out in a view and saves
   what it drew, so the picture on the page and the code on the page cannot
   drift apart.

   Figures land in the build's results directory. Copy them to
   docs/modules/ROOT/images/controls/ to update the page.
=============================================================================*/
#include "test_support.hpp"

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

TEST_CASE("marks page: grid lines figure", "[marks_page]")
{
   // images/controls/grid_lines.png: the v and h name the axis that is
   // divided, not the direction the rules run.
   figure(560, 220, "controls_grid_lines.png",
      margin({20, 16, 20, 16},
         htile(
            vtile(vgrid_lines{4, 20}, caption("vgrid_lines{4, 20}")),
            hspace(30),
            vtile(hgrid_lines{4, 20}, caption("hgrid_lines{4, 20}"))
         )
      )
   );
}

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
