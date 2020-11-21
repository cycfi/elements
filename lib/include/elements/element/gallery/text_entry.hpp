/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_GALLERY_TEXT_ENTRY_JUNE_5_2016)
#define ELEMENTS_GALLERY_TEXT_ENTRY_JUNE_5_2016

#include <elements/element/layer.hpp>
#include <elements/element/port.hpp>
#include <elements/element/margin.hpp>
#include <elements/element/misc.hpp>
#include <elements/element/text.hpp>
#include <elements/element/size.hpp>
#include <elements/support/text_utils.hpp>
#include <type_traits>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Text Entry
   ////////////////////////////////////////////////////////////////////////////
   template <typename InputBox>
   inline auto input_box(
      InputBox&& text_input
    , rect pad = { 5, 5, 5, 5 }
    , typename std::enable_if<
         std::is_base_of<element, remove_cvref_t<InputBox>>::value
      >::type* = nullptr
   )
   {
      return layer(
         margin(
            pad,
            scroller(
               hsize(16384, std::forward<InputBox>(text_input)),
               no_scrollbars | no_vscroll
            )
         ),
         frame{}
      );
   }

   inline auto input_box(
      std::string placeholder
    , font font_
    , float size // ratio relative to get_theme().text_box_font_size
   )
   {
      auto font_size = get_theme().text_box_font_size * size;
      auto pad = rect{
         font_size * 0.3f, font_size * 0.3f
       , font_size * 0.3f, font_size * 0.3f
      };
      auto tbox = share(basic_input_box{ std::move(placeholder), font_, font_size });
      return std::make_pair(input_box(hold(tbox), pad), tbox);
   }

   inline auto input_box(
      std::string placeholder
    , float size // ratio relative to get_theme().text_box_font_size
   )
   {
      return input_box(std::move(placeholder), get_theme().text_box_font, size);
   }

   inline auto input_box(std::string placeholder)
   {
      return input_box(std::move(placeholder), get_theme().text_box_font, 1.0);
   }

   inline auto input_box(float size = 1.0)   // ratio relative to get_theme().text_box_font_size
   {
      return input_box("", get_theme().text_box_font, size);
   }
}}

#endif
