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
   /**
    * \class
    *    sprite_dial_styler
    *
    * \brief
    *    A class template that styles a dial control with a sprite.
    *
    *    This class template inherits from a base dial class and provides
    *    additional functionality to style the dial using a sprite. The
    *    sprite can be used to represent different states or positions of the
    *    dial. The dial has a value that is a normalized `double` in the
    *    range [0, 1]. This value maps to different frames in the sprite to
    *    visually show the dial position.
    *
    * \tparam Base The base dial class to inherit from.
    */
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

   /**
    * \brief
    *    Draws the sprite dial with the given context.
    *
    *    The draw function determines the appropriate frame of the sprite to
    *    display based on the dial's value: a normalized `double` in the
    *    range [0, 1], which maps to different frames in the sprite to
    *    visually show the dial position.
    *
    * \param ctx
    *    The context within which the element should be drawn. The context
    *    provides information about the view, canvas, and other properties
    *    that might be necessary for drawing the element.
    */
   template <typename Base>
   inline void sprite_dial_styler<Base>::draw(context const& ctx)
   {
      if (auto sp = find_subject<sprite*>(this))
      {
         sp->index(this->value() * (sp->num_frames()-1));
         Base::draw(ctx);
      }
   }

   /**
    * \brief
    *    Creates a dial control with a sprite dial styler.
    *
    * \tparam Styler
    *    The type of the sprite styler. The sprite styler must adhere to the
    *    SpriteSubject concept.
    *
    * \param styler
    *    The sprite styler to be used for the dial.
    *
    * \param init_value
    *    The initial value of the dial, which is a normalized `double` in the
    *    range [0, 1].
    *
    * \return
    *    A proxy object that combines the sprite dial styler with the basic
    *    dial.
    */
   template <concepts::SpriteSubject Styler>
   inline proxy<remove_cvref_t<Styler>, sprite_dial_styler<basic_dial>>
   dial(Styler&& styler, double init_value)
   {
      return {std::forward<Styler>(styler), init_value};
   }
}

#endif
