/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>
#include <algorithm>
#include <random>

using namespace cycfi::elements;
using cycfi::artist::rgba;

// Main window background color
auto constexpr bkd_color = rgba(35, 35, 37, 255);
auto background = box(bkd_color);

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

auto make_dynamic_menu(char const* title, menu_position pos)
{
   auto popup  = button_menu(title, pos);

   // This will be called every time the menu button is clicked
   // We will simulate dynamic creation of the menu items here.
   auto populate_menu =
      [](auto& popup)
      {
         // Sample menu
         char const* items[] =
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

         // Dynamically generate your menu here
         vtile_composite list;
         for (auto item : items)
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
         popup.menu(menu);
      };

   // Populate menu dynamically
   popup.on_open_menu = populate_menu;
   return popup;
}

auto make_menus(view& view_)
{
   return
      margin({20, 0, 20, 20},
         vtile(
            hmin_size(300, make_selection_menu()),
            margin_top(20, make_popup_menu("Dropdown Menu", menu_position::bottom_right, view_)),
            margin_top(20, make_dynamic_menu("Dynamic Menu", menu_position::bottom_right)),
            margin_top(20, scroller(image{"deep_space.jpg"})),
            margin_top(20, make_popup_menu("Dropup Menu", menu_position::top_right, view_))
         )
      );
}

auto make_controls(view& view_)
{
   return margin({20, 20, 20, 20}, make_menus(view_));
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
      make_controls(view_),
      background
   );

   _app.run();
   return 0;
}
