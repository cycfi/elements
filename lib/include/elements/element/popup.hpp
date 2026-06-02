/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_POPUP_JUNE_4_2016)
#define ELEMENTS_POPUP_JUNE_4_2016

#include <elements/element/floating.hpp>
#include <elements/view.hpp>
#include <infra/support.hpp>

namespace cycfi::elements
{
   class basic_button_menu;

   /**
    * \class basic_popup_element
    *
    * \brief
    *    A basic_popup_element is a component in a UI framework that displays
    *    content in a floating layer above other UI elements. It inherits
    *    from floating_element, allowing it to overlay above other elements.
    */
   class basic_popup_element : public floating_element
   {
   public:

      using cursor_function = std::function<void(point p, cursor_tracking status)>;

                              basic_popup_element(rect bounds = {})
                               : floating_element(bounds)
                              {}

      bool                    wants_control() const override { return true; }
      element*                hit_test(context const& ctx, point p, bool leaf, bool control) override;
      bool                    cursor(context const& ctx, point p, cursor_tracking status) override;

      bool                    is_open(view& view_) const;
      virtual void            open(view& view_);
      virtual  void           close(view& view_);

      cursor_function         on_cursor = [](auto, auto){};
   };

   /**
    * \brief
    *    Wraps a given UI element in a basic_popup_element, allowing it to be
    *    displayed as a popup. This function template creates a proxy object
    *    that encapsulates the UI element, providing it with popup
    *    functionality.
    *
    * @tparam Subject
    *    The type of the UI element to be wrapped.
    *
    * @param subject
    *    The UI element to wrap.
    *
    * @param bounds
    *    Optional bounds for the popup.
    *
    * @return
    *    A proxy object that wraps the specified UI element in a
    *    basic_popup_element.
    */
   template <concepts::Element Subject>
   inline proxy<remove_cvref_t<Subject>, basic_popup_element>
   basic_popup(Subject&& subject, rect bounds = {})
   {
      return {std::forward<Subject>(subject), bounds};
   }

   /**
    * \class basic_popup_menu_element
    *
    *    The `basic_popup_menu_element` class extends `basic_popup_element`
    *    to provide functionality specific to popup menus in a UI framework.
    *    It is designed to display a menu in a floating layer above other UI
    *    elements, allowing for interaction with menu items.
    *
    *    It also includes methods to associate a `basic_button_menu` with the
    *    popup menu, enabling interaction between the menu button and the
    *    popup menu itself.
    */
   class basic_popup_menu_element : public basic_popup_element
   {
   public:

      using basic_popup_element::basic_popup_element;

      element*                hit_test(context const& ctx, point p, bool leaf, bool control) override;
      bool                    click(context const& ctx, mouse_button btn) override;
      void                    close(view& view_) override;

      basic_button_menu*      menu_button();
      void                    menu_button(basic_button_menu* p);

   private:

      basic_button_menu*      _menu_button = nullptr;
   };

   /**
    * @brief
    *    Gets the menu button associated with this popup menu.
    *
    * @return
    *    A pointer to the associated `basic_button_menu`, or nullptr if no
    *    button is associated.
    */
   inline basic_button_menu* basic_popup_menu_element::menu_button()
   {
      return _menu_button;
   }

   /**
    * @brief
    *    Associates a menu button with this popup menu.
    *
    * @param p
    *    A pointer to the `basic_button_menu` to associate with this popup
    *    menu. Passing nullptr dissociates any currently associated menu
    *    button.
    */
   inline void basic_popup_menu_element::menu_button(basic_button_menu* p)
   {
      _menu_button = p;
   }

   /**
    * @brief
    *    Creates a proxy object for displaying a UI element as a popup menu.
    *
    *    This template function wraps a given UI element, allowing it to be
    *    displayed as a popup menu. It returns a proxy object that
    *    encapsulates the specified UI element, providing it with popup menu
    *    functionality.
    *
    * @tparam
    *    Subject The type of the UI element to be wrapped. Must meet the
    *    requirements of the `concepts::Element` concept.
    *
    * @param subject
    *    The UI element to wrap in the popup menu. This element will be
    *    displayed as the content of the popup menu.
    *
    * @param bounds
    *    Optional. The rectangular bounds of the popup menu. If not
    *    specified, the bounds will be determined based on the content and
    *    context.
    *
    * @return
    *    A proxy object that wraps the specified UI element in a
    *    `basic_popup_menu_element`.
    */
   template <concepts::Element Subject>
   inline proxy<remove_cvref_t<Subject>, basic_popup_menu_element>
   basic_popup_menu(Subject&& subject, rect bounds = {})
   {
      return {std::forward<Subject>(subject), bounds};
   }
}

#endif
