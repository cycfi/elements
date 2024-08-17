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
   template <typename Subject>
   class sprite_dial_styler : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                              sprite_dial_styler(Subject subject);

      void                    draw(context const& ctx) override;
   };

   template <concepts::SpriteSubject Styler>
   inline proxy<sprite_dial_styler<remove_cvref_t<Styler>>, basic_dial>
   dial(Styler&& styler, double init_value = 0.0);

   //--------------------------------------------------------------------------
   // Inlines
   //--------------------------------------------------------------------------

   template <typename Subject>
   inline sprite_dial_styler<Subject>::sprite_dial_styler(Subject subject)
    : base_type{std::move(subject)}
   {}

   template <typename Subject>
   inline void sprite_dial_styler<Subject>::draw(context const& ctx)
   {
      auto dial = find_parent<basic_dial*>(ctx);
      if (!dial)
         return;

      auto sp = find_subject<sprite*>(this);
      if (sp)
      {
         sp->index(dial->value() * (sp->num_frames()-1));
         sp->draw(ctx);
      }
   }

   template <concepts::SpriteSubject Styler>
   inline proxy<sprite_dial_styler<remove_cvref_t<Styler>>, basic_dial>
   dial(Styler&& styler, double init_value)
   {
      using styler_type = sprite_dial_styler<remove_cvref_t<Styler>>;
      return {styler_type{std::forward<Styler>(styler)}, init_value};
   }
}

#endif
