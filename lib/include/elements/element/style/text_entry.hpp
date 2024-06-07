/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_STYLE_TEXT_ENTRY_JUNE_5_2016)
#define ELEMENTS_STYLE_TEXT_ENTRY_JUNE_5_2016

#include <elements/element/layer.hpp>
#include <elements/element/port.hpp>
#include <elements/element/margin.hpp>
#include <elements/element/misc.hpp>
#include <elements/element/text.hpp>
#include <elements/element/size.hpp>
#include <elements/support/text_utils.hpp>
#include <type_traits>

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Text Entry
   ////////////////////////////////////////////////////////////////////////////
   template <typename InputBox>
   inline auto input_box(
      InputBox&& text_input
    , rect pad = {5, 5, 5, 5}
    , typename std::enable_if<
         std::is_base_of<element, remove_cvref_t<InputBox>>::value
      >::type* = nullptr
   )
   {
      return layer(
         margin(
            pad,
            scroller(
               std::forward<InputBox>(text_input),
               no_scrollbars | no_vscroll
            )
         ),
         frame{}
      );
   }

   using input_clip_action = basic_input_box::clip_action;

   inline auto input_box(
      std::string placeholder
    , font_descr font_
    , float size // ratio relative to get_theme().text_box_font_size
    , input_clip_action clip_action = input_clip_action::clip_right
   )
   {
      auto font_size = get_theme().text_box_font._size * size;
      auto pad = rect{
         font_size * 0.3f, font_size * 0.3f
       , font_size * 0.3f, font_size * 0.3f
      };
      auto tbox = share(
         basic_input_box{
            std::move(placeholder)
          , font_.size(font_size)
          , clip_action
         }
      );
      return std::make_pair(input_box(hold(tbox), pad), tbox);
   }

   inline auto input_box(
      std::string placeholder
    , float size // ratio relative to get_theme().text_box_font_size
    , input_clip_action clip_action = input_clip_action::clip_right
   )
   {
      return input_box(
         std::move(placeholder)
       , get_theme().text_box_font
       , size
       , clip_action
      );
   }

   inline auto input_box(
      std::string placeholder
    , input_clip_action clip_action = input_clip_action::clip_right
   )
   {
      return input_box(
         std::move(placeholder)
       , get_theme().text_box_font
       , 1.0
       , clip_action
      );
   }

   inline auto input_box(
      float size = 1.0 // ratio relative to get_theme().text_box_font_size
    , input_clip_action clip_action = input_clip_action::clip_right
   )
   {
      return input_box(
         ""
       , get_theme().text_box_font
       , size
       , clip_action
      );
   }
}

#endif
