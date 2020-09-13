/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_GALLERY_CHECK_BOX_JUNE_5_2016)
#define ELEMENTS_GALLERY_CHECK_BOX_JUNE_5_2016

#include <elements/element/gallery/button.hpp>
#include <elements/element/gallery/toggle_selector.hpp>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Check Box
   ////////////////////////////////////////////////////////////////////////////
   struct check_box_element : toggle_selector, basic_receiver<button_state>
   {
      using toggle_selector::toggle_selector;

      void                    draw(context const& ctx) override;
   };

   inline auto check_box(std::string text)
   {
      return toggle_button(check_box_element{ text });
   }
}}

#endif
