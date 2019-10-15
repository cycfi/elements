/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

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
#include <elements/element//gallery/button.hpp>
#include <elements/support/theme.hpp>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Dialog 0 (no button)
   ////////////////////////////////////////////////////////////////////////////
   template <typename Content>
   inline auto dialog0(
      Content&& content
   )
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
   // Dialog 1 (single botton, e.g. OK)
   ////////////////////////////////////////////////////////////////////////////
   template <typename Content>
   inline auto dialog1(
      Content&& content
    , char const* ok_text = "OK"
    , color ok_color = get_theme().indicator_color
   )
   {
      auto ok_button = share(button(ok_text, 1.0, ok_color));
      auto popup = share(
         key_intercept(align_center_middle(
            layer(
               margin({ 20, 20, 20, 20 },
                  vtile(
                     bottom_margin(20, std::forward<Content>(content)),
                     align_right(hsize(100, hold(ok_button)))
                  )
               ),
               panel{ /* opacity */0.98 }
         ))));

      popup->on_key =
         [ok_ = get(ok_button)](auto k)
         {
            if (k.key == key_code::enter)
            {
               if (auto ok = ok_.lock())
                  ok->value(true);
               return true;
            }
            return false;
         };

      return std::pair{ ok_button, popup };
   }

   ////////////////////////////////////////////////////////////////////////////
   // Dialog 2 (two bottons, e.g. Cancel and OK)
   ////////////////////////////////////////////////////////////////////////////
   template <typename Content>
   inline auto dialog2(
      Content&& content
    , char const* cancel_text = "Cancel"
    , char const* ok_text = "OK"
    , color ok_color = get_theme().indicator_color
   )
   {
      auto cancel_button = share(button(cancel_text, 1.0));
      auto ok_button = share(button(ok_text, 1.0, ok_color));
      auto popup = share(
         key_intercept(align_center_middle(
            layer(
               margin({ 20, 20, 20, 20 },
                  vtile(
                     bottom_margin(20, std::forward<Content>(content)),
                     align_right(
                        htile(
                           hsize(100, hold(cancel_button)),
                           left_margin(20, hsize(100, hold(ok_button)))
                        )
                     )
                  )
               ),
               panel{ /* opacity */0.98 }
         ))));

      popup->on_key =
         [ok_ = get(ok_button), cancel_ = get(cancel_button)](auto k)
         {
            if (k.action == key_action::release)
               return false;

            if (k.key == key_code::enter)
            {
               if (auto ok = ok_.lock())
                  ok->value(true);
               return true;
            }
            else if (k.key == key_code::escape)
            {
               if (auto cancel = cancel_.lock())
                  cancel->value(true);
               return true;
            }
            return false;
         };

      return std::tuple{ ok_button, cancel_button, popup };
   }
}}

#endif
