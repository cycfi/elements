/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/styles/notebooks.adoc and
   draws its figures. The figures land in the build's results directory;
   copy them to docs/modules/ROOT/images/styles/ to update the page.
=============================================================================*/
#include "test_support.hpp"
#include <elements/element/style/notebook.hpp>
#include <stdexcept>
#include <string>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   void open(test_view& tv)
   {
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
      headless::open(tv.view_);
#endif
   }

   void click(view& v, point p)
   {
      mouse_button b{};
      b.state = mouse_button::left;
      b.num_clicks = 1;
      b.pos = p;
      b.down = true;
      v.click(b);
      b.down = false;
      v.click(b);
   }
}

TEST_CASE("notebooks page: probe", "[notebooks_page]")
{
   SECTION("a click on a tab selects its page")
   {
      test_view tv{extent{400, 300}};
      open(tv);
      auto nb = share(vnotebook(tv.view_,
         deck(box(colors::red), box(colors::green), box(colors::blue)),
         tab("One"), tab("Two"), tab("Three")));
      tv.view_.content(hold(nb));
      tv.view_.layout();
      tv.draw();

      // The pages, and the tabs, inside the notebook.
      auto* pages = find_element<deck_element*>(&nb->at(1));
      auto* row = find_subject<composite_base*>(&nb->at(0));
      REQUIRE(pages);
      REQUIRE(row);
      auto tab_at = [&](std::size_t i) { return find_element<basic_choice*>(&row->at(i)); };
      REQUIRE(tab_at(0));
      CHECK(tab_at(0)->is_selected());
      CHECK(pages->selected() == 0);

      // Into the second tab: right of the first.
      basic_context bctx{tv.view_, tv.cnv};
      auto w1 = row->at(0).limits(bctx).min.x;
      click(tv.view_, {w1 + 10, 10});
      tv.view_.poll();
      CHECK(pages->selected() == 1);
      CHECK(tab_at(1)->is_selected());
      CHECK(!tab_at(0)->is_selected());
   }
   SECTION("tabs and pages must match")
   {
      // A deck made with deck(...) has a fixed number of pages: a
      // mismatch does not compile.
      using fixed = decltype(deck(box(colors::red), box(colors::green)));
      static_assert(detail::fixed_pages<fixed>::value);
      static_assert(detail::fixed_pages<fixed>::size == 2);
      static_assert(!detail::fixed_pages<deck_composite>::value);

      // A deck_composite's number of pages can change: a mismatch throws.
      test_view tv{extent{400, 300}};
      auto pages = share(deck_composite{});
      pages->push_back(share(box(colors::red)));
      pages->push_back(share(box(colors::green)));
      auto one = share(tab("One"));
      CHECK_THROWS_AS(link_tabs(tv.view_, pages, one), std::runtime_error);
   }
}

TEST_CASE("notebooks page: link_tabs", "[notebooks_page]")
{
   test_view tv{extent{400, 300}};
   open(tv);
   auto& view_ = tv.view_;

   // The page's snippet.
   auto pages = share(deck(box(colors::red), box(colors::green)));
   auto red   = share(tab("Red"));
   auto green = share(tab("Green"));

   link_tabs(view_, pages, red, green);
   auto notebook = vtile(
      align_left(htile(hold(red), hold(green))),
      hold(pages)
   );

   CHECK(red->is_selected());
   view_.content(notebook);
   view_.layout();
   tv.draw();

   // The handles reach the notebook: select the second page from code.
   green->on_click(true);
   CHECK(pages->selected() == 1);
}

#if !defined(ARTIST_RECORDING)
namespace
{
   auto constexpr bkd_color = rgba(35, 35, 37, 255);

   // The page's example: examples/notebook, verbatim.
   auto make_tabs(view& view_)
   {
      auto make_page = [](auto text)
      {
         return layer(
            align_center_middle(label(text).font_size(100)),
            frame{}
         );
      };

      return vnotebook(
         view_,
         deck(
            make_page("Dream"),
            make_page("Insanity"),
            make_page("Imagination")
         ),
         tab("Dream"),
         tab("Insanity"),
         tab("Imagination")
      );
   }
}

TEST_CASE("notebooks page: figures", "[notebooks_page]")
{
   {
      test_view tv{extent{560, 300}, 2};
      open(tv);
      tv.view_.content(make_tabs(tv.view_), box(bkd_color));
      tv.view_.layout();
      tv.draw();
      tv.img.save_png(std::string{RESULTS_PATH} + "notebooks_vnotebook.png");
   }
   {
      test_view tv{extent{560, 300}, 2};
      open(tv);
      auto make_page = [](auto text)
      {
         return layer(align_center_middle(label(text).font_size(60)), frame{});
      };
      tv.view_.content(
         hnotebook(tv.view_,
            deck(make_page("Dream"), make_page("Insanity"), make_page("Imagination")),
            tab("Dream"), tab("Insanity"), tab("Imagination")),
         box(bkd_color));
      tv.view_.layout();
      tv.draw();
      tv.img.save_png(std::string{RESULTS_PATH} + "notebooks_hnotebook.png");
   }
}
#endif
