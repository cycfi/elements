/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/gallery.hpp>

namespace photon
{
   widget_limits heading::limits(basic_context const& ctx) const
   {
      auto const&    theme_ = get_theme();
      auto           canvas_ = ctx.canvas();

      canvas_.font(theme_.heading_font, theme_.heading_font_size * _size);
      auto  info = canvas_.measure_text(_text.c_str());
      auto  height = info.ascent + info.descent + info.leading;
      return { { info.size.x, height }, { info.size.x, height } };
   }

   void heading::draw(context const& ctx)
   {
      auto const&    theme_ = get_theme();
      auto           canvas_ = ctx.canvas();
      auto           state = canvas_.new_state();

      canvas_.fill_style(theme_.heading_font_color);
      canvas_.font(theme_.heading_font, theme_.heading_font_size * _size, theme_.heading_style);
      canvas_.text_align(canvas_.middle | canvas_.center);

      float cx = ctx.bounds.left + (ctx.bounds.width() / 2);
      float cy = ctx.bounds.top + (ctx.bounds.height() / 2);

      canvas_.fill_text(point{ cx, cy }, _text.c_str());
   }

   widget_limits label::limits(basic_context const& ctx) const
   {
      auto const&    theme_ = get_theme();
      auto           canvas_ = ctx.canvas();

      canvas_.font(theme_.label_font, theme_.label_font_size * _size);
      auto  info = canvas_.measure_text(_text.c_str());
      auto  height = info.ascent + info.descent + info.leading;
      return { { info.size.x, height }, { info.size.x, height } };
   }

   void label::draw(context const& ctx)
   {
      auto const&    theme_ = get_theme();
      auto           canvas_ = ctx.canvas();
      auto           state = canvas_.new_state();

      canvas_.fill_style(theme_.label_font_color);
      canvas_.font(theme_.label_font, theme_.label_font_size * _size, theme_.label_style);
      canvas_.text_align(canvas_.middle | canvas_.center);

      float cx = ctx.bounds.left + (ctx.bounds.width() / 2);
      float cy = ctx.bounds.top + (ctx.bounds.height() / 2);

      canvas_.fill_text(point{ cx, cy }, _text.c_str());
   }

   void frame::draw(context const& ctx)
   {
      auto const&    theme_ = get_theme();
      auto           canvas_ = ctx.canvas();
      auto const&    bounds = ctx.bounds;

      canvas_.line_width(theme_.frame_stroke_width);
      canvas_.stroke_style(colors::black.opacity(0.4));
      canvas_.stroke_round_rect(bounds.move(-1, -1), theme_.frame_corner_radius);
      canvas_.stroke_style(theme_.frame_color);
      canvas_.stroke_round_rect(bounds, theme_.frame_corner_radius);
   }

   void vgrid_lines::draw(context const& ctx)
   {
      auto const&    theme_ = get_theme();
      auto           canvas_ = ctx.canvas();
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
}
