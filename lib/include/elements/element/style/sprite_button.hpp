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
   template <typename Subject>
   class sprite_button_styler : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                              sprite_button_styler(Subject subject);

      void                    draw(context const& ctx) override;
   };

   template <
      concepts::MomentaryButton Base = basic_button
    , concepts::SpriteSubject Styler
   >
   inline proxy<sprite_button_styler<remove_cvref_t<Styler>>, Base>
   momentary_button(Styler&& s)
   {
      using styler_type = sprite_button_styler<remove_cvref_t<Styler>>;
      return {styler_type{std::forward<Styler>(s)}};
   }

   template <
      concepts::ToggleButton Base = basic_toggle_button
    , concepts::SpriteSubject Styler
   >
   inline proxy<sprite_button_styler<remove_cvref_t<Styler>>, Base>
   toggle_button(Styler&& s)
   {
      using styler_type = sprite_button_styler<remove_cvref_t<Styler>>;
      return {styler_type{std::forward<Styler>(s)}};
   }

   template <
      concepts::LatchingButton Base = basic_latching_button
    , concepts::SpriteSubject Styler
   >
   inline proxy<sprite_button_styler<remove_cvref_t<Styler>>, Base>
   latching_button(Styler&& s)
   {
      using styler_type = sprite_button_styler<remove_cvref_t<Styler>>;
      return {styler_type{std::forward<Styler>(s)}};
   }

   //--------------------------------------------------------------------------
   // Inlines
   //--------------------------------------------------------------------------

   template <typename Subject>
   inline sprite_button_styler<Subject>::sprite_button_styler(Subject subject)
    : base_type{std::move(subject)}
   {}

   template <typename Subject>
   inline void sprite_button_styler<Subject>::draw(context const& ctx)
   {
      auto btn = find_parent<basic_button*>(ctx);
      if (!btn)
         return;

      auto sp = find_subject<sprite*>(this);
      if (sp)
      {
         auto value = btn->value();
         auto hilite = btn->hilite();
         if (!ctx.enabled && sp->num_frames() > 4)
            sp->index(4); // disabled
         else
            sp->index((value? 2 : 0) + hilite); // enabled
         sp->draw(ctx);
      }
   }
}

#endif
