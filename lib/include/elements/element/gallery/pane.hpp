/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_GALLERY_PANE_JUNE_5_2016)
#define ELEMENTS_GALLERY_PANE_JUNE_5_2016

#include <elements/element/align.hpp>
#include <elements/element/layer.hpp>
#include <elements/element/margin.hpp>
#include <elements/element/misc.hpp>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Pane
   ////////////////////////////////////////////////////////////////////////////
   template <typename Heading, typename Content>
   inline auto pane(
      Heading&& heading,
      Content&& content,
      bool center_heading = true
   )
   {
      auto align_ = center_heading? 0.5 : 0;
      return
        layer(
            align_top(
               layer(
                  halign(
                     align_,
                     margin({10, 4, 10, 4}, std::forward<Heading>(heading))
                  ),
                  title_bar{}
               )
            ),
            top_margin(40, std::forward<Content>(content)),
            panel{}
        );
   }

   template <typename Content>
   inline auto pane(
      std::string title,
      Content&& content,
      float title_size = 1.0,
      bool center_heading = true
   )
   {
      return pane(heading(std::move(title)).relative_font_size(title_size), content, center_heading);
   }

   template <typename Content>
   inline auto pane(
      char const* title,
      Content&& content,
      float title_size = 1.0,
      bool center_heading = true
   )
   {
      return pane(heading(title).relative_font_size(title_size), content, center_heading);
   }
}}

#endif
