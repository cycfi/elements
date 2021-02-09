/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_GALLERY_PANE_JUNE_5_2016)
#define ELEMENTS_GALLERY_PANE_JUNE_5_2016

#include <elements/element/tile.hpp>
#include <elements/element/layer.hpp>
#include <elements/element/margin.hpp>
#include <elements/element/misc.hpp>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Pane
   ////////////////////////////////////////////////////////////////////////////
   template <typename TitleBar, typename Heading, typename Content>
   inline auto pane_ex(
      TitleBar&& title_bar,
      Heading&& heading,
      Content&& content,
      float opacity = get_theme().panel_color.alpha,
      bool center_heading = true
   )
   {
      auto align_ = center_heading? 0.5 : 0;
      return
        layer(
            vtile(
               layer(
                  halign(
                     align_,
                     margin({10, 4, 10, 4}, std::forward<Heading>(heading))
                  ),
                  title_bar
               ),
               std::forward<Content>(content)
            ),
            panel{opacity}
        );
   }

   template <typename Heading, typename Content>
   inline auto pane(
      Heading&& heading,
      Content&& content,
      bool center_heading = true
   )
   {
      return pane_ex(
         title_bar{},
         std::forward<Heading>(heading),
         std::forward<Content>(content),
         center_heading
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
      return pane(
         heading(std::move(title)).relative_font_size(title_size),
         content,
         center_heading
      );
   }

   template <typename TitleBar, typename Content>
   inline auto pane_ex(
      TitleBar&& title_bar,
      std::string title,
      Content&& content,
      float title_size = 1.0,
      float opacity = get_theme().panel_color.alpha,
      bool center_heading = true
   )
   {
      return pane_ex(
         std::forward<TitleBar>(title_bar),
         heading(std::move(title)).relative_font_size(title_size),
         content,
         opacity,
         center_heading
      );
   }

   template <typename Content>
   inline auto pane(
      char const* title,
      Content&& content,
      float title_size = 1.0,
      bool center_heading = true
   )
   {
      return pane(
         heading(title).relative_font_size(title_size),
         content,
         center_heading
      );
   }

   template <typename TitleBar, typename Content>
   inline auto pane_ex(
      TitleBar&& title_bar,
      char const* title,
      Content&& content,
      float title_size = 1.0,
      float opacity = get_theme().panel_color.alpha,
      bool center_heading = true
   )
   {
      return pane_ex(
         std::forward<TitleBar>(title_bar),
         heading(title).relative_font_size(title_size),
         content,
         opacity,
         center_heading
      );
   }
}}

#endif
