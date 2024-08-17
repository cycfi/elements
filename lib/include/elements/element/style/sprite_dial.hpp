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
   namespace detail
   {
      template <typename T>
      constexpr auto is_sprite()
      {
         if constexpr (std::is_base_of_v<proxy_base, T>)
            return is_sprite<typename T::subject_type>();
         else
            return std::false_type{};
      }

      template <>
      constexpr auto is_sprite<sprite>()
      {
         return std::true_type{};
      }
   }

   namespace concepts
   {
      template <typename T>
      concept SpriteSubject
         = concepts::Element<T> &&
         decltype(detail::is_sprite<std::decay_t<T>>())::value;
   }

   template <concepts::SpriteSubject Styler>
   inline proxy<remove_cvref_t<Styler>, basic_dial>
   dial(Styler&& styler, double init_value = 0.0)
   {
      return {std::forward<Styler>(styler), init_value};
   }
}

#endif
