/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_PHOTON_GUI_LIB_WIDGET_MENU_JUNE_4_2016)
#define CYCFI_PHOTON_GUI_LIB_WIDGET_MENU_JUNE_4_2016

#include <photon/element/element.hpp>
#include <photon/element/button.hpp>
#include <photon/element/popup.hpp>
#include <photon/view.hpp>

namespace cycfi { namespace photon
{
   ////////////////////////////////////////////////////////////////////////////
   // Basic Dropdown Menu
   ////////////////////////////////////////////////////////////////////////////
   class basic_dropdown_menu : public basic_button
   {
   public:
                              template <typename W1, typename W2>
                              basic_dropdown_menu(W1&& off, W2&& on);

      virtual void            layout(context const& ctx);
      virtual element*        click(context const& ctx, mouse_button btn);
      virtual void            drag(context const& ctx, mouse_button btn);
      virtual bool            key(context const& ctx, key_info k);
      virtual bool            focus(focus_request r);

                              template <typename Menu>
      void                    menu(Menu&& menu_);

   private:

      using popup_ptr = std::shared_ptr<basic_popup_element>;

      popup_ptr               _popup;
   };

   template <typename W1, typename W2>
   inline basic_dropdown_menu::basic_dropdown_menu(W1&& off, W2&& on)
    : basic_button(std::forward<W1>(off), std::forward<W2>(on))
   {}

   template <typename Menu>
   inline void basic_dropdown_menu::menu(Menu&& menu_)
   {
      _popup = std::dynamic_pointer_cast<basic_popup_element>(share(basic_popup(menu_)));
   }

   ////////////////////////////////////////////////////////////////////////////
   // Menu Items
   ////////////////////////////////////////////////////////////////////////////
   class basic_menu_item_element : public proxy_base
   {
   public:

      using menu_item_function = std::function<void()>;

      virtual void            draw(context const& ctx);
      virtual element*        hit_test(context const& ctx, point p);
      virtual element*        click(context const& ctx, mouse_button btn);
      virtual bool            cursor(context const& ctx, point p, cursor_tracking status);
      virtual bool            is_control() const;

      menu_item_function      on_click;
   };

   template <typename Subject>
   inline proxy<Subject, basic_menu_item_element>
   basic_menu_item(Subject&& subject)
   {
      return { std::forward<Subject>(subject) };
   }
}}

#endif
