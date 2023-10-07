/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/gallery/button.hpp>
#include <utility>

namespace cycfi { namespace elements
{
   void draw_button_base(
      context const& ctx, rect bounds, color color_, bool enabled, float corner_radius)
   {
      draw_button(ctx.canvas, bounds, color_, enabled, corner_radius);
   }

   void basic_button_body::draw(context const& ctx)
   {
      draw_button_base(ctx, ctx.bounds, body_color, is_enabled(), corner_radius);
   }

   layered_button
   button(
      std::uint32_t icon_code
    , float size
    , color body_color
   )
   {
      return make_button<layered_button>(icon_code, size, body_color);
   }

   layered_button
   button(
      std::uint32_t icon_code
    , std::string text
    , float size
    , color body_color
   )
   {
      return make_button<layered_button>(icon_code, std::move(text), size, body_color);
   }

   layered_button
   button(
      std::string text
    , std::uint32_t icon_code
    , float size
    , color body_color
   )
   {
      return make_button<layered_button>(std::move(text), icon_code, size, body_color);
   }

   basic_toggle_button<>
   toggle_button(
      std::uint32_t icon_code
    , float size
    , color body_color
   )
   {
      return make_button<basic_toggle_button<>>(icon_code, size, body_color);
   }

   basic_toggle_button<>
   toggle_button(
      std::uint32_t icon_code
    , std::string text
    , float size
    , color body_color
   )
   {
      return make_button<basic_toggle_button<>>(icon_code, std::move(text), size, body_color);
   }

   basic_toggle_button<>
   toggle_button(
      std::string text
    , std::uint32_t icon_code
    , float size
    , color body_color
   )
   {
      return make_button<basic_toggle_button<>>(std::move(text), icon_code, size, body_color);
   }

   basic_latching_button<>
   latching_button(
      std::uint32_t icon_code
    , float size
    , color body_color
   )
   {
      return make_button<basic_latching_button<>>(icon_code, size, body_color);
   }

   basic_latching_button<>
   latching_button(
      std::uint32_t icon_code
    , std::string text
    , float size
    , color body_color
   )
   {
      return make_button<basic_latching_button<>>(icon_code, std::move(text), size, body_color);
   }

   basic_latching_button<>
   latching_button(
      std::string text
    , std::uint32_t icon_code
    , float size
    , color body_color
   )
   {
      return make_button<basic_latching_button<>>(std::move(text), icon_code, size, body_color);
   }

   view_limits button_element::limits(basic_context const& ctx) const
   {
      auto const& theme = get_theme();
      auto margin = theme.button_margin;
      auto font = theme.label_font;

      auto  size = measure_text(ctx.canvas, _text.c_str(), font.size(font._size * _size));
      auto x_space = margin.left + margin.right;
      auto y_space = margin.top + margin.bottom;
      return {
         { size.x + x_space, size.y + y_space }
       , { full_extent, size.y + y_space }
      };
   }

   bool button_element::cursor(context const& ctx, point /*p*/, cursor_tracking /*status*/)
   {
      ctx.view.refresh(ctx);
      return true;
   }

   bool button_element::wants_control() const
   {
      return true;
   }

   void button_element::draw(context const& ctx)
   {
      auto& canvas_ = ctx.canvas;
      auto canvas_state = canvas_.new_state();
      auto const& theme = get_theme();
      auto bounds = ctx.bounds;

      auto state = value();
      auto value = state.value;
      auto hilite = state.hilite;
      auto tracking = state.tracking;
      auto enabled = state.enabled;

      if (value)
         bounds = bounds.move(1, 1);

      draw_button_base(ctx, bounds, _body_color, enabled, theme.button_corner_radius);

      // Draw text
      auto text_c = enabled?
         theme.label_font_color :
         theme.label_font_color.opacity(theme.label_font_color.alpha * theme.disabled_opacity)
         ;

      auto font = theme.label_font;
      if (hilite && enabled)
         text_c = text_c.level(1.5);

      canvas_.fill_style(text_c);
      canvas_.font(font);
      canvas_.text_align(canvas_.center | canvas_.middle);
      canvas_.fill_text(_text.c_str(), center_point(bounds));
   }
}}
