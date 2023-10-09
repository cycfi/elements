/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/gallery/icon_button.hpp>
#include <elements/support/text_utils.hpp>

namespace cycfi { namespace elements
{
   void draw_plain_icon_button(
      context const& ctx
    , uint32_t code
    , float size
    , bool state
    , bool hilite
    , bool enabled
   )
   {
      auto const& theme = get_theme();
      canvas& canvas_ = ctx.canvas;
      rect bounds = ctx.bounds;

      // Draw Icon
      color icon_color = (enabled && state)?
         theme.icon_color.level(1.5) :
         theme.icon_color.level(0.9);

      if (!enabled)
         icon_color = icon_color.opacity(icon_color.alpha * theme.disabled_opacity);
      else if (hilite)
         icon_color = icon_color.level(1.5);

      draw_icon(canvas_, bounds.move(0.5, 0.5), code, size, icon_color);
   }

   void draw_icon_button(
      context const& ctx
    , uint32_t code
    , float size
    , color body_color_
    , bool state
    , bool hilite
    , bool enabled
   )
   {
      // Draw Button Body
      float corner_radius = 6;
      color body_color = state? body_color_ : body_color_.opacity(0.5).level(0.8);
      draw_button_base(ctx, ctx.bounds, body_color, enabled, corner_radius);

      // Draw Icon
      draw_plain_icon_button(ctx, code, size, state, hilite, enabled);
   }

   view_limits icon_button_styler_base::limits(basic_context const& /*ctx*/) const
   {
      auto  size = _size * get_theme().icon_font._size * 1.8f;
      return {{size, size}, {size, size}};
   }

   void icon_button_styler::draw(context const& ctx)
   {
      auto  state = value();
      auto  value = state.value;
      auto  hilite = state.hilite;
      auto  enabled = state.enabled;

      draw_icon_button(
         ctx, _code, _size * get_theme().icon_font._size
         , _body_color, value, hilite, enabled
      );
   }

   void  icon_button_styler2::draw(context const& ctx)
   {
      auto  state = value();
      auto  value = state.value;
      auto  hilite = state.hilite;
      auto  enabled = state.enabled;

      draw_icon_button(
         ctx, value? _code2 : _code1, _size * get_theme().icon_font._size
         , _body_color, true, hilite, enabled
      );
   }

   void plain_icon_button_element::draw(context const& ctx)
   {
      auto  state = value();
      auto  value = state.value;
      auto  enabled = state.enabled;
      auto  hilite = state.hilite;

      draw_plain_icon_button(
         ctx, _code, _size * get_theme().icon_font._size, value, hilite, enabled
      );
   }
}}
