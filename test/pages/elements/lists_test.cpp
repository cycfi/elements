/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/elements/lists.adoc and
   draws its figures. The figures land in the build's results directory;
   copy them to docs/modules/ROOT/images/elements/ to update the page.
=============================================================================*/
#include "test_support.hpp"
#include <string>
#include <vector>
#include <algorithm>
#include <memory>

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
}

TEST_CASE("lists page: changes", "[lists_page]")
{
   SECTION("empty list, derived limits")
   {
      std::vector<std::string> items;
      int bad = 0;
      auto cp = basic_vcell_composer(items.size(),
         [&](std::size_t i) -> element_ptr
         {
            if (i >= items.size()) { ++bad; return share(label("?")); }
            return share(label(items[i]));
         });
      auto l = share(list(cp));
      test_view tv{extent{200, 200}};
      open(tv);
      tv.view_.content(hold(l), box(bkd_color));
      tv.view_.layout();
      tv.draw();
      CHECK(bad == 0);
   }
   SECTION("two inserts before a draw")
   {
      std::vector<std::string> items = {"a", "b"};
      auto cp = basic_vcell_composer(items.size(),
         [&](std::size_t i) { return share(label(items[i])); });
      auto l = share(list(cp));
      test_view tv{extent{200, 200}};
      open(tv);
      tv.view_.content(hold(l), box(bkd_color));
      tv.view_.layout();
      tv.draw();
      items.insert(items.begin(), "x");
      l->insert(0, 1);
      items.insert(items.begin(), "y");
      l->insert(0, 1);
      tv.draw();
      REQUIRE(l->size() == 4);
      for (std::size_t i = 0; i != 4; ++i)
      {
         auto lbl = dynamic_cast<text_reader_u8*>(&l->at(i));
         REQUIRE(lbl);
         CHECK(lbl->get_text() == items[i]);
      }
   }
   SECTION("erase then insert before a draw")
   {
      std::vector<std::string> items = {"a", "b", "c"};
      auto cp = basic_vcell_composer(items.size(),
         [&](std::size_t i) { return share(label(items[i])); });
      auto l = share(list(cp));
      test_view tv{extent{200, 200}};
      open(tv);
      tv.view_.content(hold(l), box(bkd_color));
      tv.view_.layout();
      tv.draw();
      erase_indices(items, {2});
      l->erase({2});
      items.push_back("d");
      items.push_back("e");
      l->insert(2, 2);
      tv.draw();
      REQUIRE(l->size() == 4);
      for (std::size_t i = 0; i != 4; ++i)
      {
         auto lbl = dynamic_cast<text_reader_u8*>(&l->at(i));
         REQUIRE(lbl);
         CHECK(lbl->get_text() == items[i]);
      }
   }
   SECTION("reverse for_each_visible")
   {
      auto cp = basic_vcell_composer(5,
         [&](std::size_t i) { return share(label("Item " + std::to_string(i))); });
      auto l = share(list(cp));
      test_view tv{extent{200, 200}};
      open(tv);
      tv.view_.content(hold(l), box(bkd_color));
      tv.view_.layout();
      tv.draw();
      std::vector<std::size_t> seen;
      context ctx{tv.view_, tv.cnv, l.get(), rect{0, 0, 200, 200}};
      l->for_each_visible(ctx,
         [&](element&, std::size_t ix, rect const&) { seen.push_back(ix); return false; },
         true);
      CHECK(seen == std::vector<std::size_t>{4, 3, 2, 1, 0});
   }
   SECTION("the focus follows its cell")
   {
      std::vector<std::string> items = {"a", "b", "c", "d"};
      auto cp = basic_vcell_composer(items.size(),
         [&](std::size_t i) { return share(input_box(items[i]).first); });
      auto l = share(list(cp));
      test_view tv{extent{200, 200}};
      open(tv);
      tv.view_.content(hold(l), box(bkd_color));
      tv.view_.layout();
      tv.draw();
      l->focus(2);                                 // on "c"
      REQUIRE(l->focus_index() == 2);

      // Erased before it: it moves up.
      erase_indices(items, {0});
      l->erase({0});
      tv.draw();
      CHECK(l->focus_index() == 1);

      // Inserted before it: it moves down.
      items.insert(items.begin(), "x");
      l->insert(0, 1);
      tv.draw();
      CHECK(l->focus_index() == 2);

      // Moved: it goes with its cell.
      move_indices(items, 0, {2});
      l->move(0, {2});
      tv.draw();
      CHECK(l->focus_index() == 0);

      // Erased after it: it stays.
      erase_indices(items, {3});
      l->erase({3});
      tv.draw();
      CHECK(l->focus_index() == 0);

      // Its cell erased: the list lets go of it.
      erase_indices(items, {0});
      l->erase({0});
      tv.draw();
      tv.draw();
      CHECK(l->focus_index() == -1);
   }
}

TEST_CASE("lists page: cells", "[lists_page]")
{
   SECTION("made as they come into view")
   {
      std::vector<int> made(1000, 0);
      auto cp = basic_cell_composer(1000,
         [&](std::size_t i)
         {
            ++made[i];
            return share(margin({20, 2, 20, 2}, align_left(label("Item"))));
         });
      auto l = share(list(cp));
      test_view tv{extent{200, 200}};
      open(tv);
      tv.view_.content(vscroller(hold(l)), box(bkd_color));
      tv.view_.layout();
      tv.draw();

      // Item 0 is made to measure the row, then those in view.
      auto n = std::count_if(made.begin(), made.end(), [](int m) { return m > 0; });
      CHECK(n < 20);
      CHECK(made[999] == 0);
   }
   SECTION("let go of as they leave it")
   {
      for (bool manage : {true, false})
      {
         std::vector<std::weak_ptr<element>> cells(100);
         auto cp = basic_cell_composer(100,
            [&](std::size_t i)
            {
               auto e = share(label("Item"));
               cells[i] = e;
               return e;
            });
         auto l = share(list(cp, manage));
         auto sc = share(vscroller(hold(l)));
         test_view tv{extent{200, 100}};
         open(tv);
         tv.view_.content(hold(sc), box(bkd_color));
         tv.view_.layout();
         tv.draw();
         REQUIRE(!cells[1].expired());
         sc->valign(1);                       // to the end
         tv.view_.layout();
         tv.draw();
         CHECK(cells[1].expired() == manage);
      }
   }
   SECTION("static limits")
   {
      auto cp = basic_cell_composer(100, 30, 10,
         [](std::size_t) { return share(label("Item")); });
      test_view tv{extent{200, 200}};
      basic_context bctx{tv.view_, tv.cnv};
      auto l = list(cp);
      auto lim = l.limits(bctx);
      CHECK(lim.min.x == 100);
      CHECK(lim.max.x == full_extent);
      CHECK(lim.min.y == 300);
      CHECK(lim.max.y == 300);
   }
   SECTION("hlist")
   {
      auto cp = basic_hcell_composer(10,
         [](std::size_t) { return share(fixed_size({40, 20}, element{})); });
      test_view tv{extent{200, 200}};
      basic_context bctx{tv.view_, tv.cnv};
      auto l = hlist(cp);
      auto lim = l.limits(bctx);
      CHECK(lim.min.x == 400);
      CHECK(lim.min.y == 20);
   }
}

TEST_CASE("lists page: vector helpers", "[lists_page]")
{
   std::vector<char> v = {'a', 'b', 'c', 'd', 'e'};
   move_indices(v, 1, {3, 4});
   CHECK(v == std::vector<char>{'a', 'd', 'e', 'b', 'c'});
   erase_indices(v, {0, 2});
   CHECK(v == std::vector<char>{'d', 'b', 'c'});
}

#if !defined(ARTIST_RECORDING)
TEST_CASE("lists page: example", "[lists_page]")
{
   test_view tv{extent{320, 200}, 2};
   open(tv);
   auto& view_ = tv.view_;
   view_.content(
      // The page's example: examples/list, verbatim from here.
      [&]
      {
         auto&& draw_cell =
            [](std::size_t index)
            {
               auto text = "This is item number " + std::to_string(index+1);
               return share(margin({20, 2, 20, 2}, align_left(label(text))));
            };

         auto my_composer =
            basic_cell_composer(
               1000000,                // size (number of rows)
               draw_cell               // Composer function
            );

         auto content = share(list{my_composer});
         return vscroller(hold(content));
      }(),
      box(bkd_color)
   );
   view_.layout();
   tv.draw();
   tv.img.save_png(std::string{RESULTS_PATH} + "lists_example.png");
}
#endif
