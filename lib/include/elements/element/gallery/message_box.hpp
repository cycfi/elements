/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_GALLERY_MESSAGE_BOX_JUNE_5_2016)
#define ELEMENTS_GALLERY_MESSAGE_BOX_JUNE_5_2016

#include <elements/element/text.hpp>
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
   // Message Box
   ////////////////////////////////////////////////////////////////////////////
   inline auto message_box0(
      char const* message
    , std::uint32_t icon_id
    , extent size = get_theme().message_box_size
   )
   {
      auto textbox = static_text_box{ message };
      auto popup = share(
         align_center_middle(
            fixed_size(size,
            layer(
               margin({ 20, 20, 20, 20 }, htile(
                  align_top(icon{ icon_id, 2.5 }),
                  left_margin(20, std::move(textbox))
               )),
               panel{}
         ))));

      return popup;
   }

   inline auto message_box1(
      char const* message
    , std::uint32_t icon_id
    , char const* ok_text = "OK"
    , extent size = get_theme().message_box_size
    , color ok_color = get_theme().indicator_color
   )
   {
      auto textbox = static_text_box{ message };
      auto ok_button = share(button(ok_text, 1.0, ok_color));
      auto popup = share(
         key_intercept(align_center_middle(
            fixed_size(size,
            layer(
               margin({ 20, 20, 20, 20 },
                  vtile(
                     htile(
                        align_top(icon{ icon_id, 2.5 }),
                        left_margin(20, std::move(textbox))
                     ),
                     align_right(hsize(100, hold(ok_button)))
                  )
               ),
               panel{}
         )))));

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

   inline auto message_box2(
      char const* message
    , std::uint32_t icon_id
    , char const* cancel_text = "Cancel"
    , char const* ok_text = "OK"
    , extent size = get_theme().message_box_size
    , color ok_color = get_theme().indicator_color
   )
   {
      auto textbox = static_text_box{ message };
      auto cancel_button = share(button(cancel_text, 1.0));
      auto ok_button = share(button(ok_text, 1.0, ok_color));
      auto popup = share(
         key_intercept(align_center_middle(
            fixed_size(size,
            layer(
               margin({ 20, 20, 20, 20 },
                  vtile(
                     htile(
                        align_top(icon{ icon_id, 2.5 }),
                        left_margin(20, std::move(textbox))
                     ),
                     align_right(
                        htile(
                           hsize(100, hold(cancel_button)),
                           left_margin(20, hsize(100, hold(ok_button)))
                        )
                     )
                  )
               ),
               panel{}
         )))));

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
