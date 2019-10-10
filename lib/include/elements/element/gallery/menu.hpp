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

   inline auto menu_item_text(std::string_view text, shortcut_key shortcut)
   {
      return xside_margin({ 20, 20 },
         htile(
            align_left(label(text))
          , align_right(label(diplay(shortcut.key, shortcut.modifiers), get_theme().system_font))
         )
      );
   }

   inline auto menu_item(std::string_view text)
   {
      return basic_menu_item(menu_item_text(text));
   }

   inline auto menu_item(std::string_view text, shortcut_key shortcut)
   {
      auto r = basic_menu_item(menu_item_text(text, shortcut));
      r.shortcut = shortcut;
      return r;
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
   struct menu_selector
   {
      virtual std::size_t        size() const = 0;
      virtual std::string_view   operator[](std::size_t index) const = 0;
   };

   std::pair<basic_menu, std::shared_ptr<label>>
   selection_menu(
      std::function<void(std::string_view item)> on_select
    , std::string_view init
   );

   std::pair<basic_menu, std::shared_ptr<label>>
   selection_menu(
      std::function<void(std::string_view item)> on_select
    , menu_selector const& items
   );

   template <typename Sequence>
   inline std::pair<basic_menu, std::shared_ptr<label>>
   selection_menu(
      std::function<void(std::string_view item)> on_select
    , Sequence const& seq
    , typename std::enable_if<!std::is_base_of<menu_selector, Sequence>::value>::type* = 0
   )
   {
      struct seq_menu_selector : menu_selector
      {
         seq_menu_selector(Sequence const& seq_)
          : _seq(seq_)
         {}

         virtual std::size_t
         size() const
         {
            return std::size(_seq);
         }

         virtual std::string_view
         operator[](std::size_t index) const
         {
            return _seq[index];
         }

         Sequence const& _seq;
      };

      return selection_menu(on_select, seq_menu_selector{ seq });
   }

   template <typename T>
   std::pair<basic_menu, std::shared_ptr<label>>
   selection_menu(
      std::function<void(std::string_view item)> on_select
    , std::initializer_list<T> const& list
   )
   {
      struct init_list_menu_selector : menu_selector
      {
         init_list_menu_selector(std::initializer_list<T> const& list_)
          : _list(list_)
         {}

         virtual std::size_t
         size() const
         {
            return _list.size();
         }

         virtual std::string_view
         operator[](std::size_t index) const
         {
            return *(_list.begin()+index);
         }

         std::initializer_list<T> const& _list;
      };

      return selection_menu(on_select, init_list_menu_selector{ list });
   }
}}

#endif
