/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/gallery/icon_button.hpp>

namespace cycfi { namespace elements
{
   void draw_plain_icon_button(
      context const& ctx
    , uint32_t code
    , float size
    , bool hilite
   )
   {
      auto const& theme = get_theme();
      canvas& canvas_ = ctx.canvas;
      rect bounds = ctx.bounds;

      // Draw Icon
      color icon_color = hilite?
         theme.icon_color.level(1.5) :
         theme.icon_color.level(0.9);
      draw_icon(canvas_, bounds.move(0.5, 0.5), code, size, icon_color);
   }

   void draw_icon_button(
      context const& ctx
    , uint32_t code
    , float size
    , color body_color_
    , bool state
    , bool hilite
   )
   {
      float corner_radius = 6;
      auto const& theme = get_theme();

      // Draw Button Body
      color body_color = state? body_color_ : body_color_.opacity(0.5).level(0.8);
      draw_button_base(ctx, ctx.bounds, body_color, corner_radius);

      canvas& canvas_ = ctx.canvas;
      rect bounds = ctx.bounds;

      // Draw Icon
      color icon_color = hilite?
         theme.icon_color.level(1.5) :
         theme.icon_color.level(0.9);
      draw_icon(canvas_, bounds.move(0.5, 0.5), code, size, icon_color);
   }
}}
