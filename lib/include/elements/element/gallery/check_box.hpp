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
   void draw_check_box(
      context const& ctx, std::string const& text, bool state, bool hilite
   );

   template <bool state>
   struct check_box_element : toggle_selector
   {
      using toggle_selector::toggle_selector;

      void                    draw(context const& ctx) override;
   };

   template <bool state>
   void check_box_element<state>::draw(context const& ctx)
   {
      draw_check_box(ctx, _text, state, ctx.bounds.includes(ctx.view.cursor_pos()));
   }

   inline basic_toggle_button<> check_box(std::string text)
   {
      return basic_toggle_button(
         check_box_element<false>{ text }
       , check_box_element<true>{ text }
      );
   }
}}

#endif
