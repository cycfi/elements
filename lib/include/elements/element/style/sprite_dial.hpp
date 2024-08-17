/*=============================================================================
   Copyright (c) 2016-2024 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_SPRITE_DIAL_AUGUST_17_2024)
#define ELEMENTS_SPRITE_DIAL_AUGUST_17_2024

#include <elements/element/dial.hpp>
#include <elements/element/image.hpp>
#include <type_traits>

namespace cycfi::elements
{
   template <concepts::SpriteSubject Styler>
   inline proxy<remove_cvref_t<Styler>, basic_dial>
   dial(Styler&& styler, double init_value = 0.0);

   //--------------------------------------------------------------------------
   // Inlines
   //--------------------------------------------------------------------------

   template <concepts::SpriteSubject Styler>
   inline proxy<remove_cvref_t<Styler>, basic_dial>
   dial(Styler&& styler, double init_value)
   {
      return {std::forward<Styler>(styler), init_value};
   }
}

#endif
