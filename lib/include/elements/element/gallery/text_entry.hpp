/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

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

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Text Entry
   ////////////////////////////////////////////////////////////////////////////
   template <typename InputBox>
   inline auto input_box(
      InputBox&& text_input
    , rect pad  = rect{ 5, 5, 5, 4 }
   )
   {
      return layer(
         margin(
            pad,
            scroller(
               hsize(16384, std::move(text_input)),
               no_scrollbars | no_vscroll
            )
         ),
         frame{}
      );
   }

   inline auto input_box(
      std::string const& placeholder
    , rect pad = rect{ 5, 5, 5, 4 }
   )
   {
      return input_box(
         basic_input_box{ placeholder }
       , pad
      );
   }

   inline auto input_box(
      char const* placeholder
    , rect pad = rect{ 5, 5, 5, 4 }
   )
   {
      return input_box(
         basic_input_box{ placeholder }
       , pad
      );
   }
}}

#endif
