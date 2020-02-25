/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_GALLERY_CHECK_BOX_JUNE_5_2016)
#define ELEMENTS_GALLERY_CHECK_BOX_JUNE_5_2016

#include <elements/element/gallery/button.hpp>
#include <elements/support/theme.hpp>
#include <string>
#include <utility>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Check Box
   ////////////////////////////////////////////////////////////////////////////
   void draw_check_box(
      context const& ctx, std::string const& text, bool state, bool hilite
   );

   template <bool state>
   struct check_box_element : element
   {
                              check_box_element(std::string text)
                               : _text(std::move(text))
                              {}

      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;

      std::string             _text;
   };

   template <bool state>
   view_limits check_box_element<state>::limits(basic_context const& ctx) const
   {
      auto& thm = get_theme();
      auto  size = measure_text(ctx.canvas, _text.c_str(), thm.label_font, thm.label_font_size);
      size.x += 15 + size.y + 10 + 15;
      return { { size.x, size.y }, { size.x, size.y } };
   }

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
