/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/elements/menus.adoc and
   draws its figures. The figures land in the build's results directory;
   copy them to docs/modules/ROOT/images/elements/ to update the page.
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
   auto constexpr bkd_color = rgba(35, 35, 37, 255);

   void open(test_view& tv)
   {
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
      headless::open(tv.view_);
#endif
   }

   void wait(view& v, std::chrono::milliseconds t = 1ms)
   {
      std::this_thread::sleep_for(t);
      v.poll();
   }

   mouse_button left(point p, bool down)
   {
      mouse_button b{};
      b.state = mouse_button::left;
      b.down = down;
      b.num_clicks = 1;
      b.pos = p;
      return b;
   }

   void click(view& v, point p)
   {
      v.click(left(p, true));
      v.click(left(p, false));
   }

   bool press(view& v, key_code k, int mods = 0)
   {
      return v.key(key_info{k, key_action::press, mods});
   }

#if defined(__APPLE__)
   constexpr int action_mods = mod_action | mod_command;
#else
   constexpr int action_mods = mod_action | mod_control;
#endif

   // A menu button of one item, Alpha, with `sk` for its shortcut.
   auto one_item_menu(shortcut_key sk, int& count)
   {
      auto btn = share(button_menu("Menu"));
      auto item = menu_item("Alpha", sk);
      item.on_click = [&count]{ ++count; };
      btn->menu(layer(item, panel{}));
      return btn;
   }

   std::shared_ptr<basic_popup_menu_element> menu_of(view& v)
   {
      return std::dynamic_pointer_cast<basic_popup_menu_element>(v.layers().back());
   }

   struct fixture
   {
      fixture(menu_position pos = menu_position::bottom_right, point at = {20, 20})
       : tv{extent{400, 300}}
      {
         open(tv);
         btn = share(button_menu("Menu", pos));
         auto a_ = share(menu_item("Alpha", shortcut_key{key_code::f, mod_action}));
         auto b_ = share(menu_item("Beta"));
         auto c_ = share(menu_item("Gamma"));
         a = a_; b = b_; c = c_;
         a->on_click = [this]{ ++clicked_a; };
         b->on_click = [this]{ ++clicked_b; };
         c->is_enabled = [this]{ return gamma_enabled; };
         btn->menu(hsize(150, layer(vtile(hold(a_), hold(b_), hold(c_)), panel{})));
         tv.view_.content(
            margin({at.x, at.y, 0, 0}, align_left_top(hsize(100, hold(btn)))),
            box(bkd_color)
         );
         tv.view_.layout();
         tv.draw();
      }

      void open_menu()
      {
         click(tv.view_, {at_btn().x, at_btn().y});
         wait(tv.view_); wait(tv.view_);
      }

      point at_btn() const { return {30, 30}; }

      test_view tv;
      decltype(share(button_menu("x"))) btn;
      std::shared_ptr<basic_menu_item_element> a, b, c;
      int clicked_a = 0, clicked_b = 0;
      bool gamma_enabled = true;
   };
}

TEST_CASE("menus page: probe", "[menus_page]")
{
   SECTION("open below")
   {
      fixture f;
      f.open_menu();
      auto m = menu_of(f.tv.view_);
      REQUIRE(m);
      INFO(m->bounds().left << " " << m->bounds().top);
      CHECK(f.btn->value());
      CHECK(m->bounds().top >= 20);
   }
   SECTION("keys")
   {
      fixture f;
      f.open_menu();
      CHECK(press(f.tv.view_, key_code::down));
      CHECK(f.a->is_selected());
      CHECK(press(f.tv.view_, key_code::down));
      CHECK(f.b->is_selected());
      CHECK(press(f.tv.view_, key_code::enter));
      wait(f.tv.view_); wait(f.tv.view_);
      CHECK(f.clicked_b == 1);
      CHECK(!menu_of(f.tv.view_));
      CHECK(!f.btn->value());
   }
   SECTION("shortcut while open")
   {
      fixture f;
      f.open_menu();
      press(f.tv.view_, key_code::f, action_mods);
      wait(f.tv.view_); wait(f.tv.view_);
      CHECK(f.clicked_a == 1);
   }
   SECTION("shortcut while closed")
   {
      fixture f;
      press(f.tv.view_, key_code::f, action_mods);
      wait(f.tv.view_);
      CHECK(f.clicked_a == 1);
   }
   SECTION("press, drag and release on an item")
   {
      fixture f;
      f.tv.view_.click(left(f.at_btn(), true));
      wait(f.tv.view_); wait(f.tv.view_);
      auto m = menu_of(f.tv.view_);
      REQUIRE(m);
      auto b = m->bounds();
      auto on_beta = point{b.left + 30, b.top + b.height() / 2};
      f.tv.view_.drag(left(on_beta, true));
      f.tv.view_.click(left(on_beta, false));
      wait(f.tv.view_); wait(f.tv.view_);
      CHECK(f.clicked_b == 1);
      CHECK(!menu_of(f.tv.view_));
   }
   SECTION("a quick click on an item acts once")
   {
      // The press and the release reach the view before it polls, as on
      // a host that dispatches every pending event first.
      fixture f;
      f.open_menu();
      auto m = menu_of(f.tv.view_);
      REQUIRE(m);
      auto b = m->bounds();
      click(f.tv.view_, {b.left + 30, b.top + b.height() / 2});
      wait(f.tv.view_); wait(f.tv.view_);
      CHECK(f.clicked_b == 1);
      CHECK(!menu_of(f.tv.view_));
   }
   SECTION("escape closes")
   {
      fixture f;
      f.open_menu();
      CHECK(press(f.tv.view_, key_code::down));
      CHECK(press(f.tv.view_, key_code::escape));
      wait(f.tv.view_); wait(f.tv.view_);
      CHECK(!menu_of(f.tv.view_));
      CHECK(f.clicked_a == 0);
   }
   SECTION("click outside closes")
   {
      fixture f;
      f.open_menu();
      click(f.tv.view_, {380, 280});
      wait(f.tv.view_); wait(f.tv.view_);
      CHECK(!menu_of(f.tv.view_));
      CHECK(!f.btn->value());
   }
   SECTION("all disabled, down")
   {
      fixture f;
      f.a->is_enabled = []{ return false; };
      f.b->is_enabled = []{ return false; };
      f.gamma_enabled = false;
      f.open_menu();
      press(f.tv.view_, key_code::down);
      CHECK(true);
   }
   SECTION("near the bottom: above")
   {
      fixture f{menu_position::bottom_right, {20, 260}};
      click(f.tv.view_, {30, 270});
      wait(f.tv.view_); wait(f.tv.view_);
      auto m = menu_of(f.tv.view_);
      REQUIRE(m);
      CHECK(m->bounds().bottom <= 260);
      CHECK(m->bounds().top >= 0);
   }
   SECTION("near the right: moved left")
   {
      fixture f{menu_position::bottom_right, {340, 20}};
      click(f.tv.view_, {350, 30});
      wait(f.tv.view_); wait(f.tv.view_);
      auto m = menu_of(f.tv.view_);
      REQUIRE(m);
      CHECK(m->bounds().right == view_bounds(f.tv.view_).right);
   }
}

TEST_CASE("menus page: hotkeys", "[menus_page]")
{
   auto const cmd_f = shortcut_key{key_code::f, mod_action};

   SECTION("blocked by a modal")
   {
      fixture f;
      f.tv.view_.add(share(ui_block()));
      wait(f.tv.view_);
      press(f.tv.view_, key_code::f, action_mods);
      CHECK(f.clicked_a == 0);
   }
   SECTION("not a disabled item")
   {
      fixture f;
      f.a->is_enabled = []{ return false; };
      CHECK(!press(f.tv.view_, key_code::f, action_mods));
      CHECK(f.clicked_a == 0);
   }
   SECTION("not a hidden menu")
   {
      test_view tv{extent{400, 300}};
      open(tv);
      int n = 0;
      auto h = share(hidable(hold(one_item_menu(cmd_f, n))));
      h->is_hidden = true;
      tv.view_.content(hold(h), box(bkd_color));
      tv.view_.layout();
      press(tv.view_, key_code::f, action_mods);
      CHECK(n == 0);
      h->is_hidden = false;
      press(tv.view_, key_code::f, action_mods);
      CHECK(n == 1);
   }
   SECTION("only the selected page of a deck")
   {
      test_view tv{extent{400, 300}};
      open(tv);
      int n1 = 0, n2 = 0;
      auto d = share(deck(hold(one_item_menu(cmd_f, n1)), hold(one_item_menu(cmd_f, n2))));
      tv.view_.content(hold(d), box(bkd_color));
      tv.view_.layout();
      d->select(1);
      press(tv.view_, key_code::f, action_mods);
      CHECK(n1 == 0);
      CHECK(n2 == 1);
   }
   SECTION("the focus first, then the first in order")
   {
      test_view tv{extent{400, 300}};
      open(tv);
      int first = 0, second = 0, sel_all = 0;
      auto cmd_a = shortcut_key{key_code::a, mod_action};
      auto [box_, tbox] = input_box("text");
      auto first_menu = one_item_menu(cmd_f, first);
      auto second_menu = one_item_menu(cmd_f, second);
      auto a_menu = one_item_menu(cmd_a, sel_all);
      tv.view_.content(
         htile(
            vtile(hold(first_menu), hold(a_menu)),
            vtile(box_, hold(second_menu))
         ),
         box(bkd_color)
      );
      tv.view_.layout();
      tv.draw();

      // Into the text box.
      click(tv.view_, {300, 10});
      wait(tv.view_);

      // The text box takes Select All itself.
      press(tv.view_, key_code::a, action_mods);
      CHECK(sel_all == 0);

      // Nothing in the focus takes the key: the first menu does.
      press(tv.view_, key_code::f, action_mods);
      CHECK(first == 1);
      CHECK(second == 0);
   }
   SECTION("only the elements that take hotkeys are asked")
   {
      // A list of 10000 cells: a key goes to the cells it holds, not to
      // the others, which it would have to make.
      int composed = 0;
      auto cp = basic_vcell_composer(10000,
         [&](std::size_t i) -> element_ptr
         {
            ++composed;
            return share(label("Item " + std::to_string(i)));
         });
      int n = 0;
      test_view tv{extent{400, 300}};
      open(tv);
      tv.view_.content(
         vtile(hold(one_item_menu(cmd_f, n)), vsize(200, vscroller(list(cp)))),
         box(bkd_color)
      );
      tv.view_.layout();
      tv.draw();
      auto before = composed;
      press(tv.view_, key_code::g, action_mods);
      CHECK(composed == before);
      press(tv.view_, key_code::f, action_mods);
      CHECK(n == 1);
   }
}

#if !defined(ARTIST_RECORDING)
namespace
{
   // The page's example: examples/menus, verbatim.
   // Show which item was chosen, by click or by its shortcut
   void alert(view& view_, std::string text)
   {
      open_popup(message_box1(view_, std::move(text), icons::info, []{}), view_);
   }

   auto make_popup_menu(char const* title, menu_position pos, view& view_)
   {
      auto popup  = button_menu(title, pos);

      auto skf = shortcut_key{key_code::f, mod_action};
      auto skp = shortcut_key{key_code::p, mod_action};

      auto quantum_feedback_loop = menu_item("Quantum Feedback Loop", skf);
      auto psionic_wave_oscillator = menu_item("Psionic Wave Oscillator", skp);
      auto photonic_mesh = menu_item("Photonic Mesh");
      auto antimatter_soup = menu_item("Antimatter Soup");

      // Here's how you dynamically enable or disable menu items:
      // Just give it a function that returns the enabled state.
      static bool enable = false; // just for demo; avoid statics!
      photonic_mesh.is_enabled   = []{ return enable; };
      antimatter_soup.is_enabled = []{ return enable; };

      // Here's how you attach functions that will be called when a
      // menu item is selected:
      quantum_feedback_loop.on_click =
         [&view_]()
         {
            enable = true;
            alert(view_, "Quantum Feedback Loop: Photonic Mesh and Antimatter Soup enabled");
         };
      psionic_wave_oscillator.on_click =
         [&view_]()
         {
            enable = false;
            alert(view_, "Psionic Wave Oscillator: Photonic Mesh and Antimatter Soup disabled");
         };

      auto sk1 = shortcut_key{key_code::g, mod_action};
      auto sk2 = shortcut_key{key_code::c, mod_action+mod_shift};
      auto sk3 = shortcut_key{key_code::b, mod_action+mod_alt};

      // The rest just say they were chosen
      auto alerting =
         [&view_](auto item, std::string text)
         {
            item.on_click = [&view_, text]() { alert(view_, text); };
            return item;
         };
      photonic_mesh.on_click = [&view_]() { alert(view_, "Photonic Mesh"); };
      antimatter_soup.on_click = [&view_]() { alert(view_, "Antimatter Soup"); };

      auto menu =
         layer(
            vtile(
               photonic_mesh,
               quantum_feedback_loop,
               psionic_wave_oscillator,
               alerting(menu_item("Gaia Abiogenesis", sk1), "Gaia Abiogenesis"),
               menu_item_spacer(),
               alerting(menu_item("Chaotic Synchronicity", sk2), "Chaotic Synchronicity"),
               alerting(menu_item("Omega Quadrant"), "Omega Quadrant"),
               antimatter_soup,
               alerting(menu_item("Dark Beta Quarks", sk3), "Dark Beta Quarks"),
               alerting(menu_item("Cosmic Infrared Shift"), "Cosmic Infrared Shift")
            ),
            panel{}
         );

      popup.menu(hsize(300, menu));

      return popup;
   }
}

TEST_CASE("menus page: example", "[menus_page]")
{
   test_view tv{extent{340, 250}, 2};
   open(tv);
   auto& view_ = tv.view_;
   view_.content(
      margin({20, 20, 20, 20},
         align_left_top(hsize(200, make_popup_menu("Dropdown Menu", menu_position::bottom_right, view_)))),
      box(bkd_color)
   );
   view_.layout();
   tv.draw();

   // Open it, and rest on its second item.
   click(view_, {60, 32});
   wait(view_); wait(view_);
   view_.layout();
   tv.draw();
   auto m = menu_of(view_);
   REQUIRE(m);
   auto b = m->bounds();
   view_.cursor({b.left + 60, b.top + 45}, cursor_tracking::hovering);
   wait(view_);
   tv.draw();
   tv.img.save_png(std::string{RESULTS_PATH} + "menus_example.png");
}
#endif
