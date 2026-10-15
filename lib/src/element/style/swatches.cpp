/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/style/swatches.hpp>
#include <elements/element/traversal.hpp>
#include <elements/support/context.hpp>

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // swatch_highlighter
   ////////////////////////////////////////////////////////////////////////////
   swatch_highlighter::swatch_highlighter()
    : swatch_highlighter(
         get_theme().indicator_color.opacity(0.25)
       , get_theme().indicator_color)
   {}

   swatch_highlighter::swatch_highlighter(color hot, color current)
    : hot_color(hot)
    , current_color(current)
   {}

   void swatch_highlighter::draw(context const& ctx)
   {
      auto g = find_parent<basic_swatches*>(ctx);
      if (!g)
         return;

      // Nothing set here reaches what is drawn next
      auto& cnv = ctx.canvas;
      auto saved = cnv.new_state();
      auto const b = ctx.bounds.inset(line_width, line_width);
      auto const s = g->state(g->drawing());
      if (s == basic_swatches::hot)
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
   // region_highlighter
   ////////////////////////////////////////////////////////////////////////////
   region_highlighter::region_highlighter(regions_type regions_, state_function state_)
    : regions(std::move(regions_))
    , state(std::move(state_))
    , lit_color(get_theme().indicator_hilite_color)
    , dim_color(get_theme().panel_color.opacity(0.7))
   {}

   // Each region of the swatch drawn, mapped from its unit square
   void region_highlighter::draw(context const& ctx)
   {
      auto g = find_parent<basic_swatches*>(ctx);
      if (!g || !state)
         return;
      auto const swatch = g->drawing();
      if (swatch < 0 || swatch >= int(regions.size()))
         return;

      auto& cnv = ctx.canvas;
      auto saved = cnv.new_state();
      auto const& b = ctx.bounds;
      auto const& list = regions[swatch];
      for (int i = 0; i != int(list.size()); ++i)
      {
         auto const s = state(swatch, i);
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
