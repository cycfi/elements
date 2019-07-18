/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_ELEMENTS_GUI_LIB_WIDGET_MENU_JUNE_4_2016)
#define CYCFI_ELEMENTS_GUI_LIB_WIDGET_MENU_JUNE_4_2016

#include <elements/element/element.hpp>
#include <elements/element/button.hpp>
#include <elements/element/popup.hpp>
#include <elements/view.hpp>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Basic Dropdown Menu
   ////////////////////////////////////////////////////////////////////////////
   enum class menu_position
   {
      top_left,
      top_right,
      bottom_left,
      bottom_right
   };

   class basic_menu : public layered_button
   {
   public:
                              template <typename W1, typename W2>
                              basic_menu(
                                 W1&& off, W2&& on
                               , menu_position pos = menu_position::bottom_right);

      virtual void            layout(context const& ctx);
      virtual element*        click(context const& ctx, mouse_button btn);
      virtual void            drag(context const& ctx, mouse_button btn);
      virtual bool            key(context const& ctx, key_info k);
      virtual bool            focus(focus_request r);

      menu_position           position() const              { return _position; }
      void                    position(menu_position pos)   { _position = pos; }

                              template <typename Menu>
      void                    menu(Menu&& menu_);

   private:

      using popup_ptr = std::shared_ptr<basic_popup_element>;

      popup_ptr               _popup;
      menu_position           _position;
   };

   template <typename W1, typename W2>
   inline basic_menu::basic_menu(W1&& off, W2&& on, menu_position pos)
    : layered_button(std::forward<W1>(off), std::forward<W2>(on))
    , _position(pos)
   {}

   template <typename Menu>
   inline void basic_menu::menu(Menu&& menu_)
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
