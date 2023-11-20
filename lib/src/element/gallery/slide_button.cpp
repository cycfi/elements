/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/gallery/slide_button.hpp>

namespace cycfi { namespace elements
{
   namespace colors = artist::colors;

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
      // auto hilite = state.hilite;
      // auto tracking = state.tracking;
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

      // Draw slider
      color = theme_.slide_button_button_color;
      if (!enabled)
         color = color.opacity(color.alpha * theme_.disabled_opacity);

      canvas_.begin_path();
      auto x_pos = value? bounds.right-radius : bounds.left+radius;
      canvas_.add_circle(x_pos, bounds.top+radius, radius-1.5);
      canvas_.fill_style(color);
      canvas_.fill();
   }

   bool slide_button_styler::wants_control() const
   {
      return true;
   }
}}
