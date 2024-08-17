/*=============================================================================
   Copyright (c) 2016-2024 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_SPRITE_BUTTON_AUGUST_16_2024)
#define ELEMENTS_SPRITE_BUTTON_AUGUST_16_2024

#include <elements/element/button.hpp>
#include <elements/element/image.hpp>

namespace cycfi::elements
{
   template <typename Base>
   class sprite_button_styler : public Base
   {
   public:

      using Base::Base;

      void                    draw(context const& ctx) override;
   };

   template <
      concepts::MomentaryButton Base = basic_button
    , concepts::SpriteSubject Styler
   >
   inline proxy<remove_cvref_t<Styler>, sprite_button_styler<Base>>
   momentary_button(Styler&& s);

   template <
      concepts::ToggleButton Base = basic_toggle_button
    , concepts::SpriteSubject Styler
   >
   inline proxy<remove_cvref_t<Styler>, sprite_button_styler<Base>>
   toggle_button(Styler&& s);

   template <
      concepts::LatchingButton Base = basic_latching_button
    , concepts::SpriteSubject Styler
   >
   inline proxy<remove_cvref_t<Styler>, sprite_button_styler<Base>>
   latching_button(Styler&& s);

   //--------------------------------------------------------------------------
   // Inlines
   //--------------------------------------------------------------------------

   template <typename Base>
   inline void sprite_button_styler<Base>::draw(context const& ctx)
   {
      if (auto sp = find_subject<sprite*>(this))
      {
         auto value = this->value();
         auto hilite = this->hilite();
         if (!ctx.enabled && sp->num_frames() > 4)
            sp->index(4); // disabled
         else
            sp->index((value? 2 : 0) + hilite); // enabled
         Base::draw(ctx);
      }
   }

   template <
      concepts::MomentaryButton Base
    , concepts::SpriteSubject Styler
   >
   inline proxy<remove_cvref_t<Styler>, sprite_button_styler<Base>>
   momentary_button(Styler&& s)
   {
      return {std::forward<Styler>(s)};
   }

   template <
      concepts::ToggleButton Base
    , concepts::SpriteSubject Styler
   >
   inline proxy<remove_cvref_t<Styler>, sprite_button_styler<Base>>
   toggle_button(Styler&& s)
   {
      return {std::forward<Styler>(s)};
   }

   template <
      concepts::LatchingButton Base
    , concepts::SpriteSubject Styler
   >
   inline proxy<remove_cvref_t<Styler>, sprite_button_styler<Base>>
   latching_button(Styler&& s)
   {
      return {std::forward<Styler>(s)};
   }
}

#endif
