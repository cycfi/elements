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
   template <typename Base>
   class sprite_dial_styler : public Base
   {
   public:

      using Base::Base;

      void                    draw(context const& ctx) override;
   };

   template <concepts::SpriteSubject Styler>
   inline proxy<remove_cvref_t<Styler>, sprite_dial_styler<basic_dial>>
   dial(Styler&& styler, double init_value = 0.0);

   //--------------------------------------------------------------------------
   // Inlines
   //--------------------------------------------------------------------------

   template <typename Base>
   inline void sprite_dial_styler<Base>::draw(context const& ctx)
   {
      if (auto sp = find_subject<sprite*>(this))
      {
         sp->index(this->value() * (sp->num_frames()-1));
         Base::draw(ctx);
      }
   }

   template <concepts::SpriteSubject Styler>
   inline proxy<remove_cvref_t<Styler>, sprite_dial_styler<basic_dial>>
   dial(Styler&& styler, double init_value)
   {
      return {std::forward<Styler>(styler), init_value};
   }
}

#endif
