/*=============================================================================
   Copyright (c) 2020 Michal Urbanski

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_GALLERY_TAB_MAY_02_2020)
#define ELEMENTS_GALLERY_TAB_MAY_02_2020

#include <elements/element/gallery/button.hpp>
#include <elements/element/gallery/toggle_selector.hpp>
#include <elements/element/selectable.hpp>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Radio Button
   ////////////////////////////////////////////////////////////////////////////
   class basic_tab : public basic_latching_button<>, public selectable
   {
   public:

      using basic_latching_button::basic_latching_button;

      void              select(bool state) override;
      bool              is_selected() const override;
      element*          click(context const& ctx, mouse_button btn) override;
   };

   void draw_tab(
      context const& ctx, std::string const& text, bool state, bool hilite
   );

   template <bool state>
   struct tab_element : toggle_selector
   {
      using toggle_selector::toggle_selector;

      void                    draw(context const& ctx) override;
   };

   template <bool state>
   void tab_element<state>::draw(context const& ctx)
   {
      draw_tab(ctx, _text, state, ctx.bounds.includes(ctx.view.cursor_pos()));
   }

   inline basic_tab tab(std::string text)
   {
      return basic_tab(
         tab_element<false>{ text }
       , tab_element<true>{ text }
      );
   }
}}

#endif
