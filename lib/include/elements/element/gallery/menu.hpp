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

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Popup Button
   ////////////////////////////////////////////////////////////////////////////
   basic_menu button_menu(
      std::string const& text
    , menu_position pos = menu_position::bottom_right
    , color body_color = get_theme().default_button_color
   );

   basic_menu button_menu(
      menu_position pos = menu_position::bottom_right
    , color body_color = get_theme().default_button_color
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
   inline auto menu_item_text(std::string const& text)
   {
      return xside_margin({ 20, 20 }, align_left(label(text)));
   }

   inline auto menu_item(std::string const& text)
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
   template <typename First, typename... Rest>
   inline std::pair<basic_menu, std::shared_ptr<label>>
   selection_menu(
      std::function<void(std::string const& item)> on_select
    , First&& first, Rest&&... rest
   )
   {
      auto btn_text = share(label(std::forward<First>(first), 1.0));

      auto menu_btn = text_button<basic_menu>(
         margin(
            button_margin,
            htile(
               align_left(hold(btn_text)),
               align_right(left_margin(12, icon(icons::down_dir, 1.0)))
            )
         )
      );

      menu_btn.position(menu_position::bottom_right);

      auto&& make_menu_item = [=](auto&& text)
      {
         auto e = menu_item(std::forward<decltype(text)>(text));
         e.on_click = [=]()
         {
            btn_text->text(text);
            on_select(text);
         };
         return e;
      };

      auto menu =
         layer(
            vtile(
               make_menu_item(std::forward<First>(first)),
               make_menu_item(std::forward<Rest>(rest))...
            ),
            menu_background{}
         );

      menu_btn.menu(menu);

      return { std::move(menu_btn), btn_text };
   }
}}

#endif
