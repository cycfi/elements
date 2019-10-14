/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_GALLERY_MESSAGE_BOX_JUNE_5_2016)
#define ELEMENTS_GALLERY_MESSAGE_BOX_JUNE_5_2016

#include <elements/element/text.hpp>
#include <elements/element//gallery/button.hpp>
#include <elements/element//gallery/dialog.hpp>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Message Box 0 (no button)
   ////////////////////////////////////////////////////////////////////////////
   inline auto message_box0(
      char const* message
    , std::uint32_t icon_id
    , float width = get_theme().message_box_width
   )
   {
      auto textbox = static_text_box{ message };
      return dialog0(
         margin({ 20, 20, 20, 20 }, htile(
            align_top(icon{ icon_id, 2.5 }),
            left_margin(20, std::move(textbox))
         )),
         width
      );
   }

   ////////////////////////////////////////////////////////////////////////////
   // Message Box 1 (single botton, e.g. OK)
   ////////////////////////////////////////////////////////////////////////////
   inline auto message_box1(
      char const* message
    , std::uint32_t icon_id
    , char const* ok_text = "OK"
    , float width = get_theme().message_box_width
    , color ok_color = get_theme().indicator_color
   )
   {
      auto textbox = static_text_box{ message };
      return dialog1(
         htile(
            align_top(icon{ icon_id, 2.5 }),
            left_margin(20, std::move(textbox))
         ),
         ok_text,
         width,
         ok_color
      );
   }

   ////////////////////////////////////////////////////////////////////////////
   // Message Box 2 (two bottons, e.g. Cancel and OK)
   ////////////////////////////////////////////////////////////////////////////
   inline auto message_box2(
      char const* message
    , std::uint32_t icon_id
    , char const* cancel_text = "Cancel"
    , char const* ok_text = "OK"
    , float width = get_theme().message_box_width
    , color ok_color = get_theme().indicator_color
   )
   {
      auto textbox = static_text_box{ message };
      return dialog2(
         htile(
            align_top(icon{ icon_id, 2.5 }),
            left_margin(20, std::move(textbox))
         ),
         cancel_text,
         ok_text,
         width,
         ok_color
      );
   }
}}

#endif
