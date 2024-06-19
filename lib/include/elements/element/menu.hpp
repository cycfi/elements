/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_MENU_JUNE_4_2016)
#define ELEMENTS_MENU_JUNE_4_2016

#include <elements/element/element.hpp>
#include <elements/element/button.hpp>
#include <elements/element/popup.hpp>
#include <elements/element/selection.hpp>
#include <elements/view.hpp>
#include <infra/support.hpp>

namespace cycfi::elements
{
   /**
    * \enum menu_position
    * \brief
    *    Specifies the menu position relative to the button.
    */
   enum class menu_position
   {
      top_left,
      top_right,
      bottom_left,
      bottom_right
   };

   /**
    * \class basic_button_menu
    * \brief
    *    A button that can open a menu.
    *
    *    Extends basic_button to provide functionality for opening a menu.
    *    The position of the menu relative to the button can be specified.
    *
    * \var on_open_menu
    *    Callback function to be called when the menu is opened.
    */
   class basic_button_menu : public basic_button
   {
   public:

      using on_open_menu_function = std::function<void(basic_button_menu& menu)>;
      using basic_button::focus;

                              basic_button_menu(
                                 menu_position pos = menu_position::bottom_right
                              );

      bool                    click(context const& ctx, mouse_button btn) override;
      void                    drag(context const& ctx, mouse_button btn) override;

      menu_position           position() const;
      void                    position(menu_position pos);

                              template <typename Menu>
      void                    menu(Menu&& menu_);

      on_open_menu_function   on_open_menu;

   private:

      void                    layout_menu(context const& ctx);

      using popup_menu_ptr = std::shared_ptr<basic_popup_menu_element>;

      popup_menu_ptr          _popup;
      menu_position           _position;
   };

   /**
    * \brief
    *    Constructs a basic_button_menu with a specified menu position.
    *
    *    Initializes the button menu with a position that determines where
    *    the menu will appear relative to the button.
    *
    * \param pos
    *    The initial position of the menu relative to the button.
    */
   inline basic_button_menu::basic_button_menu(menu_position pos)
    : _position(pos)
   {}

   /**
    * \brief
    *    Sets the menu to be displayed when the button is activated.
    *
    *    This function allows setting a custom menu for the button. The menu
    *    is created and stored internally as a shared pointer to a
    *    basic_popup_menu.
    *
    * \tparam Menu
    *    The type of the menu to be set.
    *
    * \param menu_
    *    The menu to be associated with the button.
    */
   template <typename Menu>
   inline void basic_button_menu::menu(Menu&& menu_)
   {
      _popup = share(basic_popup_menu(std::forward<Menu>(menu_)));
   }

   /**
    * \brief
    *    Retrieves the current menu position.
    *
    *    Returns the current position of the menu relative to the button.
    *    This position determines where the menu will appear when it is
    *    activated.
    *
    * \return
    *    The current menu position.
    */
   inline menu_position basic_button_menu::position() const
   {
      return _position;
   }

   /**
    * \brief
    *    Sets the menu's position.
    *
    *    Updates the position of the menu relative to the button. This
    *    position determines where the menu will appear when it is activated.
    *
    * \param pos
    *    The new menu position.
    */
   inline void basic_button_menu::position(menu_position pos)
   {
      _position = pos;
   }

   /**
    * \struct shortcut_key
    * \brief
    *    Represents a keyboard shortcut key combination.
    *
    *    Encapsulates a keyboard shortcut, consisting of a key code and
    *    modifier keys. Modifiers are adjusted based on the platform,
    *    automatically including platform-specific modifiers for action
    *    commands.
    */
   struct shortcut_key
   {
                  shortcut_key(key_code key_, int modifiers_);
                  shortcut_key() = default;

      key_code    key = key_code::unknown;
      int         modifiers = 0; // same as modifiers in key_info (see base_view.hpp)
   };

   /**
    * \brief
    *    Constructs a shortcut_key with specified key and modifiers.
    *
    *    Initializes the shortcut with a key code and modifier keys. If the
    *    action modifier is present, it automatically adds the
    *    platform-specific command (macOS) or control (other platforms)
    *    modifier.
    *
    * \param key_
    *    The primary key code of the shortcut.
    *
    * \param modifiers_
    *    Bitwise combination of modifier keys.
    */
   inline shortcut_key:: shortcut_key(key_code key_, int modifiers_)
    : key(key_)
    , modifiers(modifiers_)
   {
      if (modifiers & mod_action)
#if defined(__APPLE__)
         modifiers |= mod_command;
#else
         modifiers |= mod_control;
#endif
   }

   /**
    * \struct basic_menu_item_element_base
    * \brief
    *    Base for menu item elements, enabling polymorphic behavior.
    *
    *    This struct serves as a base for menu item elements, allowing them
    *    to exhibit polymorphic behavior through CRTP (Curiously Recurring
    *    Template Pattern). It delegates the is_enabled check to the derived
    *    class.
    */
   template <typename Derived>
   struct basic_menu_item_element_base : proxy_base
   {
      bool is_enabled() const override;
   };

   /**
    * \brief
    *    Checks if the menu item is enabled.
    *
    *    Delegates the check to the derived class to determine if the menu
    *    item is enabled. This allows derived classes to provide their own
    *    logic for enabling or disabling menu items.
    *
    * \return
    *    True if the menu item is enabled, false otherwise.
    */
   template <typename Derived>
   inline bool basic_menu_item_element_base<Derived>::is_enabled() const
   {
      return static_cast<Derived const*>(this)->is_enabled();
   }

   /**
    * \class basic_menu_item_element
    * \brief
    *    Represents a menu item element with selectable behavior.
    *
    *    This class combines the functionality of a basic menu item element
    *    base with selectable behavior, allowing it to be used as a menu item
    *    that can be selected, clicked, and interacted with via keyboard or
    *    mouse.
    */
   class basic_menu_item_element
    : public basic_menu_item_element_base<basic_menu_item_element>
    , public selectable
   {
   public:

      using menu_item_function = std::function<void()>;
      using menu_enabled_function = std::function<bool()>;

      void                    draw(context const& ctx) override;
      element*                hit_test(context const& ctx, point p, bool leaf, bool control) override;
      bool                    click(context const& ctx, mouse_button btn) override;
      bool                    key(context const& ctx, key_info k) override;
      bool                    cursor(context const& ctx, point p, cursor_tracking status) override;
      bool                    wants_control() const override;
      bool                    wants_focus() const override { return true; }

      menu_enabled_function   is_enabled = []{ return true; };
      menu_item_function      on_click;
      shortcut_key            shortcut;

      bool                    is_selected() const override;
      void                    select(bool state) override;

   private:

      bool                    _selected = false;
   };

   /**
    * \brief
    *    Wraps a subject in a menu item element.
    *
    *    This function template takes a subject and wraps it in a menu item
    *    element, effectively making the subject behave as a selectable menu
    *    item within a menu.
    *
    * \tparam Subject
    *    The type of the subject to be wrapped.
    *
    * \param subject
    *    The subject to be wrapped by the menu item element.
    *
    * \return
    *    A proxy object that acts as a basic_menu_item_element wrapping the
    *    subject, making it selectable and interactive within a menu.
    */
   template <concepts::Element Subject>
   inline proxy<remove_cvref_t<Subject>, basic_menu_item_element>
   basic_menu_item(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }

   /**
    * \brief Checks if the menu item element is selected.
    *
    * \return
    *    True if the element is selected, false otherwise.
    */
   inline bool basic_menu_item_element::is_selected() const
   {
      return _selected;
   }

   /**
    * \brief
    *    Sets the selection state of the menu item element.
    *
    * \param state
    *    The new selection state. True to select the item, false to deselect.
    */
   inline void basic_menu_item_element::select(bool state)
   {
      _selected = state;
   }
}

#endif
