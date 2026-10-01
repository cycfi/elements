/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/style/image_grid.hpp>
#include <elements/support/context.hpp>

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // image_grid_highlight
   ////////////////////////////////////////////////////////////////////////////
   image_grid_highlight::image_grid_highlight()
    : image_grid_highlight(
         get_theme().indicator_color.opacity(0.25)
       , get_theme().indicator_color)
   {}

   image_grid_highlight::image_grid_highlight(color hot, color current)
    : hot_color(hot)
    , current_color(current)
   {}

   void image_grid_highlight::draw(context const& ctx)
   {
      auto g = find_parent<basic_image_grid*>(ctx);
      if (!g)
         return;

      auto& cnv = ctx.canvas;
      auto const b = ctx.bounds.inset(line_width, line_width);
      auto const s = g->state(g->drawing());
      if (s == basic_image_grid::hot)
      {
         cnv.begin_path();
         cnv.add_round_rect(b, corner_radius);
         cnv.fill_style(hot_color);
         cnv.fill();
      }
      if (g->drawing() == g->value())
      {
         cnv.begin_path();
         cnv.add_round_rect(b, corner_radius);
         cnv.line_width(line_width);
         cnv.stroke_style(current_color);
         cnv.stroke();
      }
      if (label)
      {
         auto const& theme = get_theme();
         auto font = theme.label_font;
         cnv.font(font.bold());
         cnv.fill_style(theme.label_font_color);
         cnv.text_align(cnv.left | cnv.top);
         cnv.fill_text(label(g->drawing()), {b.left + 4, b.top + 2});
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   // image_regions
   ////////////////////////////////////////////////////////////////////////////
   image_regions::image_regions(regions_type regions_, state_function state_)
    : regions(std::move(regions_))
    , state(std::move(state_))
    , lit_color(get_theme().indicator_hilite_color)
    , dim_color(get_theme().panel_color.opacity(0.7))
   {}

   // Each region of the cell drawn, mapped from the cell's unit square
   void image_regions::draw(context const& ctx)
   {
      auto g = find_parent<basic_image_grid*>(ctx);
      if (!g || !state)
         return;
      auto const cell = g->drawing();
      if (cell < 0 || cell >= int(regions.size()))
         return;

      auto& cnv = ctx.canvas;
      auto const& b = ctx.bounds;
      auto const& list = regions[cell];
      for (int i = 0; i != int(list.size()); ++i)
      {
         auto const s = state(cell, i);
         if (s == normal)
            continue;

         auto const& u = list[i];
         rect const r = {
            b.left + u.left * b.width(), b.top + u.top * b.height()
          , b.left + u.right * b.width(), b.top + u.bottom * b.height()
         };
         cnv.begin_path();
         if (s == lit)
         {
            cnv.add_round_rect(r, corner_radius);
            cnv.line_width(line_width);
            cnv.stroke_style(lit_color);
            cnv.stroke();
         }
         else
         {
            cnv.add_round_rect(r, corner_radius);
            cnv.fill_style(dim_color);
            cnv.fill();
         }
      }
   }
}
