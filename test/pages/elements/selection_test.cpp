/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/elements/selection.adoc.
=============================================================================*/
#include "test_support.hpp"
#include <string>
#include <vector>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   // The page's example: an item that is selectable, drawn highlighted when
   // it is.
   struct item : element, selectable
   {
      item(std::string text = "") : _text{std::move(text)} {}

      view_limits limits(basic_context const&) const override
      {
         return {{40, 22}, {full_extent, 22}};
      }

      void draw(context const& ctx) override
      {
         auto& cnv = ctx.canvas;
         auto const& thm = get_theme();
         if (_selected)
         {
            cnv.fill_style(thm.indicator_color.opacity(0.6));
            cnv.add_rect(ctx.bounds);
            cnv.fill();
         }
         cnv.fill_style(thm.label_font_color);
         cnv.font(thm.label_font);
         cnv.text_align(cnv.left | cnv.middle);
         cnv.fill_text(_text, {ctx.bounds.left + 8, center_point(ctx.bounds).y});
      }

      bool wants_control() const override { return true; }
      bool is_selected() const override   { return _selected; }
      void select(bool state) override    { _selected = state; }

      std::string _text;
      bool _selected = false;
   };

   mouse_button left_button(point p, bool down, int modifiers = 0)
   {
      mouse_button btn{};
      btn.down = down;
      btn.state = mouse_button::left;
      btn.num_clicks = 1;
      btn.pos = p;
      btn.modifiers = modifiers;
      return btn;
   }

   // Five items in a column, 20 pixels each, under a selection list.
   struct list_view : test_view
   {
      list_view(bool multi = true)
       : test_view{{200, 100}}
       , items{share(item{}), share(item{}), share(item{}), share(item{}), share(item{})}
       , content{share(selection_list(
            vtile(hold(items[0]), hold(items[1]), hold(items[2]),
                  hold(items[3]), hold(items[4])), multi))}
       , list{find_element<selection_list_element*>(content.get())}
      {
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
         headless::open(view_);
#endif
         list->on_select = [this](int start, int end)
         {
            selects.push_back({start, end});
         };
         view_.content(hold(content));
         view_.layout();
         draw();
      }

      void click_item(int i, int modifiers = 0)
      {
         point p{100, 10.0f + 20 * i};
         view_.click(left_button(p, true, modifiers));
         view_.click(left_button(p, false, modifiers));
      }

      std::vector<bool> selected() const
      {
         std::vector<bool> r;
         for (auto const& i : items)
            r.push_back(i->is_selected());
         return r;
      }

      std::vector<std::shared_ptr<item>>  items;
      std::shared_ptr<element>            content;
      selection_list_element*             list;
      std::vector<std::pair<int, int>>    selects;
   };
}

TEST_CASE("selection page: click selects one item", "[selection_page]")
{
   list_view lv;
   lv.click_item(2);
   CHECK(lv.selected() == std::vector<bool>{false, false, true, false, false});
   CHECK(lv.list->get_selection() == selection_list_element::indices_type{2});
   CHECK(lv.selects == std::vector<std::pair<int, int>>{{2, 2}});

   // Another click moves the selection.
   lv.click_item(4);
   CHECK(lv.selected() == std::vector<bool>{false, false, false, false, true});
   CHECK(lv.list->get_select_start() == 4);
}

TEST_CASE("selection page: shift-click extends, action-click toggles", "[selection_page]")
{
   list_view lv;
   lv.click_item(1);
   lv.click_item(3, mod_shift);
   CHECK(lv.selected() == std::vector<bool>{false, true, true, true, false});
   CHECK(lv.list->get_select_start() == 1);
   CHECK(lv.list->get_select_end() == 3);

   lv.click_item(2, mod_action);              // toggle 2 off
   CHECK(lv.selected() == std::vector<bool>{false, true, false, true, false});
   lv.click_item(0, mod_action);              // toggle 0 on
   CHECK(lv.selected() == std::vector<bool>{true, true, false, true, false});
}

TEST_CASE("selection page: single selection", "[selection_page]")
{
   list_view lv{false};
   lv.click_item(1);
   lv.click_item(3, mod_shift);               // no extension without multi
   CHECK(lv.selected() == std::vector<bool>{false, false, false, true, false});
   lv.click_item(0, mod_action);              // action-click moves, not adds
   CHECK(lv.selected() == std::vector<bool>{true, false, false, false, false});
}

TEST_CASE("selection page: the keyboard", "[selection_page]")
{
   list_view lv;
   lv.click_item(2);
   // Keys go to the focus, and an item that does not want focus leaves
   // the list without it: Tab gives the list the focus.
   lv.view_.key({key_code::tab, key_action::press, 0});
   lv.view_.key({key_code::down, key_action::press, 0});
   CHECK(lv.selected() == std::vector<bool>{false, false, false, true, false});
   lv.view_.key({key_code::up, key_action::press, mod_shift});
   CHECK(lv.selected() == std::vector<bool>{false, false, true, true, false});
   lv.view_.key({key_code::a, key_action::press, mod_action});
   CHECK(lv.selected() == std::vector<bool>{true, true, true, true, true});
}

TEST_CASE("selection page: the example compiles", "[selection_page]")
{
   auto list = share(selection_list(
      vtile(item{"Alpha"}, item{"Bravo"}, item{"Charlie"}, item{"Delta"}, item{"Echo"})));
   list->on_select = [](int, int) {};
   test_view tv{extent{200, 100}, 1};
   tv.view_.content(vscroller(hold(list)), box(colors::black));
   tv.view_.layout();
   tv.draw();
   CHECK(list->get_selection().empty());
}

TEST_CASE("selection page: from code", "[selection_page]")
{
   list_view lv;
   lv.list->set_selection({0, 4});
   CHECK(lv.selected() == std::vector<bool>{true, false, false, false, true});
   lv.list->select_none();
   CHECK(lv.selected() == std::vector<bool>{false, false, false, false, false});
   lv.list->select_all();
   CHECK(lv.list->get_selection() == selection_list_element::indices_type{0, 1, 2, 3, 4});
}

TEST_CASE("selection page: example figure", "[selection_page]")
{
   // The page's example rendered, Bravo and Charlie selected.
   auto list = share(selection_list(
      vtile(item{"Alpha"}, item{"Bravo"}, item{"Charlie"}, item{"Delta"}, item{"Echo"})));
   test_view tv{extent{200, 140}, 2};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(
      margin({10, 10, 10, 10}, vscroller(hold(list))),
      box(rgba(35, 35, 37, 255))
   );
   tv.view_.layout();
   tv.draw();
   list->set_selection({1, 2});
   tv.draw();
   tv.img.save_png(std::string{RESULTS_PATH} + "selection_example.png");
}
