/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_GALLERY_RADIO_BUTTON_FEBRUARY_29_2020)
#define ELEMENTS_GALLERY_RADIO_BUTTON_FEBRUARY_29_2020

#include <elements/element/gallery/button.hpp>
#include <elements/element/gallery/toggle_selector.hpp>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Radio Button
   ////////////////////////////////////////////////////////////////////////////
   void draw_radio_button(
      context const& ctx, std::string const& text, bool state, bool hilite
   );

   template <bool state>
   struct radio_button_element : toggle_selector
   {
      using toggle_selector::toggle_selector;

      void                    draw(context const& ctx) override;
   };

   template <bool state>
   void radio_button_element<state>::draw(context const& ctx)
   {
      draw_radio_button(ctx, _text, state, ctx.bounds.includes(ctx.view.cursor_pos()));
   }

   inline basic_choice radio_button(std::string text)
   {
      return basic_choice(
         radio_button_element<false>{ text }
       , radio_button_element<true>{ text }
      );
   }
}}

#endif
