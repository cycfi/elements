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
   /**
    * \class
    *    sprite_button_styler
    *
    * \brief
    *    A class template that styles a button with a sprite.
    *
    *    This class template inherits from a base button class and provides
    *    additional functionality to style the button using a sprite. The
    *    sprite can be used to represent different states of the button.
    *
    * \tparam Base The base button class to inherit from.
    */
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

   /**
    * \brief
    *    Draws the sprite button with the given context.
    *
    *    This function overrides the draw function in the base class to
    *    provide specific drawing functionality for the sprite button. It
    *    determines the appropriate frame of the sprite to display based on
    *    the button's state (enabled/disabled, value, and highlight).
    *
    * \param ctx
    *    The context within which the element should be drawn. The context
    *    provides information about the view, canvas, and other properties
    *    that might be necessary for drawing the element.
    */
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

   /**
    * \brief
    *    Creates a momentary button with a sprite button styler.
    *
    *    This function template creates a proxy object for a momentary button
    *    styled with a sprite. A momentary button is a button that only
    *    remains active while it is being pressed.
    *
    * \tparam Base
    *    The base type of the momentary button. `Base` should be a momentary
    *    button that adheres to ` MomentaryButton` concept with
    *    `basic_button` as the default type.
    *
    * \tparam Styler
    *    The type of the sprite styler. The sprite styler must adhere to the
    *    SpriteSubject concept,
    *
    * \param s
    *    The sprite styler to be used for the button.
    *
    * \return
    *    A proxy object that combines the sprite button styler with the
    *    momentary button.
    */
   template <
      concepts::MomentaryButton Base
    , concepts::SpriteSubject Styler
   >
   inline proxy<remove_cvref_t<Styler>, sprite_button_styler<Base>>
   momentary_button(Styler&& s)
   {
      return {std::forward<Styler>(s)};
   }

   /**
    * \brief
    *    Creates a toggle button with a sprite button styler.
    *
    *    This function template creates a proxy object for a toggle button
    *    styled with a sprite. A toggle button is a button that remains
    *    active until it is pressed again.
    *
    * \tparam Base
    *    The base type of the latching button. `Base` should be a toggle
    *    button that adheres to ` ToggleButton` concept with
    *    `basic_toggle_button` as the default type.
    *
    * \tparam Styler
    *    The type of the sprite styler. The sprite styler must adhere to the
    *    SpriteSubject concept,
    *
    * \param s
    *    The sprite styler to be used for the button.
    *
    * \return
    *    A proxy object that combines the sprite button styler with the
    *    toggle button.
    */
   template <
      concepts::ToggleButton Base
    , concepts::SpriteSubject Styler
   >
   inline proxy<remove_cvref_t<Styler>, sprite_button_styler<Base>>
   toggle_button(Styler&& s)
   {
      return {std::forward<Styler>(s)};
   }

   /**
    * \brief
    *    Creates a latching button with a sprite button styler.
    *
    *    This function template creates a proxy object for a latching button
    *    styled with a sprite. A latching button is a button that maintains
    *    its state after being clicked and holds this state indefinitely
    *    until it is reset programmatically.
    *
    * \tparam Base
    *    The base type of the latching button. `Base` should be a latching
    *    button that adheres to `LatchingButton` concept with
    *    `basic_latching_button` as the default type.
    *
    * \tparam Styler
    *    The type of the sprite styler. The sprite styler must adhere to the
    *    SpriteSubject concept,
    *
    * \param s
    *    The sprite styler to be used for the button.
    *
    * \return
    *    A proxy object that combines the sprite button styler with the
    *    latching button.
    */
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
