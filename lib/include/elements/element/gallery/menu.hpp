/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_GALLERY_MENU_JUNE_5_2016)
#define ELEMENTS_GALLERY_MENU_JUNE_5_2016

#include <elements/element/indirect.hpp>
#include <elements/element/menu.hpp>
#include <elements/support/theme.hpp>
#include <elements/element/gallery/button.hpp>
#include <string_view>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Popup Button
   ////////////////////////////////////////////////////////////////////////////
   basic_menu button_menu(
      std::string_view text
    , menu_position pos = menu_position::bottom_right
    , color body_color = get_theme().default_button_color
   );

   basic_menu button_menu(
      menu_position pos = menu_position::bottom_right
    , color body_color = get_theme().default_button_color
   );

   basic_menu icon_menu(
      uint32_t code
    , float size
    , menu_position pos = menu_position::bottom_right
   );

   ////////////////////////////////////////////////////////////////////////////
   // Menu Background
   ////////////////////////////////////////////////////////////////////////////
   struct menu_background : element
   {
      virtual void            draw(context const& ctx);
   };

   ////////////////////////////////////////////////////////////////////////////
   // Menu Items
   ////////////////////////////////////////////////////////////////////////////
   inline auto menu_item_text(std::string_view text)
   {
      return xside_margin({ 20, 20 }, align_left(label(text)));
   }

   inline auto menu_item(std::string_view text)
   {
      return basic_menu_item(menu_item_text(text));
   }

   struct menu_item_spacer_element : public element
   {
      virtual view_limits  limits(basic_context const& ctx) const;
      virtual void         draw(context const& ctx);
   };

   inline auto menu_item_spacer()
   {
      return menu_item_spacer_element{};
   }

   ////////////////////////////////////////////////////////////////////////////
   // Selection Menu
   ////////////////////////////////////////////////////////////////////////////
   std::pair<basic_menu, std::shared_ptr<label>>
   selection_menu(
      std::function<void(std::string_view item)> on_select
    , std::string_view init
   );

   std::pair<basic_menu, std::shared_ptr<label>>
   selection_menu(
      std::function<void(std::string_view item)> on_select
    , std::size_t num_items, std::string_view items[]
   );

   template <int N>
   inline std::pair<basic_menu, std::shared_ptr<label>>
   selection_menu(
      std::function<void(std::string_view item)> on_select
    , std::string_view (&items)[N]
   )
   {
      return selection_menu(on_select, N, items);
   }
}}

#endif
