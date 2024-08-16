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
   class sprite_button_styler : public sprite
   {
   public:
                              sprite_button_styler(sprite const& s);

      void                    draw(context const& ctx) override;
   };

   template <concepts::MomentaryButton Base = basic_button>
   inline proxy<sprite_button_styler, Base>
   momentary_button(sprite const& s)
   {
      return {sprite_button_styler{s}};
   }

   template <concepts::MomentaryButton Base = basic_button>
   inline proxy<sprite_button_styler, Base>
   momentary_button(sprite& s)
   {
      return {sprite_button_styler{s}};
   }

   template <concepts::ToggleButton Base = basic_toggle_button>
   inline proxy<sprite_button_styler, Base>
   toggle_button(sprite const& s)
   {
      return {sprite_button_styler{s}};
   }

   template <concepts::ToggleButton Base = basic_toggle_button>
   inline proxy<sprite_button_styler, Base>
   toggle_button(sprite& s)
   {
      return {sprite_button_styler{s}};
   }

   template <concepts::LatchingButton Base = basic_latching_button>
   inline proxy<sprite_button_styler, Base>
   latching_button(sprite const& s)
   {
      return {sprite_button_styler{s}};
   }

   template <concepts::LatchingButton Base = basic_latching_button>
   inline proxy<sprite_button_styler, Base>
   latching_button(sprite& s)
   {
      return {sprite_button_styler{s}};
   }

   //--------------------------------------------------------------------------
   // Inlines
   //--------------------------------------------------------------------------
   inline sprite_button_styler::sprite_button_styler(sprite const& s)
    : sprite(s)
   {}

   inline void sprite_button_styler::draw(context const& ctx)
   {
      auto btn = find_parent<basic_button*>(ctx);
      if (!btn)
         return;

      auto value = btn->value();
      auto hilite = btn->hilite();
      // if (!ctx.enabled && num_frames() > 4)
      //    index(4); // disabled
      // else
         index((value? 2 : 0) + hilite); // enabled
      basic_sprite::draw(ctx);
   }
}

#endif
