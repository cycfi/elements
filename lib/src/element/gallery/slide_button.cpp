/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/gallery/slide_button.hpp>

namespace cycfi { namespace elements
{
   view_limits slide_button_styler::limits(basic_context const& /*ctx*/) const
   {
      auto& thm = get_theme();
      auto  size = thm.slide_button_size;
      return {{size.x, size.y}, {size.x, size.y}};
   }

   void slide_button_styler::draw(context const& ctx)
   {
      auto& canvas_ = ctx.canvas;
      auto canvas_state = canvas_.new_state();
      auto const& theme_ = get_theme();
      auto bounds = ctx.bounds;
      auto height = bounds.height();
      auto radius = height/2;

      auto state = value();
      auto value = state.value;
      auto enabled = state.enabled;

      auto color = value?
         theme_.slide_button_on_color :
         theme_.slide_button_base_color
         ;
      if (!enabled)
         color = color.opacity(color.alpha * theme_.disabled_opacity);

      // Draw base
      canvas_.begin_path();
      canvas_.add_round_rect(bounds, radius);
      canvas_.fill_style(color);
      canvas_.fill();

      color = theme_.slide_button_thumb_color;
      if (!enabled)
         color = color.opacity(color.alpha * theme_.disabled_opacity);

      // Animate sliding
      auto target = value? bounds.right-radius : bounds.left+radius;
      if (enabled && _xpos > bounds.left && _xpos < bounds.right)
      {
         auto diff = target - _xpos;
         constexpr auto alpha = 0.3;
         _xpos += alpha * diff;
         if (std::abs(diff) > 1.0f)
            ctx.view.refresh(ctx);
         else
            _xpos = target;
      }
      else
      {
         _xpos = target;
      }

      // Draw the thumb
      canvas_.begin_path();
      canvas_.add_circle({_xpos, bounds.top+radius, radius-1.5f});
      canvas_.fill_style(color);
      canvas_.fill();
   }

   bool slide_button_styler::wants_control() const
   {
      return true;
   }
}}
