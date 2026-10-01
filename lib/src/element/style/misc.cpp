/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/style/misc.hpp>
#include <elements/support/text_utils.hpp>

namespace cycfi::elements
{
   namespace colors = artist::colors;

   void panel::draw(context const& ctx)
   {
      draw_panel(
         ctx.canvas
       , ctx.bounds
       , get_theme().panel_color.opacity(_opacity)
       , 4.0
      );
   }

   frame::frame(color color_, float width, float radius)
    : _custom(true)
    , _color(color_)
    , _width(width)
    , _radius(radius)
   {}

   void frame::draw(context const& ctx)
   {
      auto const&    theme_ = get_theme();
      auto&          canvas_ = ctx.canvas;
      auto const&    bounds = ctx.bounds;

      // A given color: one outline, inside the bounds
      if (_custom)
      {
         auto const half = _width / 2;
         canvas_.line_width(_width);
         canvas_.stroke_style(_color);
         canvas_.stroke_round_rect(bounds.inset(half, half), _radius);
         return;
      }

      canvas_.line_width(theme_.frame_stroke_width);
      canvas_.stroke_style(colors::black.opacity(0.4));
      canvas_.stroke_round_rect(bounds.move(-1, -1), theme_.frame_corner_radius);
      canvas_.stroke_style(theme_.frame_color);
      canvas_.stroke_round_rect(bounds, theme_.frame_corner_radius);
   }

   icon::icon(std::uint32_t code_, float size_)
    : _code(code_)
    , _size(size_)
   {}

   view_limits icon::limits(basic_context const& ctx) const
   {
      auto& thm = get_theme();
      float font_size = thm.icon_font._size * _size;
      point s = measure_icon(ctx.canvas, _code, font_size);
      return {{s.x, s.y}, {s.x, s.y}};
   }

   void icon::draw(context const& ctx)
   {
      auto& thm = get_theme();
      float font_size = thm.icon_font._size * _size;
      draw_icon(ctx.canvas, ctx.bounds, _code, font_size);
   }
}

