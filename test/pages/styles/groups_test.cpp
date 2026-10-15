/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/styles/groups.adoc and
   draws its figures. The figures land in the build's results directory;
   copy them to docs/modules/ROOT/images/styles/ to update the page.
=============================================================================*/
#include "test_support.hpp"
#include <elements/element/style/group.hpp>
#include <elements/element/style/pane.hpp>
#include <string>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

#if !defined(ARTIST_RECORDING)
namespace
{
   void open(test_view& tv)
   {
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
      headless::open(tv.view_);
#endif
   }

   // The pixel at (x, y): premultiplied B, G, R, A.
   std::array<int, 4> pixel(cycfi::artist::image const& img, int x, int y)
   {
      auto p = reinterpret_cast<std::uint8_t const*>(img.pixels());
      auto i = 4 * (y * int(img.bitmap_size().x) + x);
      return {p[i], p[i+1], p[i+2], p[i+3]};
   }

   template <typename E>
   std::array<int, 4> panel_pixel(E&& e)
   {
      test_view tv{extent{200, 200}, 1};
      open(tv);
      tv.view_.content(std::forward<E>(e), box(colors::white));
      tv.view_.layout();
      tv.draw();
      return pixel(tv.img, 100, 150);    // in the content, below the title
   }
}

TEST_CASE("groups page: pane opacity", "[groups_page]")
{
   // pane is pane_ex with a title bar and the theme's panel opacity.
   auto content = [] { return vsize(150, element{}); };
   auto a = panel_pixel(pane("Title", content()));
   auto b = panel_pixel(pane_ex(title_bar{}, "Title", content()));
   CHECK(a == b);
}
namespace
{
   auto constexpr bkd_color = rgba(35, 35, 37, 255);

   auto body(char const* text)
   {
      return margin({20, 45, 20, 20}, align_left_top(label(text)));
   }

   template <typename E>
   void save(extent size, E&& content, char const* name)
   {
      test_view tv{size, 2};
      open(tv);
      tv.view_.content(margin({20, 20, 20, 20}, std::forward<E>(content)), box(bkd_color));
      tv.view_.layout();
      tv.draw();
      tv.img.save_png(std::string{RESULTS_PATH} + name);
   }
}

TEST_CASE("groups page: figures", "[groups_page]")
{
   save({600, 170},
      htile(
         hmin_size(170, group("Group", body("Content"))),
         margin_left(20, hmin_size(170, group("Left", body("Content"), 1.0, false))),
         margin_left(20, hmin_size(170, unframed_group("Unframed", body("Content"))))
      ),
      "groups_groups.png");

   save({600, 170},
      htile(
         hmin_size(260, pane("Pane", margin({20, 20, 20, 20}, align_left_top(label("Content"))))),
         margin_left(20, hmin_size(260, pane("Left", margin({20, 20, 20, 20}, align_left_top(label("Content"))), 1.0, false)))
      ),
      "groups_panes.png");
}

TEST_CASE("groups page: example", "[groups_page]")
{
   // The page's example: examples/buttons, verbatim.
   auto  check_box1 = check_box("Reionizing electrons");
   auto  check_box2 = check_box("The Nexus Meridian Unfolding");
   auto  check_box3 = check_box("Serenity Dreamscape Exploration");
   auto  check_box4 = check_box("Forever Disabled");
   auto  check_box5 = check_box("Forever Checked");

   check_box1.value(true);
   check_box2.value(true);
   check_box3.value(true);
   check_box4.enable(false); // Disable this one
   check_box5.value(true);
   check_box5.enable(false);

   auto  check_boxes =
         group("Check boxes",
            margin({10, 45, 20, 20},
               vtile_spaced(10.0, // space in between
                  align_left(check_box1),
                  align_left(check_box2),
                  align_left(check_box3),
                  align_left(check_box4),
                  align_left(check_box5)
               )
            )
         );

   save({360, 250}, align_left_top(hsize(320, check_boxes)), "groups_example.png");
}
TEST_CASE("groups page: pane example", "[groups_page]")
{
   // The page's pane example.
   auto settings =
      pane("Settings",
         margin({20, 20, 20, 20},
            vtile_spaced(10.0,
               align_left(check_box("Snap to grid")),
               align_left(check_box("Show rulers"))
            )
         )
      );
   test_view tv{extent{200, 200}};
   basic_context bctx{tv.view_, tv.cnv};
   CHECK(settings.limits(bctx).min.x > 0);
}

#endif
