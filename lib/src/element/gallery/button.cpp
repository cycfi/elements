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

   // layered_button
   // button(
   //    std::uint32_t icon_code
   //  , float size
   //  , color body_color
   // )
   // {
   //    return make_button<layered_button>(icon_code, size, body_color);
   // }

//   layered_button
//   button(
//      std::uint32_t icon_code
//    , std::string text
//    , float size
//    , color body_color
//   )
//   {
//      return make_button<layered_button>(icon_code, std::move(text), size, body_color);
//   }

//   layered_button
//   button(
//      std::string text
//    , std::uint32_t icon_code
//    , float size
//    , color body_color
//   )
//   {
//      return make_button<layered_button>(std::move(text), icon_code, size, body_color);
//   }

//    basic_toggle_button<>
//    toggle_button(
//       std::uint32_t icon_code
//     , float size
//     , color body_color
//    )
//    {
//       return make_button<basic_toggle_button<>>(icon_code, size, body_color);
//    }

//   basic_toggle_button<>
//   toggle_button(
//      std::uint32_t icon_code
//    , std::string text
//    , float size
//    , color body_color
//   )
//   {
//      return make_button<basic_toggle_button<>>(icon_code, std::move(text), size, body_color);
//   }

//   basic_toggle_button<>
//   toggle_button(
//      std::string text
//    , std::uint32_t icon_code
//    , float size
//    , color body_color
//   )
//   {
//      return make_button<basic_toggle_button<>>(std::move(text), icon_code, size, body_color);
//   }

   // basic_latching_button<>
   // latching_button(
   //    std::uint32_t icon_code
   //  , float size
   //  , color body_color
   // )
   // {
   //    return make_button<basic_latching_button<>>(icon_code, size, body_color);
   // }

//   basic_latching_button<>
//   latching_button(
//      std::uint32_t icon_code
//    , std::string text
//    , float size
//    , color body_color
//   )
//   {
//      return make_button<basic_latching_button<>>(icon_code, std::move(text), size, body_color);
//   }
//
//   basic_latching_button<>
//   latching_button(
//      std::string text
//    , std::uint32_t icon_code
//    , float size
//    , color body_color
//   )
//   {
//      return make_button<basic_latching_button<>>(std::move(text), icon_code, size, body_color);
//   }

   view_limits button_element::limits(basic_context const& ctx) const
   {
      auto const& theme = get_theme();
      auto margin = theme.button_margin;
      auto font = theme.label_font;

      // Measure the text width
      auto size = measure_text(ctx.canvas, _text.c_str(), font.size(font._size * _size));

      // Add space for the icon if necessary
      if (_icon_placement != icon_none)
         size.x += (_size * theme.icon_font._size) + 8;

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
      auto enabled = state.enabled;
      auto body_color = _body_color;

      // Draw the body
      if (value)
      {
         bounds = bounds.move(1, 1);
         body_color = body_color.opacity(0.5);
      }
      else
      {
         body_color = body_color.level(0.9);
      }
      draw_button_base(ctx, bounds, body_color, enabled, theme.button_corner_radius);

      // Adjust the font size
      auto font = theme.label_font;
      font = font.size(font._size * _size);

      // Measure text and icon
      auto text_size = measure_text(ctx.canvas, _text.c_str(), font.size(font._size * _size));
      auto icon_space = (_size * theme.icon_font._size) + 8;
      auto size = text_size;

      if (_icon_placement != icon_none)
         size.x += icon_space;

      auto text_c = enabled?
         theme.label_font_color :
         theme.label_font_color.opacity(theme.label_font_color.alpha * theme.disabled_opacity)
         ;

      if (hilite && enabled)
         text_c = text_c.level(1.5);

      float text_offset = -size.x/2;
      if (_icon_placement == icon_left)
         text_offset += icon_space;

      // Draw text
      auto text_pos = center_point(bounds).move(text_offset, 0);
      canvas_.fill_style(text_c);
      canvas_.font(font);
      canvas_.text_align(canvas_.left | canvas_.middle);
      canvas_.fill_text(_text.c_str(), text_pos);

      // Draw icon
      if (_icon_placement != icon_none)
      {
         auto icon_pos = text_pos.move((_icon_placement == icon_left)? -icon_space : text_size.x + 8, 0);
         auto icon_font = theme.icon_font;
         canvas_.font(icon_font.size(_size * icon_font._size));
         canvas_.fill_text(codepoint_to_utf8(_icon_code).c_str(), icon_pos);
      }
   }
}}

