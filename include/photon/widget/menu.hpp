/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_MENU_JUNE_4_2016)
#define PHOTON_GUI_LIB_WIDGET_MENU_JUNE_4_2016

#include <photon/widget/widget.hpp>
#include <photon/widget/button.hpp>
#include <photon/widget/popup.hpp>
#include <photon/view.hpp>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Dropdown Menu
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class basic_dropdown_menu : public basic_button
   {
   public:
                              template <typename W1, typename W2>
                              basic_dropdown_menu(W1&& off, W2&& on);

      virtual void            layout(context const& ctx);
      virtual widget*         click(context const& ctx, mouse_button btn);
      virtual void            drag(context const& ctx, mouse_button btn);
//      virtual bool            key(context const& ctx, key_info const& k);
      virtual bool            focus(focus_request r);

                              template <typename Menu>
      void                    menu(Menu&& menu_);

   private:

      basic_popup_widget&     popup() const;

      widget_ptr              _menu;
   };

   template <typename W1, typename W2>
   inline basic_dropdown_menu::basic_dropdown_menu(W1&& off, W2&& on)
    : basic_button(std::forward<W1>(off), std::forward<W2>(on))
   {}

   template <typename Menu>
   inline void basic_dropdown_menu::menu(Menu&& menu_)
   {
      _menu = share(basic_popup(menu_));
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Menu Items
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class basic_menu_item_widget : public proxy_base
   {
   public:

      using menu_item_function = std::function<void()>;

      virtual void            draw(context const& ctx);
      virtual widget*         hit_test(context const& ctx, point p);
      virtual widget*         click(context const& ctx, mouse_button btn);
      virtual bool            cursor(context const& ctx, point p, cursor_tracking status);
      virtual bool            is_control() const;

      menu_item_function      on_click;
   };

   template <typename Subject>
   inline proxy<typename std::decay<Subject>::type, basic_menu_item_widget>
   basic_menu_item(Subject&& subject)
   {
      return { std::forward<Subject>(subject) };
   }
}

#endif
