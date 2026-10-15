/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_STYLE_TOOLTIP_OCTOBER_10_2026)
#define ELEMENTS_STYLE_TOOLTIP_OCTOBER_10_2026

#include <elements/element/tooltip.hpp>
#include <elements/element/label.hpp>
#include <elements/element/margin.hpp>
#include <elements/support/theme.hpp>
#include <string>

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Tooltip bubbles: a tip in a bubble of the theme's tooltip_color, its
   // pointer toward the element, with a soft shadow.
   ////////////////////////////////////////////////////////////////////////////
   template <concepts::Element Content>
   inline auto tooltip_bubble(Content&& content)
   {
      return basic_tooltip_bubble(
         margin({12, 6, 12, 6}, std::forward<Content>(content)));
   }

   inline auto tooltip_bubble(std::string text)
   {
      return tooltip_bubble(
         label(std::move(text)).font_color(get_theme().tooltip_text_color));
   }
}

#endif
