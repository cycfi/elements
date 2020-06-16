/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_GALLERY_MESSAGE_BOX_JUNE_5_2016)
#define ELEMENTS_GALLERY_MESSAGE_BOX_JUNE_5_2016

#include <elements/element/text.hpp>
#include <elements/element/gallery/button.hpp>
#include <elements/element/gallery/dialog.hpp>
#include <infra/string_view.hpp>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Message Box 0 (no button)
   ////////////////////////////////////////////////////////////////////////////
   inline auto message_box0(
      std::string message
    , std::uint32_t icon_id
    , extent text_box_size = get_theme().message_textbox_size
   )
   {
      auto textbox = fixed_size(text_box_size, static_text_box{ std::move(message) });
      return dialog0(
         margin({ 20, 20, 20, 20 }, htile(
            align_top(icon{ icon_id, 2.5 }),
            left_margin(20, std::move(textbox))
         ))
      );
   }

   ////////////////////////////////////////////////////////////////////////////
   // Message Box 1 (single button, e.g. OK)
   ////////////////////////////////////////////////////////////////////////////
   template <typename F>
   inline auto message_box1(
      view& view_
    , std::string message
    , std::uint32_t icon_id
    , F&& on_ok
    , std::string ok_text = "OK"
    , extent text_box_size = get_theme().message_textbox_size
    , color ok_color = get_theme().indicator_color
   )
   {
      auto textbox = fixed_size(text_box_size, static_text_box{ std::move(message) });
      return dialog1(
         view_,
         htile(
            align_top(icon{ icon_id, 2.5 }),
            left_margin(20, std::move(textbox))
         ),
         std::forward<F>(on_ok),
         std::move(ok_text),
         ok_color
      );
   }

   ////////////////////////////////////////////////////////////////////////////
   // Message Box 2 (two buttons, e.g. Cancel and OK)
   ////////////////////////////////////////////////////////////////////////////
   template <typename F1, typename F2>
   inline auto message_box2(
      view& view_
    , std::string message
    , std::uint32_t icon_id
    , F1&& on_ok
    , F2&& on_cancel
    , std::string ok_text = "OK"
    , std::string cancel_text = "Cancel"
    , extent text_box_size = get_theme().message_textbox_size
    , color ok_color = get_theme().indicator_color
   )
   {
      auto textbox = fixed_size(text_box_size, static_text_box{ std::move(message) });
      return dialog2(
         view_,
         htile(
            align_top(icon{ icon_id, 2.5 }),
            left_margin(20, std::move(textbox))
         ),
         std::forward<F1>(on_ok),
         std::forward<F2>(on_cancel),
         std::move(ok_text),
         std::move(cancel_text),
         ok_color
      );
   }

   ////////////////////////////////////////////////////////////////////////////
   // Message Box 2 Reversed (two buttons, e.g. Cancel and OK, but with Cancel
   // being the default that maps to both the enter and esc keys)
   ////////////////////////////////////////////////////////////////////////////
   template <typename F1, typename F2>
   inline auto message_box2r(
      view& view_
    , std::string message
    , std::uint32_t icon_id
    , F1&& on_ok
    , F2&& on_cancel
    , std::string ok_text = "OK"
    , std::string cancel_text = "Cancel"
    , extent text_box_size = get_theme().message_textbox_size
    , color ok_color = get_theme().indicator_color
   )
   {
      auto textbox = fixed_size(text_box_size, static_text_box{ std::move(message) });
      return dialog2r(
         view_,
         htile(
            align_top(icon{ icon_id, 2.5 }),
            left_margin(20, std::move(textbox))
         ),
         std::forward<F1>(on_ok),
         std::forward<F2>(on_cancel),
         std::move(ok_text),
         std::move(cancel_text),
         ok_color
      );
   }
}}

#endif
