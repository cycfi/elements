/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

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

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Dialog 0 (no button)
   ////////////////////////////////////////////////////////////////////////////
   template <typename Content>
   inline auto dialog0(Content&& content)
   {
      auto popup = share(
         align_center_middle(
            layer(
               std::forward<Content>(content),
               panel{ /* opacity */0.98 }
         )));

      return popup;
   }

   ////////////////////////////////////////////////////////////////////////////
   // Dialog 1 (single button, e.g. OK)
   ////////////////////////////////////////////////////////////////////////////
   namespace detail
   {
      template <typename Content>
      auto make_dialog_popup(Content&& content)
      {
         return share(
            key_intercept(align_center_middle(
               layer(
                  margin({ 32, 32, 32, 32 },
                     std::forward<Content>(content)
                  ),
                  panel{ /* opacity */0.98 }
            ))));
      }

      template <typename PopupPtr, typename ButtonPtr>
      void link_key(PopupPtr popup, ButtonPtr btn)
      {
         popup->on_key =
            [btn = get(btn)](auto k)
            {
               if (k.action == key_action::press &&
                  k.key == key_code::enter)
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

      template <typename PopupPtr, typename ButtonPtr>
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

      template <typename PopupPtr, typename ButtonPtr, typename F>
      void link_button(view& view_, PopupPtr popup, ButtonPtr btn, F&& f)
      {
         using namespace std::chrono_literals;

         btn->on_click =
            [&view_, eptr = get(popup), f, btn = get(btn)](bool)
            {
               f();
               view_.post(100ms,
                  [&view_, btn, eptr]()
                  {
                     if (auto b = btn.lock())
                        b->value(false);
                     view_.remove(eptr.lock()); // Close the dialog
                  }
               );
            };
      }
   }

   template <typename Content, typename F>
   inline auto dialog1(
      view& view_
    , Content&& content
    , F&& on_ok
    , std::string ok_text = "OK"
    , color ok_color = get_theme().indicator_color
   )
   {
      auto button_size = get_theme().dialog_button_size;
      auto ok_button = share(button(std::move(ok_text), 1.0, ok_color));
      auto popup =
         detail::make_dialog_popup(
            vtile(
               bottom_margin(20, std::forward<Content>(content)),
               align_right(hsize(button_size, hold(ok_button)))
            ));

      detail::link_key(popup, ok_button);
      detail::link_button(view_, popup, ok_button, std::forward<F>(on_ok));
      return popup;
   }

   ////////////////////////////////////////////////////////////////////////////
   // Dialog 2 (two buttons, e.g. Cancel and OK)
   ////////////////////////////////////////////////////////////////////////////
   template <typename Content, typename F1, typename F2>
   inline auto dialog2(
      view& view_
    , Content&& content
    , F1&& on_ok
    , F2&& on_cancel
    , std::string ok_text = "OK"
    , std::string cancel_text = "Cancel"
    , color ok_color = get_theme().indicator_color
   )
   {
      auto button_size = get_theme().dialog_button_size;
      auto cancel_button = share(button(std::move(cancel_text), 1.0));
      auto ok_button = share(button(std::move(ok_text), 1.0, ok_color));
      auto popup =
         detail::make_dialog_popup(
            vtile(
               bottom_margin(20, std::forward<Content>(content)),
               align_right(
                  htile(
                     hsize(button_size, hold(cancel_button)),
                     left_margin(20, hsize(button_size, hold(ok_button)))
                  )
               )
            ));

      detail::link_key(popup, ok_button, cancel_button);
      detail::link_button(view_, popup, ok_button, std::forward<F1>(on_ok));
      detail::link_button(view_, popup, cancel_button, std::forward<F2>(on_cancel));
      return popup;
   }

   ////////////////////////////////////////////////////////////////////////////
   // Dialog 2 Reversed (two buttons, e.g. Cancel and OK, but with Cancel
   // being the default that maps to both the enter and esc keys)
   ////////////////////////////////////////////////////////////////////////////
   template <typename Content, typename F1, typename F2>
   inline auto dialog2r(
      view& view_
    , Content&& content
    , F1&& on_ok
    , F2&& on_cancel
    , std::string ok_text = "OK"
    , std::string cancel_text = "Cancel"
    , color cancel_color = get_theme().indicator_color
   )
   {
      auto button_size = get_theme().dialog_button_size;
      auto cancel_button = share(button(std::move(cancel_text), 1.0, cancel_color));
      auto ok_button = share(button(std::move(ok_text), 1.0));
      auto popup =
         detail::make_dialog_popup(
            vtile(
               bottom_margin(20, std::forward<Content>(content)),
               align_right(
                  htile(
                     hsize(button_size, hold(cancel_button)),
                     left_margin(20, hsize(button_size, hold(ok_button)))
                  )
               )
            ));

      detail::link_key(popup, cancel_button, cancel_button);
      detail::link_button(view_, popup, ok_button, std::forward<F1>(on_ok));
      detail::link_button(view_, popup, cancel_button, std::forward<F2>(on_cancel));
      return popup;
   }
}}

#endif
