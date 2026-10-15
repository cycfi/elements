/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/swatch_grid.hpp>
#include <elements/element/popup.hpp>
#include <elements/support/context.hpp>
#include <elements/view.hpp>
#include <algorithm>

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // basic_swatch_grid
   ////////////////////////////////////////////////////////////////////////////
   basic_swatch_grid::basic_swatch_grid(
      element_ptr highlighter, int columns, int rows)
    : _highlighter(std::move(highlighter))
    , _columns(std::max(columns, 1))
    , _rows(std::max(rows, 1))
   {}

   void basic_swatch_grid::value(int i)
   {
      _value = std::clamp(i, 0, size() - 1);
   }

   // The hot cell first: the one a click would pick
   basic_swatch_grid::swatch_state basic_swatch_grid::state(int i) const
   {
      if (i == _hot)
         return hot;
      return i == _value? current : normal;
   }

   rect basic_swatch_grid::cell_bounds(context const& ctx, int i) const
   {
      auto const w = ctx.bounds.width() / _columns;
      auto const h = ctx.bounds.height() / _rows;
      auto const x = ctx.bounds.left + (i % _columns) * w;
      auto const y = ctx.bounds.top + (i / _columns) * h;
      return {x, y, x + w, y + h};
   }

   int basic_swatch_grid::cell_at(context const& ctx, point p) const
   {
      if (!ctx.bounds.includes(p))
         return -1;
      auto const col = int((p.x - ctx.bounds.left)
         / (ctx.bounds.width() / _columns));
      auto const row = int((p.y - ctx.bounds.top)
         / (ctx.bounds.height() / _rows));
      return std::min(row, _rows - 1) * _columns
         + std::min(col, _columns - 1);
   }

   void basic_swatch_grid::highlight(context const& ctx, int i, rect bounds)
   {
      if (!_highlighter)
         return;
      _drawing = i;
      context hctx{ctx, _highlighter.get(), bounds};
      _highlighter->draw(hctx);
   }

   // The content, then the highlighter over each cell
   void basic_swatch_grid::draw(context const& ctx)
   {
      proxy_base::draw(ctx);
      for (int i = 0; i != size(); ++i)
         highlight(ctx, i, cell_bounds(ctx, i));
   }

   element* basic_swatch_grid::hit_test(
      context const& ctx, point p, bool /*leaf*/, bool /*control*/)
   {
      return ctx.bounds.includes(p)? this : nullptr;
   }

   // A pick is a press and a release on the same cell. In a popup, the
   // popup closes, as it does for a menu item.
   bool basic_swatch_grid::click(context const& ctx, mouse_button btn)
   {
      auto const at = cell_at(ctx, btn.pos);
      if (btn.down)
      {
         _pressed = at;
         return at >= 0;
      }

      auto const picked = at >= 0 && at == _pressed;
      _pressed = -1;
      if (!picked)
         return false;

      auto const changed = at != value();
      value(at);
      ctx.view.refresh(ctx);
      if (changed && on_change)
         on_change(at);
      if (auto popup = find_parent<basic_popup_element*>(ctx))
         popup->close(ctx.view);
      return true;
   }

   bool basic_swatch_grid::cursor(
      context const& ctx, point p, cursor_tracking status)
   {
      auto const was = _hot;
      _hot = (status == cursor_tracking::leaving)? -1 : cell_at(ctx, p);
      if (_hot != was)
         ctx.view.refresh(ctx);
      return _hot >= 0;
   }

   std::string basic_swatch_grid::class_name() const
   {
      return "swatch_grid";
   }

   ////////////////////////////////////////////////////////////////////////////
   // basic_swatch_cell
   ////////////////////////////////////////////////////////////////////////////
   // Only the current cell is shown, and nothing is hot.
   basic_swatch_cell::swatch_state basic_swatch_cell::state(int i) const
   {
      return i == value()? current : normal;
   }

   // Fitted to the space it is given, it has no size of its own.
   view_limits basic_swatch_cell::limits(basic_context const&) const
   {
      return full_limits;
   }

   // The current cell, as large as fits, its proportions kept, centered:
   // the content drawn at its own size, scaled and moved so that the cell
   // lands on the space given, and clipped to it.
   void basic_swatch_cell::draw(context const& ctx)
   {
      auto& cnv = ctx.canvas;
      auto const content = subject().limits(ctx).min;
      point const whole =
         content.x > 0 && content.y > 0? content : point{
            ctx.bounds.width() * columns(), ctx.bounds.height() * rows()};
      point const cell = {whole.x / columns(), whole.y / rows()};

      auto const s = std::min(
         ctx.bounds.width() / cell.x, ctx.bounds.height() / cell.y);
      auto const dest = center(rect{0, 0, cell.x * s, cell.y * s}, ctx.bounds);
      auto const i = value();
      point const at = {(i % columns()) * cell.x, (i / columns()) * cell.y};

      {
         auto state = cnv.new_state();
         cnv.begin_path();
         cnv.add_rect(dest);
         cnv.clip();
         cnv.translate(dest.left - at.x * s, dest.top - at.y * s);
         cnv.scale(s, s);
         context sctx{ctx, &subject(), rect{0, 0, whole.x, whole.y}};
         subject().draw(sctx);
      }
      highlight(ctx, i, dest);
   }

   element* basic_swatch_cell::hit_test(
      context const&, point, bool, bool)
   {
      return nullptr;
   }

   std::string basic_swatch_cell::class_name() const
   {
      return "swatch_cell";
   }
}
