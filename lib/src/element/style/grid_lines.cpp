/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/style/grid_lines.hpp>
#include <elements/support/theme.hpp>

namespace cycfi::elements
{
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
         canvas_.move_to({bounds.left, pos});
         canvas_.line_to({bounds.right, pos});
         canvas_.stroke();
         pos += incr;
      }

      pos = bounds.top;
      incr = bounds.height() / _minor_divisions;

      canvas_.stroke_style(theme_.minor_grid_color);
      canvas_.line_width(theme_.minor_grid_width);
      while (pos <= bounds.bottom+1)
      {
         canvas_.move_to({bounds.left, pos});
         canvas_.line_to({bounds.right, pos});
         canvas_.stroke();
         pos += incr;
      }
   }

   void hgrid_lines::draw(context const& ctx)
   {
      auto const& theme_ = get_theme();
      auto& canvas_ = ctx.canvas;
      auto const& bounds = ctx.bounds;

      float pos = bounds.left;
      float incr = bounds.width() / _major_divisions;

      canvas_.stroke_style(theme_.major_grid_color);
      canvas_.line_width(theme_.major_grid_width);
      while (pos <= bounds.right + 1)
      {
         canvas_.move_to({pos, bounds.top});
         canvas_.line_to({pos, bounds.bottom});
         canvas_.stroke();
         pos += incr;
      }

      pos = bounds.left;
      incr = bounds.width() / _minor_divisions;

      canvas_.stroke_style(theme_.minor_grid_color);
      canvas_.line_width(theme_.minor_grid_width);
      while (pos <= bounds.right + 1)
      {
         canvas_.move_to({pos, bounds.top});
         canvas_.line_to({pos, bounds.bottom});
         canvas_.stroke();
         pos += incr;
      }
   }
}

