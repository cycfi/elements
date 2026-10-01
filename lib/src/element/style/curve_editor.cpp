/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/style/curve_editor.hpp>
#include <elements/support/context.hpp>
#include <elements/view.hpp>

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // curve_lines
   ////////////////////////////////////////////////////////////////////////////
   curve_lines::curve_lines()
    : curve_lines(
         get_theme().indicator_color
       , get_theme().indicator_color.opacity(0.15))
   {}

   curve_lines::curve_lines(color line, color fill)
    : line_color(line)
    , fill_color(fill)
    , floor_color(get_theme().ticks_color)
   {}

   view_limits curve_lines::limits(basic_context const& /*ctx*/) const
   {
      return {min_size, {full_extent, full_extent}};
   }

   void curve_lines::draw(context const& ctx)
   {
      auto e = find_parent<basic_curve_editor*>(ctx);
      if (!e || e->size() == 0)
         return;

      auto& cnv = ctx.canvas;
      auto state = cnv.new_state();
      auto const b = e->plot(ctx);
      auto const pts = e->positions();
      auto at = [&](point p) { return e->to_screen(p, b); };

      // The guides: across at each height, down through each point
      cnv.line_width(1);
      cnv.stroke_style(floor_color);
      cnv.begin_path();
      for (auto y : horizontal_guides)
      {
         auto const sy = at({0, y}).y;
         cnv.move_to({ctx.bounds.left, sy});
         cnv.line_to({ctx.bounds.right, sy});
      }
      for (auto i : vertical_guides)
      {
         if (i >= pts.size())
            continue;
         auto const sx = at(pts[i]).x;
         cnv.move_to({sx, b.top});
         cnv.line_to({sx, b.bottom});
      }
      cnv.stroke();

      // The curve from the first point, each segment straight or bent
      auto trace = [&]()
      {
         cnv.move_to(at(pts[0]));
         for (std::size_t i = 1; i != pts.size(); ++i)
         {
            auto const p0 = pts[i - 1];
            auto const p1 = pts[i];
            if (shape)
            {
               for (int k = 1; k < steps; ++k)
               {
                  auto const t = float(k) / steps;
                  auto const y = p0.y + (p1.y - p0.y) * shape(i - 1, t);
                  cnv.line_to(at({p0.x + (p1.x - p0.x) * t, y}));
               }
            }
            cnv.line_to(at(p1));
         }
      };

      if (fill_color.alpha > 0)
      {
         cnv.begin_path();
         trace();
         cnv.line_to({at(pts.back()).x, b.bottom});
         cnv.line_to({at(pts.front()).x, b.bottom});
         cnv.close_path();
         cnv.fill_style(fill_color);
         cnv.fill();
      }

      cnv.begin_path();
      trace();
      cnv.line_width(line_width);
      cnv.stroke_style(line_color);
      cnv.stroke();
   }

   ////////////////////////////////////////////////////////////////////////////
   // curve_handle
   ////////////////////////////////////////////////////////////////////////////
   curve_handle::curve_handle()
    : curve_handle(
         get_theme().indicator_color
       , get_theme().panel_color.opacity(1.0))
   {}

   curve_handle::curve_handle(color ring, color interior)
    : ring_color(ring)
    , interior_color(interior)
   {}

   void curve_handle::draw(context const& ctx)
   {
      auto e = find_parent<basic_curve_editor*>(ctx);
      if (!e)
         return;

      // A point that cannot be taken hold of has no handle.
      if (!e->is_movable(e->drawing()))
         return;

      auto const live = e->state(e->drawing()) != basic_curve_editor::normal;
      auto const r = live? radius + 2 : radius;
      auto const c = center_point(ctx.bounds);

      auto& cnv = ctx.canvas;
      cnv.begin_path();
      cnv.add_circle({c.x, c.y, r});
      cnv.fill_style(live? ring_color : interior_color);
      cnv.fill_preserve();
      cnv.line_width(line_width);
      cnv.stroke_style(ring_color);
      cnv.stroke();
   }
}
