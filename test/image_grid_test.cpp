/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   An image grid menu and cell, on an offscreen image of four by two cells,
   driven as a user drives them: the cursor moved over a cell, presses and
   releases on one, and a styler told what each cell is as it is drawn.
=============================================================================*/
#include "test_support.hpp"

#include <map>
#include <string>
#include <memory>
#include <utility>
#include <vector>

namespace cycfi::elements::test
{
   namespace
   {
      using grid = basic_image_grid;

      // Four columns by two rows of 20 by 20 cells
      image_ptr make_image()
      {
         return std::make_shared<artist::image>(80.0f, 40.0f);
      }

      // A styler that remembers, per cell, the state and bounds it was
      // drawn with
      struct cell_probe : element
      {
         void draw(context const& ctx) override
         {
            auto g = find_parent<grid*>(ctx);
            REQUIRE(g);
            (*seen)[g->drawing()] = {g->state(g->drawing()), ctx.bounds};
         }

         using record = std::pair<grid::cell_state, rect>;
         std::shared_ptr<std::map<int, record>> seen =
            std::make_shared<std::map<int, record>>();
      };

      struct menu_view : test_view
      {
         explicit menu_view(float scale = 1)
          : test_view{{200, 120}}
          , probe{}
          , menu{share(image_grid_menu(make_image(), 4, 2, scale, probe))}
         {
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
            headless::open(view_);
#endif
            view_.content(align_left_top(hold(menu)));
            view_.layout();
            draw();
         }

         // The middle of a cell, as laid out at the top left
         point at(int column, int row, float scale = 1) const
         {
            return {(column * 20 + 10) * scale, (row * 20 + 10) * scale};
         }

         static mouse_button press(point p, bool down)
         {
            mouse_button btn{};
            btn.down = down;
            btn.state = mouse_button::left;
            btn.num_clicks = 1;
            btn.pos = p;
            return btn;
         }

         void click(point down, point up)
         {
            view_.click(press(down, true));
            view_.click(press(up, false));
            draw();
         }

         cell_probe probe;          // before the menu, which copies it
         using menu_type = decltype(
            image_grid_menu(make_image(), 4, 2, 1.0f, cell_probe{}));
         std::shared_ptr<menu_type> menu;
      };
   }
}

using namespace cycfi::elements;
using namespace cycfi::elements::test;

TEST_CASE("image_grid_menu: the image's size, at its scale")
{
   menu_view v{0.5f};
   auto const& seen = *v.probe.seen;
   REQUIRE(seen.size() == 8);
   CHECK(seen.at(7).second.right == Approx(40));
   CHECK(seen.at(7).second.bottom == Approx(20));
   CHECK(seen.at(7).second.width() == Approx(10));
}

TEST_CASE("image_grid_menu: every cell is drawn, in the image's order")
{
   menu_view v;
   auto const& seen = *v.probe.seen;
   REQUIRE(seen.size() == 8);
   CHECK(seen.at(0).second.left == Approx(0));
   CHECK(seen.at(6).second.left == Approx(40));
   CHECK(seen.at(6).second.top == Approx(20));
   CHECK(seen.at(6).second.width() == Approx(20));
   CHECK(seen.at(0).first == grid::current);
   CHECK(seen.at(6).first == grid::normal);
}

TEST_CASE("image_grid_menu: a press and a release on a cell picks it")
{
   menu_view v;
   std::vector<int> picked;
   v.menu->on_change = [&](int i) { picked.push_back(i); };

   v.click(v.at(2, 1), v.at(2, 1));
   CHECK(v.menu->value() == 6);
   CHECK((*v.probe.seen).at(6).first == grid::current);

   // The current cell again: nothing changes, so nothing is said
   v.click(v.at(2, 1), v.at(2, 1));
   CHECK(picked == std::vector<int>{6});

   // A release on another cell picks neither
   v.click(v.at(0, 0), v.at(1, 0));
   CHECK(v.menu->value() == 6);
   CHECK(picked == std::vector<int>{6});
}

TEST_CASE("image_grid_menu: the cell under the cursor is hot")
{
   menu_view v;
   v.view_.cursor(v.at(3, 0), cursor_tracking::hovering);
   v.draw();
   CHECK((*v.probe.seen).at(3).first == grid::hot);
   CHECK((*v.probe.seen).at(2).first == grid::normal);

   v.view_.cursor({150, 100}, cursor_tracking::hovering);
   v.draw();
   CHECK((*v.probe.seen).at(3).first == grid::normal);
}

TEST_CASE("image_grid_menu: value is clamped to the cells")
{
   menu_view v;
   v.menu->value(99);
   CHECK(v.menu->value() == 7);
   v.menu->value(-3);
   CHECK(v.menu->value() == 0);
}

TEST_CASE("image_grid_cell: the current cell, fitted, its proportions kept")
{
   test_view tv{{100, 60}};
   cell_probe probe;
   auto cell = share(image_grid_cell(make_image(), 4, 2, probe));
   cell->value(5);
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(hold(cell));
   tv.view_.layout();
   tv.draw();

   auto const& seen = *probe.seen;
   REQUIRE(seen.size() == 1);
   REQUIRE(seen.count(5) == 1);
   auto const b = seen.at(5).second;
   CHECK(b.width() == Approx(60));           // square cells, as high as fits
   CHECK(b.height() == Approx(60));
   CHECK(b.left == Approx(20));              // centered
   CHECK(seen.at(5).first == grid::current);
}

TEST_CASE("image_regions: each region of the cell drawn is asked its state")
{
   test_view tv{{100, 60}};
   std::vector<std::pair<int, int>> asked;
   image_regions regions{
      {
         {}, {}, {}
       , {{0.1f, 0.1f, 0.4f, 0.4f}, {0.5f, 0.5f, 0.9f, 0.9f}}
      }
    , [&](int cell, int region)
      {
         asked.push_back({cell, region});
         return region == 0? image_regions::lit : image_regions::dimmed;
      }
   };
   auto cell = share(image_grid_cell(make_image(), 4, 2, regions));
   cell->value(3);
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(hold(cell));
   tv.view_.layout();
   tv.draw();

   std::vector<std::pair<int, int>> const expect = {{3, 0}, {3, 1}};
   CHECK(asked == expect);

   // A cell with no regions listed asks nothing
   asked.clear();
   cell->value(7);
   tv.draw();
   CHECK(asked.empty());
}

TEST_CASE("image_grid_highlight: a label for each cell, where asked")
{
   test_view tv{{200, 120}};
   std::vector<int> labeled;
   image_grid_highlight highlight;
   highlight.label = [&](int i)
   {
      labeled.push_back(i);
      return std::to_string(i + 1);
   };
   auto menu = share(image_grid_menu(make_image(), 4, 2, 1.0f, highlight));
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(align_left_top(hold(menu)));
   tv.view_.layout();
   tv.draw();

   std::vector<int> const expect = {0, 1, 2, 3, 4, 5, 6, 7};
   CHECK(labeled == expect);
}

TEST_CASE("image: filled, the whole image fills the space given")
{
   test_view tv{{100, 60}};
   auto img = share(image{make_image(), image::fill});
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(hold(img));
   tv.view_.layout();
   tv.draw();

   rect src, bounds;
   tv.view_.in_context_do(*img, [&](context const& ctx)
   {
      src = img->source_rect(ctx);
      bounds = ctx.bounds;
   });
   CHECK(src.width() == Approx(80));        // all of the image
   CHECK(src.height() == Approx(40));
   CHECK(bounds.width() == Approx(100));    // into all of the space
   CHECK(bounds.height() == Approx(60));
}
