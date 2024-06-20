/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/style/border.hpp>

namespace cycfi::elements
{
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

   void border::draw(context const& ctx)
   {
      auto const&    theme_ = get_theme();
      auto&          canvas_ = ctx.canvas;
      auto const&    bounds = ctx.bounds;

      canvas_.line_width(theme_.frame_stroke_width);
      canvas_.stroke_style(theme_.frame_color);
      canvas_.stroke_rect(bounds);
   }

   void border_left::draw(context const& ctx)
   {
      auto const&    theme_ = get_theme();
      auto&          canvas_ = ctx.canvas;
      auto const&    bounds = ctx.bounds;

      canvas_.line_width(theme_.frame_stroke_width);
      canvas_.stroke_style(theme_.frame_color);
      canvas_.move_to(bounds.left, bounds.top);
      canvas_.line_to(bounds.left, bounds.bottom);
      canvas_.stroke();
   }

   void border_top::draw(context const& ctx)
   {
      auto const&    theme_ = get_theme();
      auto&          canvas_ = ctx.canvas;
      auto const&    bounds = ctx.bounds;

      canvas_.line_width(theme_.frame_stroke_width);
      canvas_.stroke_style(theme_.frame_color);
      canvas_.move_to(bounds.left, bounds.top);
      canvas_.line_to(bounds.right, bounds.top);
      canvas_.stroke();
   }

   void border_right::draw(context const& ctx)
   {
      auto const&    theme_ = get_theme();
      auto&          canvas_ = ctx.canvas;
      auto const&    bounds = ctx.bounds;

      canvas_.line_width(theme_.frame_stroke_width);
      canvas_.stroke_style(theme_.frame_color);
      canvas_.move_to(bounds.right, bounds.top);
      canvas_.line_to(bounds.right, bounds.bottom);
      canvas_.stroke();
   }

   void border_bottom::draw(context const& ctx)
   {
      auto const&    theme_ = get_theme();
      auto&          canvas_ = ctx.canvas;
      auto const&    bounds = ctx.bounds;

      canvas_.line_width(theme_.frame_stroke_width);
      canvas_.stroke_style(theme_.frame_color);
      canvas_.move_to(bounds.left, bounds.bottom);
      canvas_.line_to(bounds.right, bounds.bottom);
      canvas_.stroke();
   }

   void border_top_bottom::draw(context const& ctx)
   {
      auto const&    theme_ = get_theme();
      auto&          canvas_ = ctx.canvas;
      auto const&    bounds = ctx.bounds;

      canvas_.line_width(theme_.frame_stroke_width);
      canvas_.stroke_style(theme_.frame_color);
      canvas_.move_to(bounds.left, bounds.top);
      canvas_.line_to(bounds.right, bounds.top);
      canvas_.stroke();
      canvas_.move_to(bounds.left, bounds.bottom);
      canvas_.line_to(bounds.right, bounds.bottom);
      canvas_.stroke();
   }

   void border_left_right::draw(context const& ctx)
   {
      auto const&    theme_ = get_theme();
      auto&          canvas_ = ctx.canvas;
      auto const&    bounds = ctx.bounds;

      canvas_.line_width(theme_.frame_stroke_width);
      canvas_.stroke_style(theme_.frame_color);
      canvas_.move_to(bounds.left, bounds.top);
      canvas_.line_to(bounds.left, bounds.bottom);
      canvas_.stroke();
      canvas_.move_to(bounds.right, bounds.top);
      canvas_.line_to(bounds.right, bounds.bottom);
      canvas_.stroke();
   }
}

