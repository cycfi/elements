/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/styles/menus.adoc and
   draws its figure. The figure lands in the build's results directory;
   copy it to docs/modules/ROOT/images/styles/ to update the page.
=============================================================================*/
#include "test_support.hpp"
#include <chrono>
#include <string>
#include <thread>
#include <vector>

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

   void wait(view& v)
   {
      std::this_thread::sleep_for(1ms);
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

   std::shared_ptr<basic_popup_menu_element> menu_of(view& v)
   {
      return std::dynamic_pointer_cast<basic_popup_menu_element>(v.layers().back());
   }
}

TEST_CASE("menus style page: display_shortcut", "[menus_style_page]")
{
#if defined(__APPLE__)
   CHECK(display_shortcut(key_code::f, mod_action) == std::pair<std::string, std::string>{"\u2318", "F"});
   CHECK(display_shortcut(key_code::c, mod_action | mod_shift).first == "\u21e7\u2318");
#else
   CHECK(display_shortcut(key_code::f, mod_action) == std::pair<std::string, std::string>{"Ctrl+", "F"});
   CHECK(display_shortcut(key_code::c, mod_action | mod_shift).first == "Ctrl+Shift+");
#endif
   // A shifted punctuation key shows the character it makes.
   CHECK(display_shortcut(key_code::equal, mod_shift).second == "+");
}

TEST_CASE("menus style page: menu buttons", "[menus_style_page]")
{
   // The page's snippet.
   auto file = button_menu("File");
   file.menu(
      layer(
         vtile(
            menu_item("Open", shortcut_key{key_code::o, mod_action}),
            menu_item("Save", shortcut_key{key_code::s, mod_action}),
            menu_item_spacer(),
            menu_item("Close")
         ),
         menu_background{}
      )
   );
   CHECK(file.position() == menu_position::bottom_right);

   // The item keeps its shortcut.
   auto save = menu_item("Save", shortcut_key{key_code::s, mod_action});
   CHECK(save.shortcut.key == key_code::s);
}

TEST_CASE("menus style page: selection menu", "[menus_style_page]")
{
   test_view tv{extent{400, 300}};
   open(tv);
   std::string picked;
   std::vector<std::string> items = {"Small", "Medium", "Large"};
   auto [btn, text] = selection_menu(
      [&](std::string_view s) { picked = std::string(s); }, items);

   // It starts with the first item.
   CHECK(text->get_text() == "Small");

   auto b = share(btn);
   tv.view_.content(margin({20, 20, 20, 20}, align_left_top(hsize(150, hold(b)))),
      box(bkd_color));
   tv.view_.layout();
   tv.draw();
   click(tv.view_, {60, 30});
   wait(tv.view_); wait(tv.view_);
   REQUIRE(menu_of(tv.view_));

   // Down twice, then Enter: Medium.
   tv.view_.key({key_code::down, key_action::press, 0});
   tv.view_.key({key_code::down, key_action::press, 0});
   tv.view_.key({key_code::enter, key_action::press, 0});
   wait(tv.view_); wait(tv.view_);
   CHECK(picked == "Medium");
   CHECK(text->get_text() == "Medium");
}

TEST_CASE("menus style page: combo box", "[menus_style_page]")
{
   test_view tv{extent{400, 300}};
   open(tv);
   auto [combo, text] = combo_box({"Small", "Medium", "Large"}, "Size");
   std::string entered;
   text->on_enter = [&](std::string_view s)
   {
      entered = std::string(s);
      return true;
   };

   // Empty, showing the placeholder.
   CHECK(text->get_text().empty());

   auto c = share(std::move(combo));
   tv.view_.content(margin({20, 20, 20, 20}, align_left_top(hsize(150, hold(c)))),
      box(bkd_color));
   tv.view_.layout();
   tv.draw();

   // Typing completes: m fills in Medium, its rest selected; the right
   // arrow takes it.
   click(tv.view_, {50, 32});
   wait(tv.view_);
   tv.view_.text(text_info{'m', 0});
   CHECK(text->get_utf8() == "Medium");
   CHECK(text->select_start() == 1);
   CHECK(text->select_end() == 6);
   tv.view_.key({key_code::right, key_action::press, 0});
   CHECK(text->select_start() == 6);

   // A pick puts the item in and calls on_enter with it.
   click(tv.view_, {160, 32});
   wait(tv.view_); wait(tv.view_);
   REQUIRE(menu_of(tv.view_));
   tv.view_.key({key_code::down, key_action::press, 0});
   tv.view_.key({key_code::down, key_action::press, 0});
   tv.view_.key({key_code::down, key_action::press, 0});
   tv.view_.key({key_code::enter, key_action::press, 0});
   wait(tv.view_); wait(tv.view_);
   CHECK(text->get_utf8() == "Large");
   CHECK(entered == "Large");
}

#if !defined(ARTIST_RECORDING)
namespace
{
   // The page's example: examples/menus, verbatim.
   auto make_selection_menu()
   {
      return selection_menu(
         [](std::string_view select)
         {
            // This will be called when an item is selected
         },
         {
            "Quantum Feedback Loop",
            "Psionic Wave Oscillator",
            "Gaia Abiogenesis",
            "Chaotic Synchronicity",
            "Omega Quadrant",
            "Photonic Mesh",
            "Antimatter Soup",
            "Dark Beta Quarks",
            "Cosmic Infrared Shift"
         }

      ).first; // We return only the first, the menu. the second is a shared pointer to the label.
   }
}

TEST_CASE("menus style page: example", "[menus_style_page]")
{
   test_view tv{extent{340, 300}, 2};
   open(tv);
   auto& view_ = tv.view_;
   view_.content(
      margin({20, 20, 20, 20}, align_left_top(hsize(300, make_selection_menu()))),
      box(bkd_color)
   );
   view_.layout();
   tv.draw();
   click(view_, {100, 32});
   wait(view_); wait(view_);
   view_.layout();
   tv.draw();
   auto m = menu_of(view_);
   REQUIRE(m);
   auto mb = m->bounds();
   view_.cursor({mb.left + 60, mb.top + 85}, cursor_tracking::hovering);
   wait(view_);
   tv.draw();
   tv.img.save_png(std::string{RESULTS_PATH} + "menus_selection.png");
}

namespace
{
   // The page's second example: examples/menus, verbatim.
   char const* sayings[] =
   {
      "Seeker, look within",
      "Empower yourself",
      "Have you found your circuit?",
      "Complexity to the next level",
      "Ultra-angelic consciousness",
      "Indigo Child",
      "Vector of synchronicity",
      "Aspiration is a constant",
      "Strange Soup",
      "Nonchalant Slave",
      "Shiva will amplify your mind",
      "This mission never ends",
      "The future is now",
      "Who are we?",
      "One cannot self-actualize",
      "Ennobling source of stardust"
   };

   auto make_combo_box()
   {
      auto [combo, text] = combo_box(
         {std::begin(sayings), std::end(sayings)}, "Your saying");
      text->on_enter = [](std::string_view saying)
      {
         // This will be called when a saying is entered or picked
         return true;
      };
      return combo;
   }
}

TEST_CASE("menus style page: combo box example", "[menus_style_page]")
{
   test_view tv{extent{340, 380}, 2};
   open(tv);
   auto& view_ = tv.view_;
   auto [combo, text] = combo_box(
      {std::begin(sayings), std::end(sayings)}, "Your saying");
   text->set_text("Indigo Child");
   view_.content(
      margin({20, 20, 20, 20}, align_left_top(hsize(300, combo))),
      box(bkd_color)
   );
   view_.layout();
   tv.draw();

   // The arrow, at the right of the frame, opens the list.
   click(view_, {305, 32});
   wait(view_); wait(view_);
   view_.layout();
   tv.draw();
   auto m = menu_of(view_);
   REQUIRE(m);
   auto mb = m->bounds();
   // Strange Soup, the ninth of sixteen
   auto item_height = mb.height() / 16;
   view_.cursor({mb.left + 60, mb.top + item_height * 8.5f},
      cursor_tracking::hovering);
   wait(view_);
   tv.draw();
   tv.img.save_png(std::string{RESULTS_PATH} + "menus_combo_box.png");

   // The page's example builds.
   auto example = share(make_combo_box());
   CHECK(example);
}
#endif
