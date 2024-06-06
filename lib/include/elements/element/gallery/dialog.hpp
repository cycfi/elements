/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_GALLERY_DIALOG_OCTOBER_14_2019)
#define ELEMENTS_GALLERY_DIALOG_OCTOBER_14_2019

#include <elements/element/align.hpp>
#include <elements/element/margin.hpp>
#include <elements/element/tile.hpp>
#include <elements/element/misc.hpp>
#include <elements/element/size.hpp>
#include <elements/element/layer.hpp>
#include <elements/element/indirect.hpp>
#include <elements/element/gallery/button.hpp>
#include <elements/support/theme.hpp>

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Dialog creation functions
   ////////////////////////////////////////////////////////////////////////////
   namespace api {}

   template <concepts::Element Content>
   inline auto dialog0(Content&& content);

   template <concepts::Element Content, concepts::ElementPtr BtnPtr>
   inline auto dialog1(
      view& view_
    , Content&& content
    , BtnPtr ok_button
   );

   template <concepts::Element Content, concepts::NullaryFunction F>
   inline auto dialog1(
      view& view_
    , Content&& content
    , F&& on_ok
    , std::string ok_text = "OK"
    , color ok_color = get_theme().indicator_color
   );

   template <concepts::Element Content, concepts::ElementPtr ButtonPtr>
   inline auto dialog2(
      view& view_
    , Content&& content
    , ButtonPtr ok_button
    , ButtonPtr cancel_button
   );

   template <
      concepts::Element Content
    , concepts::NullaryFunction F1
    , concepts::NullaryFunction F2
   >
   inline auto dialog2(
      view& view_
    , Content&& content
    , F1&& on_ok
    , F2&& on_cancel
    , std::string ok_text = "OK"
    , std::string cancel_text = "Cancel"
    , color ok_color = get_theme().indicator_color
   );

   template <concepts::Element Content, concepts::ElementPtr ButtonPtr>
   inline auto dialog2r(
      view& view_
    , Content&& content
    , ButtonPtr ok_button
    , ButtonPtr cancel_button
   );

   template <
      concepts::Element Content
    , concepts::NullaryFunction F1
    , concepts::NullaryFunction F2
   >
   inline auto dialog2r(
      view& view_
    , Content&& content
    , F1&& on_ok
    , F2&& on_cancel
    , std::string ok_text = "OK"
    , std::string cancel_text = "Cancel"
    , color cancel_color = get_theme().indicator_color
   );

   inline void open_popup(element_ptr popup, view& view_);

   ////////////////////////////////////////////////////////////////////////////
   // Inlines
   ////////////////////////////////////////////////////////////////////////////
   namespace inlines {}

   /**
    * \brief
    *    Creates a dialog using the specified content (with no button).
    *
    *    This function constructs a popup dialog with the content passed to
    *    it. The content is wrapped in a modal and aligned at the center. The
    *    `dialog0` function accepts content that satisfies the `Element`
    *    concept. The constructed dialog is then wrapped in a shared pointer
    *    and returned.
    *
    * \tparam Content
    *    Content type for the dialog. Must satisfy `Element`.
    *
    * \param content
    *    Content to be displayed in the dialog.
    *
    * \return
    *    A shared pointer to the constructed dialog element.
    */
   template <concepts::Element Content>
   inline auto dialog0(Content&& content)
   {
      auto popup = share(
         modal(
            align_center_middle(
               layer(
                  std::forward<Content>(content),
                  panel{/*opacity*/0.98}
               )
            )
         )
      );

      return popup;
   }

   namespace detail
   {
      template <concepts::Element Content>
      auto make_dialog_popup(Content&& content)
      {
         return share(
            key_intercept(modal(align_center_middle(
               layer(
                  margin({32, 32, 32, 32},
                     std::forward<Content>(content)
                  ),
                  panel{/*opacity*/0.98}
               )
            )))
         );
      }

      template <concepts::ElementPtr PopupPtr, concepts::ElementPtr ButtonPtr>
      void link_key(PopupPtr popup, ButtonPtr btn)
      {
         popup->on_key =
            [btn = get(btn)](auto k)
            {
               if (k.action == key_action::press && k.key == key_code::enter)
               {
                  if (auto ok = btn.lock())
                  {
                     if (ok->on_click)
                     {
                        ok->on_click(true);
                        return true;
                     }
                  }
               }
               return false;
            };
      }

      template <concepts::ElementPtr PopupPtr, concepts::ElementPtr ButtonPtr>
      void link_key(PopupPtr popup, ButtonPtr btn1, ButtonPtr btn2)
      {
         popup->on_key =
            [btn1 = get(btn1), btn2 = get(btn2)](auto k) -> bool
            {
               if (k.action == key_action::release)
                  return false;

               if (k.action == key_action::press)
               {
                  if (k.key == key_code::enter)
                  {
                     if (auto ok = btn1.lock())
                     {
                        ok->value(true);
                        if (ok->on_click)
                        {
                           ok->on_click(true);
                           return true;
                        }
                     }
                  }
                  else if (k.key == key_code::escape)
                  {
                     if (auto cancel = btn2.lock())
                     {
                        cancel->value(true);
                        if (cancel->on_click)
                        {
                           cancel->on_click(true);
                           return true;
                        }
                     }
                  }
               }
               return false;
            };
      }

      template <
         concepts::ElementPtr PopupPtr
       , concepts::ElementPtr ButtonPtr
       , concepts::ButtonCallback F
      >
      void link_button(view& view_, PopupPtr popup, ButtonPtr btn, F&& f)
      {
         btn->on_click =
            [&view_, eptr = get(popup), f, btn = get(btn)](bool flag)
            {
               f(flag);
               view_.post(
                  [&view_, btn, eptr]()
                  {
                     if (auto b = btn.lock())
                        b->value(false);
                     view_.remove(eptr.lock()); // Close the dialog
                  }
               );
            };
      }

      template <concepts::ElementPtr PopupPtr, concepts::ElementPtr ButtonPtr>
      void link_button(view& view_, PopupPtr popup, ButtonPtr btn)
      {
         link_button(view_, popup, btn, btn->on_click);
      }
   }

   /**
    * \brief
    *    Creates a dialog with specified content and single button.
    *
    *    This function constructs a popup dialog with the content and a
    *    button passed to it. The content and button are vertically aligned,
    *    with the button right-aligned. The button size is set according to
    *    the dialog button size defined in the theme.
    *
    *    * Clicking the 'OK' button or pressing Return/Enter key will trigger
    *      the `ok_button` and close the dialog.
    *
    * Note: Install an `on_click` callback before calling `dialog1`.
    *
    * \tparam Content
    *    Content type for the dialog. Must satisfy `Element`.
    * \tparam BtnPtr
    *    Button type for the dialog. Must satisfy `ElementPtr`.
    *
    * \param view_
    *    Reference to the view where the dialog will be displayed.
    * \param content
    *    Content to be displayed in the dialog.
    * \param ok_button
    *    Dialog button, typically for confirming an action.
    *
    * \return
    *    A shared pointer to the constructed dialog element.
    */
   template <concepts::Element Content, concepts::ElementPtr BtnPtr>
   inline auto dialog1(
      view& view_
    , Content&& content
    , BtnPtr ok_button
   )
   {
      auto button_size = get_theme().dialog_button_size;
      auto popup =
         detail::make_dialog_popup(
            vtile(
               margin_bottom(20, std::forward<Content>(content)),
               align_right(hsize(button_size, hold(ok_button)))
            )
         );

      detail::link_key(popup, ok_button);
      detail::link_button(view_, popup, ok_button);
      return popup;
   }

   /**
    * \brief
    *    Overloaded version of dialog1 function which creates a dialog with
    *    specified content and a preset 'OK' button that triggers a specified
    *    action.
    *
    *    This function constructs a popup dialog with given content and an
    *    'OK' button. When the 'OK' button is clicked, it triggers a
    *    user-defined action (specified by the `on_ok` parameter).
    *
    * \tparam Content
    *    Content type for the dialog. Must satisfy `Element`.
    * \tparam F
    *    Callable type for the 'OK' button click action. Must satisfy
    *    `NullaryFunction`.
    *
    * \param view_
    *    Reference to the view where the dialog will be displayed.
    * \param content
    *    Content to be displayed in the dialog.
    * \param on_ok
    *    A callable object that is invoked when the 'OK' button is clicked.
    * \param ok_text
    *    Text to be displayed on the 'OK' button. Defaults to "OK".
    * \param ok_color
    *    Color of the 'OK' button. Defaults to the color specified in the
    *    theme's `indicator_color`.
    *
    * \return
    *    A shared pointer to the constructed dialog1.
    */
   template <concepts::Element Content, concepts::NullaryFunction F>
   inline auto dialog1(
      view& view_
    , Content&& content
    , F&& on_ok
    , std::string ok_text
    , color ok_color
   )
   {
      auto ok_button = share(button(std::move(ok_text), 1.0, ok_color));
      ok_button->on_click = [on_ok](bool) mutable
      {
         on_ok();
      };

      return dialog1(view_, content, ok_button);
   }

   /**
    * \brief
    *    Creates a dialog with specified content and two buttons.
    *
    *    This function constructs a dialog box with the specified content and
    *    two buttons that are vertically aligned in the dialog. The function
    *    is generally used for scenarios requiring a 'Cancel' and 'OK'
    *    option.
    *
    *    The popup dialog reacts to certain keyboard and mouse events:
    *    * Clicking the 'OK' button or pressing Return/Enter key will trigger
    *      the `ok_button` and close the dialog.
    *    * Clicking the 'Cancel' button or pressing the Escape key will
    *       trigger the `cancel_button` and close the dialog.
    *
    *    Note: Install `on_click` callbacks before calling `dialog2`.
    *
    * \tparam Content
    *    Content type. Has to satisfy `Element`.
    * \tparam ButtonPtr
    *    Button type. Must satisfy `ElementPtr`.
    *
    * \param view_
    *    Reference to the view to display the dialog.
    * \param content
    *    Content to be displayed in the dialog.
    * \param ok_button
    *    Dialog button, typically for confirming an action.
    * \param cancel_button
    *    Dialog button, typically for cancelling an action.
    *
    * \return
    *    Shared pointer to the created dialog.
    */
   template <concepts::Element Content, concepts::ElementPtr ButtonPtr>
   inline auto dialog2(
    view& view_
    , Content&& content
    , ButtonPtr ok_button
    , ButtonPtr cancel_button
   )
   {
      auto button_size = get_theme().dialog_button_size;
      auto popup =
         detail::make_dialog_popup(
            vtile(
               margin_bottom(20, std::forward<Content>(content)),
               align_right(
                  htile(
                     hsize(button_size, hold(cancel_button)),
                     margin_left(20, hsize(button_size, hold(ok_button)))
                  )
               )
            )
         );

      detail::link_key(popup, ok_button, cancel_button);
      detail::link_button(view_, popup, ok_button);
      detail::link_button(view_, popup, cancel_button);
      return popup;
   }

   /**
    * \brief
    *    Overloaded version of dialog2 function which creates a dialog with
    *    specified content and preset 'OK' and 'Cancel' buttons that trigger
    *    user-defined actions.
    *
    *    This function constructs a dialog box with the user-specified
    *    content and two buttons: 'OK' and 'Cancel'. When the 'OK' button is
    *    clicked or 'Enter' key is pressed, it triggers an action defined by
    *    the `on_ok` parameter. Similarily, a click on the 'Cancel' button or
    *    'Escape' key press triggers an action defined by the `on_cancel`
    *    parameter.
    *
    * \tparam Content
    *    The type of the content. The type must satisfy the `Element`
    *    concept.
    * \tparam F1
    *    The type of `on_ok` function. The type must be
    *    `NullaryFunction`.
    * \tparam F2
    *    The type of `on_cancel` function. The type must be
    *    `NullaryFunction`.
    *
    * \param view_
    *    Reference to the view for displaying the dialog.
    * \param content
    *    The content to be displayed.
    * \param on_ok
    *    Function to be called when the 'OK' button is clicked or 'Enter' key
    *    is pressed.
    * \param on_cancel
    *    Function to be called when the 'Cancel' button is clicked or
    *    'Escape' key is pressed.
    * \param ok_text
    *    The text for the 'OK' button. Default text is "OK".
    * \param cancel_text
    *    The text for the 'Cancel' button. Default text is "Cancel".
    * \param ok_color
    *    The color for 'OK' button. Default is the theme's indicator_color.
    *
    * \return
    *    A shared pointer to the constructed dialog.
    */
   template <
      concepts::Element Content
    , concepts::NullaryFunction F1
    , concepts::NullaryFunction F2
   >
   inline auto dialog2(
      view& view_
    , Content&& content
    , F1&& on_ok
    , F2&& on_cancel
    , std::string ok_text
    , std::string cancel_text
    , color ok_color
   )
   {
      auto cancel_button = share(button(std::move(cancel_text), 1.0));
      auto ok_button = share(button(std::move(ok_text), 1.0, ok_color));

      cancel_button->on_click = [on_cancel](bool) mutable
      {
         on_cancel();
      };

      ok_button->on_click = [on_ok](bool) mutable
      {
         on_ok();
      };

      return dialog2(view_, content, ok_button, cancel_button);
   }

   /**
    * \brief
    *    `dialog2r` function creating a dialog with preset 'OK' and 'Cancel'
    *    buttons reversed, with 'Cancel' being the default that maps to both
    *    the 'Enter' and 'Esc' keys.
    *
    *    This function constructs a dialog box with the specified content and
    *    two buttons: 'OK' and 'Cancel'. In contrast to other dialog
    *    functions, 'Cancel' serves as the default choice. Pressing the
    *    'Enter' or 'Esc' key, or clicking the 'Cancel' button, triggers the
    *    action associated with the `cancel_button`.
    *
    * \tparam Content
    *   The type of content to be displayed. The type must satisfy the
    *   `Element` concept.
    * \tparam ButtonPtr
    *   The type of the button. The type must satisfy the `ElementPtr` concept.
    *
    * \param view_
    *   Reference to the view to display the dialog.
    * \param content
    *   Content to be displayed.
    * \param ok_button
    *   Button typically used for confirming an action.
    * \param cancel_button
    *   Button typically used for cancelling an action and is set as the
    *   default response.
    *
    * \return
    *   A shared pointer to the created dialog.
    */
   template <concepts::Element Content, concepts::ElementPtr ButtonPtr>
   inline auto dialog2r(
      view& view_
    , Content&& content
    , ButtonPtr ok_button
    , ButtonPtr cancel_button
   )
   {
      auto button_size = get_theme().dialog_button_size;
      auto popup =
         detail::make_dialog_popup(
            vtile(
               margin_bottom(20, std::forward<Content>(content)),
               align_right(
                  htile(
                     hsize(button_size, hold(cancel_button)),
                     margin_left(20, hsize(button_size, hold(ok_button)))
                  )
               )
            )
         );

      detail::link_key(popup, cancel_button, cancel_button);
      detail::link_button(view_, popup, ok_button);
      detail::link_button(view_, popup, cancel_button);
      return popup;
   }

   /**
    * \brief
    *    `dialog2r` Overloaded version of dialog2r function with specified
    *    button actions, creating a dialog with preset 'OK' and 'Cancel'
    *    buttons, and 'Cancel' as the default connected to both the 'Enter'
    *    and 'Esc' keys.
    *
    *    This function constructs a dialog box with the user-specified
    *    content and two buttons: 'OK' and 'Cancel'. With 'Cancel' as the
    *    default option, pressing the 'Enter' or 'Esc' key, or clicking the
    *    'Cancel' button, triggers an action defined by the `on_cancel`
    *    parameter. Clicking the 'OK' button triggers an action defined by
    *    the `on_ok` parameter.
    *
    * \tparam Content
    *    The type of the content. The type must satisfy the `Element`
    *    concept.
    * \tparam F1
    *    The type of `on_ok` function. The type must be
    *    `NullaryFunction`.
    * \tparam F2
    *    The type of `on_cancel` function. The type must be
    *    `NullaryFunction`.
    *
    * \param view_
    *    Reference to the view to display the dialog.
    * \param content
    *    The content to be displayed in the dialog.
    * \param on_ok
    *    Action to be executed on 'OK' button click.
    * \param on_cancel
    *    Action to be executed on 'Enter', 'Esc', or 'Cancel' button click.
    * \param ok_text
    *    Text for the 'OK' button. Default is "OK".
    * \param cancel_text
    *    Text for the 'Cancel' button. Default is "Cancel".
    * \param cancel_color
    *    Color for the 'Cancel' button. Default is the theme's
    *    indicator_color.
    *
    * \return
    *    A shared pointer to the constructed dialog.
    */
   template <
      concepts::Element Content
    , concepts::NullaryFunction F1
    , concepts::NullaryFunction F2
   >
   inline auto dialog2r(
      view& view_
    , Content&& content
    , F1&& on_ok
    , F2&& on_cancel
    , std::string ok_text
    , std::string cancel_text
    , color cancel_color
   )
   {
      auto cancel_button = share(button(std::move(cancel_text), 1.0, cancel_color));
      auto ok_button = share(button(std::move(ok_text), 1.0));

      cancel_button->on_click = [on_cancel](bool) mutable
      {
         on_cancel();
      };

      ok_button->on_click = [on_ok](bool) mutable
      {
         on_ok();
      };

      return dialog2r(view_, content, ok_button, cancel_button);
   }

   /**
    * \brief
    *    Utility to open a popup after a short delay, allowing the UI to
    *    settle.
    *
    *    `open_popup` opens a provided popup element after a few
    *    milliseconds. This delay offers the UI a chance to stabilize before
    *    the popup appears.
    *
    * \param popup
    *    Shared pointer to the popup element that will be displayed.
    * \param view_
    *    Reference to the view where the popup will be displayed.
    *
    * Note: The time delay is defined as a constant (10ms).
    */
   inline void open_popup(element_ptr popup, view& view_)
   {
      using namespace std::chrono_literals;
      constexpr auto delay = 10ms;

      view_.post(delay,
         [&view_, popup]()
         {
            view_.add(popup, true);
         }
      );
   }
}

#endif
