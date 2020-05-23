/*=============================================================================
   Copyright (c) 2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_GALLERY_TAB_MAY_02_2020)
#define ELEMENTS_GALLERY_TAB_MAY_02_2020

#include <elements/element/gallery/button.hpp>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Radio Button
   ////////////////////////////////////////////////////////////////////////////
   void draw_tab(
      context const& ctx, std::string const& text, bool state, bool hilite
   );

   struct tab_element_base : element
   {
                              tab_element_base(std::string text)
                               : _text(std::move(text))
                              {}

      view_limits             limits(basic_context const& ctx) const override;
      bool                    cursor(context const& ctx, point p, cursor_tracking status) override;
      bool                    wants_control() const override;

      std::string             _text;

   };

   template <bool state>
   struct tab_element : tab_element_base
   {
      using tab_element_base::tab_element_base;

      void                    draw(context const& ctx) override;
   };

   template <bool state>
   void tab_element<state>::draw(context const& ctx)
   {
      draw_tab(ctx, _text, state, ctx.bounds.includes(ctx.view.cursor_pos()));
   }

   inline basic_choice tab(std::string text)
   {
      return basic_choice(
         tab_element<false>{ text }
       , tab_element<true>{ text }
      );
   }
}}

#endif
