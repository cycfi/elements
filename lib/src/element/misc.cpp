/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/misc.hpp>

namespace cycfi { namespace elements
{
   void panel::draw(context const& ctx)
   {
      draw_panel(
         ctx.canvas
       , ctx.bounds
       , get_theme().panel_color.opacity(_opacity)
       , 4.0
      );
   }

   void frame::draw(context const& ctx)
   {
      auto const&    theme_ = get_theme();
      auto&          canvas_ = ctx.canvas;
      auto const&    bounds = ctx.bounds;

      canvas_.line_width(theme_.frame_stroke_width);
      canvas_.stroke_style(colors::black.opacity(0.4));
      canvas_.stroke_round_rect(bounds.move(-1, -1), theme_.frame_corner_radius);
      canvas_.stroke_style(theme_.frame_color);
      canvas_.stroke_round_rect(bounds, theme_.frame_corner_radius);
   }

   view_limits default_label::limits(basic_context const& ctx) const
   {
      auto  size = measure_text(
         ctx.canvas, c_str()
       , font()
       , font_size()
      );
      return { { size.x, size.y }, { size.x, size.y } };
   }

   void default_label::draw(context const& ctx)
   {
      auto& canvas_ = ctx.canvas;
      auto  state = canvas_.new_state();
      auto  align = text_align();

      // default should reflect the theme's vertical label_text_align
      if ((align & 0x1C) == 0)
         align |= get_theme().label_text_align & 0x1C;

      canvas_.fill_style(font_color());
      canvas_.font(font(), font_size());

      float cx = ctx.bounds.left + (ctx.bounds.width() / 2);
      switch (align & 0x3)
      {
         case canvas::left:
            cx = ctx.bounds.left;
            break;
         case canvas::center:
            break;
         case canvas::right:
            cx = ctx.bounds.right;
            break;
      }

      float cy = ctx.bounds.top + (ctx.bounds.height() / 2);
      switch (align & 0x1C)
      {
         case canvas::top:
            cy = ctx.bounds.top;
            break;
         case canvas::middle:
            break;
         case canvas::bottom:
            cy = ctx.bounds.bottom;
            break;
      }

      canvas_.text_align(align);
      canvas_.fill_text({ cx, cy }, c_str());
   }

   void vgrid_lines::draw(context const& ctx)
   {
      auto const&    theme_ = get_theme();
      auto&          canvas_ = ctx.canvas;
      auto const&    bounds = ctx.bounds;

      float pos = bounds.top;
      float incr = bounds.height() / _major_divisions;

      canvas_.stroke_style(theme_.major_grid_color);
      canvas_.line_width(theme_.major_grid_width);
      while (pos <= bounds.bottom+1)
      {
         canvas_.move_to({ bounds.left, pos });
         canvas_.line_to({ bounds.right, pos });
         canvas_.stroke();
         pos += incr;
      }

      pos = bounds.top;
      incr = bounds.height() / _minor_divisions;

      canvas_.stroke_style(theme_.minor_grid_color);
      canvas_.line_width(theme_.minor_grid_width);
      while (pos <= bounds.bottom+1)
      {
         canvas_.move_to({ bounds.left, pos });
         canvas_.line_to({ bounds.right, pos });
         canvas_.stroke();
         pos += incr;
      }
   }

   icon::icon(std::uint32_t code_, float size_)
    : _code(code_)
    , _size(size_)
   {}

   view_limits icon::limits(basic_context const& ctx) const
   {
      auto& thm = get_theme();
      float font_size = thm.icon_font_size * _size;
      point s = measure_icon(ctx.canvas, _code, font_size);
      return { { s.x, s.y }, { s.x, s.y } };
   }

   void icon::draw(context const& ctx)
   {
      auto& thm = get_theme();
      float font_size = thm.icon_font_size * _size;
      draw_icon(ctx.canvas, ctx.bounds, _code, font_size);
   }
}}

