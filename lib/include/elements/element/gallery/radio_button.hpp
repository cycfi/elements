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
   struct radio_button_element : toggle_selector, basic_receiver<button_state>
   {
      using toggle_selector::toggle_selector;

      void                    draw(context const& ctx) override;
   };

   inline auto radio_button(std::string text)
   {
      return choice(radio_button_element{ text });
   }
}}

#endif
