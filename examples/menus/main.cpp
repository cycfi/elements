/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>
#include <algorithm>
#include <random>
#include <string>

using namespace cycfi::elements;
using cycfi::artist::rgba;

namespace
{
   // Main window background color
   auto constexpr bkd_color = rgba(35, 35, 37, 255);
   auto background = box(bkd_color);

   auto make_selection_menu()
   {
      // Only the first of the pair, the menu, is used. The second is a
      // shared pointer to its label.
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
      ).first;
   }

   // The dynamic menu's items, and the combo box's
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

   // A combo box: type a saying of your own, or pick one. Typing
   // completes from the list.
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

   // Show which item was chosen, by click or by its shortcut
   void alert(view& view_, std::string text)
   {
      auto msg = message_box1(view_, std::move(text), icons::info, []{});
      open_popup(msg, view_);
   }

   // A menu item that just says it was chosen. The shortcut is optional.
   template <typename... Shortcut>
   auto chosen_item(view& view_, char const* text, Shortcut... shortcut)
   {
      auto item = menu_item(text, shortcut...);
      item.on_click = [&view_, text]() { alert(view_, text); };
      return item;
   }

   auto make_popup_menu(char const* title, menu_position pos, view& view_)
   {
      auto skf = shortcut_key{key_code::f, mod_action};
      auto skp = shortcut_key{key_code::p, mod_action};
      auto quantum_feedback = menu_item("Quantum Feedback Loop", skf);
      auto psionic_wave = menu_item("Psionic Wave Oscillator", skp);
      auto photonic_mesh = chosen_item(view_, "Photonic Mesh");
      auto antimatter_soup = chosen_item(view_, "Antimatter Soup");

      // Here's how you dynamically enable or disable menu items:
      // Just give it a function that returns the enabled state.
      static bool enable = false; // just for demo; avoid statics!
      photonic_mesh.is_enabled   = []{ return enable; };
      antimatter_soup.is_enabled = []{ return enable; };

      // Here's how you attach functions that will be called when a
      // menu item is selected:
      quantum_feedback.on_click =
         [&view_]()
         {
            enable = true;
            alert(view_, "Quantum Feedback Loop: "
               "Photonic Mesh and Antimatter Soup enabled");
         };
      psionic_wave.on_click =
         [&view_]()
         {
            enable = false;
            alert(view_, "Psionic Wave Oscillator: "
               "Photonic Mesh and Antimatter Soup disabled");
         };

      auto sk1 = shortcut_key{key_code::g, mod_action};
      auto sk2 = shortcut_key{key_code::c, mod_action+mod_shift};
      auto sk3 = shortcut_key{key_code::b, mod_action+mod_alt};

      // The rest just say they were chosen
      auto menu =
         layer(
            vtile(
               photonic_mesh,
               quantum_feedback,
               psionic_wave,
               chosen_item(view_, "Gaia Abiogenesis", sk1),
               menu_item_spacer(),
               chosen_item(view_, "Chaotic Synchronicity", sk2),
               chosen_item(view_, "Omega Quadrant"),
               antimatter_soup,
               chosen_item(view_, "Dark Beta Quarks", sk3),
               chosen_item(view_, "Cosmic Infrared Shift")
            ),
            panel{}
         );

      auto popup = button_menu(title, pos);
      popup.menu(hsize(300, menu));
      return popup;
   }

   auto make_dynamic_menu(char const* title, menu_position pos)
   {
      auto popup = button_menu(title, pos);

      // This will be called every time the menu button is clicked
      // We will simulate dynamic creation of the menu items here.
      auto populate_menu =
         [](auto& menu_button)
         {
            // Dynamically generate your menu here
            vtile_composite list;
            for (auto item : sayings)
               list.push_back(share(menu_item(item)));

            // Shuffle the items just for kicks
            std::random_device rd;
            std::mt19937 g(rd());
            std::shuffle(list.begin(), list.end(), g);

            // Show only a portion of the menu, providing a scroller
            // to allow scrolling over all the items.
            auto menu =
               layer(
                  vsize(150, vscroller(list)),
                  panel{}
               );

            // Install the menu
            menu_button.menu(menu);
         };

      // Populate menu dynamically
      popup.on_open_menu = populate_menu;
      return popup;
   }

   // A menu bar: menu items in a row, each with a menu of its own. In
   // File, Open Recent has one too: a submenu, opened to its right.
   auto make_menu_bar(view& view_)
   {
      auto recent = menu_item("Open Recent");
      recent.menu(
         layer(
            vtile(
               chosen_item(view_, "Nebula.elm"),
               chosen_item(view_, "Pulsar.elm"),
               chosen_item(view_, "Quasar.elm")
            ),
            panel{}
         )
      );

      auto file = menu_item("File");
      file.menu(
         hsize(220,
            layer(
               vtile(
                  chosen_item(view_, "New",
                     shortcut_key{key_code::n, mod_action}),
                  chosen_item(view_, "Open...",
                     shortcut_key{key_code::o, mod_action}),
                  recent,
                  menu_item_spacer(),
                  chosen_item(view_, "Save",
                     shortcut_key{key_code::s, mod_action})
               ),
               panel{}
            )
         )
      );

      auto edit = menu_item("Edit");
      edit.menu(
         hsize(220,
            layer(
               vtile(
                  chosen_item(view_, "Cut",
                     shortcut_key{key_code::x, mod_action}),
                  chosen_item(view_, "Copy",
                     shortcut_key{key_code::c, mod_action}),
                  chosen_item(view_, "Paste",
                     shortcut_key{key_code::v, mod_action})
               ),
               panel{}
            )
         )
      );

      auto help = menu_item("Help");
      help.menu(layer(chosen_item(view_, "About Menus"), panel{}));

      // The titles keep their own widths; the empty element takes the
      // rest of the bar.
      return layer(
         htile(hmin(file), hmin(edit), hmin(help), element{}),
         menu_background{}
      );
   }

   auto make_menus(view& view_)
   {
      auto const down = menu_position::bottom_right;
      auto const up = menu_position::top_right;
      return
         margin({20, 0, 20, 20},
            vtile(
               hmin_size(300, make_selection_menu()),
               margin_top(20, hmin_size(300, make_combo_box())),
               margin_top(20, make_popup_menu("Dropdown Menu", down, view_)),
               margin_top(20, make_dynamic_menu("Dynamic Menu", down)),
               margin_top(20, scroller(image{"deep_space.jpg"})),
               margin_top(20, make_popup_menu("Dropup Menu", up, view_))
            )
         );
   }

   auto make_controls(view& view_)
   {
      return margin({20, 20, 20, 20}, make_menus(view_));
   }
}

int main(int argc, char* argv[])
{
   app _app("Menus");

   // Set up the fonts and search paths once, at startup, so the
   // first window does not pay for it.
   init_resources();
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   view_.content(
      vtile(
         make_menu_bar(view_),
         make_controls(view_)
      ),
      background
   );

   _app.run();
   return 0;
}
