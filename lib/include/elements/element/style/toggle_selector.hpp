/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_STYLE_TOGGLE_SELECTOR_JUNE_5_2016)
#define ELEMENTS_STYLE_TOGGLE_SELECTOR_JUNE_5_2016

#include <elements/support/theme.hpp>
#include <elements/element/element.hpp>
#include <elements/support/text_utils.hpp>
#include <string>
#include <utility>

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // toggle_selector (e.g. check_box and radio_button where the small button
   // at the left followed by some text at the right of the button.)
   ////////////////////////////////////////////////////////////////////////////
   struct toggle_selector : element
   {
                              toggle_selector(std::string text)
                               : _text(std::move(text))
                              {}

      view_limits             limits(basic_context const& ctx) const override;
      bool                    cursor(context const& ctx, point p, cursor_tracking status) override;
      bool                    wants_control() const override;

      std::string             _text;
   };

   inline view_limits toggle_selector::limits(basic_context const& ctx) const
   {
      auto& thm = get_theme();
      auto  size = measure_text(ctx.canvas, _text.c_str(), thm.label_font);
      size.x += 15 + size.y + 10 + 15;
      return {{size.x, size.y}, {size.x, size.y}};
   }

   inline bool toggle_selector::cursor(context const& ctx, point /* p */, cursor_tracking /* status */)
   {
      ctx.view.refresh(ctx);
      return true;
   }

   inline bool toggle_selector::wants_control() const
   {
      return true;
   }
}

#endif
