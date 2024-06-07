/*=============================================================================
   Copyright (c) 2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_STYLE_TAB_MAY_02_2020)
#define ELEMENTS_STYLE_TAB_MAY_02_2020

#include <elements/element/style/button.hpp>
#include <elements/element/size.hpp>

namespace cycfi::elements
{
   template <concepts::Element Button>
   inline auto tab(Button&& button)
   {
      return choice(std::forward<Button>(button));
   }

   inline auto tab(
      std::string text
    , color active_color = get_theme().active_tab_color
   )
   {
      return tab(
         hmin(hmin_pad(20,
            button_styler{std::move(text)}
               .rounded_top()
               .active_body_color(active_color.opacity(0.5))
         ))
      );
   }
}

#endif
