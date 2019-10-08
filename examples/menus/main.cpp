/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>
#include <algorithm>
#include <random>

using namespace cycfi::elements;

// Main window background color
auto constexpr bkd_color = rgba(35, 35, 37, 255);
auto background = box(bkd_color);

auto make_selection_menu()
{
   static std::string_view items[] =
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
   };

   return selection_menu(
      [](std::string_view select)
      {
         // This will be called when an item is selected
      },
      items
   ).first; // We return only the first, the menu. the second is a shared pointer to the label.
}

auto make_popup_menu(char const* title, menu_position pos)
{
   auto popup  = button_menu(title, pos);

   auto menu =
      layer(
         vtile(
            menu_item("Quantum Feedback Loop"),
            menu_item("Psionic Wave Oscillator"),
            menu_item("Gaia Abiogenesis"),
            menu_item_spacer(),
            menu_item("Chaotic Synchronicity"),
            menu_item("Omega Quadrant"),
            menu_item("Photonic Mesh"),
            menu_item("Antimatter Soup"),
            menu_item("Dark Beta Quarks"),
            menu_item("Cosmic Infrared Shift")
         ),
         menu_background{}
      );

   popup.menu(menu);

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
               menu_background{}
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
      margin({ 20, 0, 20, 20 },
         vtile(
            hmin_size(300, make_selection_menu()),
            top_margin(20, make_popup_menu("Dropdown Menu", menu_position::bottom_right)),
            top_margin(20, make_dynamic_menu("Dynamic Menu", menu_position::bottom_right)),
            top_margin(20, scroller(image{ "deep_space.jpg" })),
            top_margin(20, make_popup_menu("Dropup Menu", menu_position::top_right))
         )
      );
}

auto make_controls(view& view_)
{
   return margin({ 20, 20, 20, 20 }, pane("Menus", make_menus(view_)));
}

int main(int argc, const char* argv[])
{
   app _app(argc, argv);
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   view_.content(
      {
         share(make_controls(view_)),
         share(background)
      }
   );

   _app.run();
   return 0;
}
