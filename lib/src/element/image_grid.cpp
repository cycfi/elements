/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/image_grid.hpp>
#include <elements/element/popup.hpp>
#include <elements/support/context.hpp>
#include <elements/support/error_handler.hpp>
#include <elements/view.hpp>
#include <algorithm>
#include <stdexcept>

namespace cycfi::elements
{
   namespace
   {
      image_ptr checked(image_ptr img, std::string const& what)
      {
         if (!img || !img->impl())
         {
            error_handler::get().on_resource_error(
               error_id::image_load_failed, "invalid image" + what);
            throw std::runtime_error{"Error: Invalid image."};
         }
         return img;
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   // basic_image_grid
   ////////////////////////////////////////////////////////////////////////////
   basic_image_grid::basic_image_grid(image_ptr img, int columns, int rows)
    : _image(checked(std::move(img), ""))
    , _columns(std::max(columns, 1))
    , _rows(std::max(rows, 1))
   {}

   basic_image_grid::basic_image_grid(
      fs::path const& path, int columns, int rows)
    : basic_image_grid(
         checked(std::make_shared<artist::image>(path), ": " + path.string())
       , columns, rows)
   {}

   void basic_image_grid::value(int i)
   {
      _value = std::clamp(i, 0, size() - 1);
   }

   basic_image_grid::cell_state basic_image_grid::state(int i) const
   {
      return i == _value? current : normal;
   }

   point basic_image_grid::cell_size() const
   {
      auto const s = _image->size();
      return {s.x / _columns, s.y / _rows};
   }

   // The cell's share of the image into the bounds, and the styler over it
   void basic_image_grid::draw_cell(context const& ctx, int i, rect bounds)
   {
      auto const c = cell_size();
      auto const x = (i % _columns) * c.x;
      auto const y = (i / _columns) * c.y;
      ctx.canvas.draw(*_image, rect{x, y, x + c.x, y + c.y}, bounds);

      _drawing = i;
      context cctx{ctx, &subject(), bounds};
      subject().draw(cctx);
   }

   std::string basic_image_grid::class_name() const
   {
      return "image_grid";
   }

   ////////////////////////////////////////////////////////////////////////////
   // basic_image_grid_menu
   ////////////////////////////////////////////////////////////////////////////
   basic_image_grid_menu::basic_image_grid_menu(
      image_ptr img, int columns, int rows, float scale)
    : basic_image_grid(std::move(img), columns, rows)
    , _scale(scale)
   {}

   basic_image_grid_menu::basic_image_grid_menu(
      fs::path const& path, int columns, int rows, float scale)
    : basic_image_grid(path, columns, rows)
    , _scale(scale)
   {}

   // The hot cell first: the one a click would pick
   basic_image_grid::cell_state basic_image_grid_menu::state(int i) const
   {
      if (i == _hot)
         return hot;
      return basic_image_grid::state(i);
   }

   view_limits basic_image_grid_menu::limits(basic_context const&) const
   {
      auto const s = get_image()->size();
      point const size = {s.x * _scale, s.y * _scale};
      return {size, size};
   }

   rect basic_image_grid_menu::cell_bounds(context const& ctx, int i) const
   {
      auto const w = ctx.bounds.width() / columns();
      auto const h = ctx.bounds.height() / rows();
      auto const x = ctx.bounds.left + (i % columns()) * w;
      auto const y = ctx.bounds.top + (i / columns()) * h;
      return {x, y, x + w, y + h};
   }

   int basic_image_grid_menu::cell_at(context const& ctx, point p) const
   {
      if (!ctx.bounds.includes(p))
         return -1;
      auto const col = int((p.x - ctx.bounds.left)
         / (ctx.bounds.width() / columns()));
      auto const row = int((p.y - ctx.bounds.top)
         / (ctx.bounds.height() / rows()));
      return std::min(row, rows() - 1) * columns()
         + std::min(col, columns() - 1);
   }

   void basic_image_grid_menu::draw(context const& ctx)
   {
      for (int i = 0; i != size(); ++i)
         draw_cell(ctx, i, cell_bounds(ctx, i));
   }

   element* basic_image_grid_menu::hit_test(
      context const& ctx, point p, bool /*leaf*/, bool /*control*/)
   {
      return ctx.bounds.includes(p)? this : nullptr;
   }

   // A pick is a press and a release on the same cell. In a popup, the
   // popup closes, as it does for a menu item.
   bool basic_image_grid_menu::click(context const& ctx, mouse_button btn)
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

   bool basic_image_grid_menu::cursor(
      context const& ctx, point p, cursor_tracking status)
   {
      auto const was = _hot;
      _hot = (status == cursor_tracking::leaving)? -1 : cell_at(ctx, p);
      if (_hot != was)
         ctx.view.refresh(ctx);
      return _hot >= 0;
   }

   std::string basic_image_grid_menu::class_name() const
   {
      return "image_grid_menu";
   }

   ////////////////////////////////////////////////////////////////////////////
   // basic_image_grid_cell
   ////////////////////////////////////////////////////////////////////////////
   view_limits basic_image_grid_cell::limits(basic_context const&) const
   {
      return {{32, 32}, {full_extent, full_extent}};
   }

   // The current cell, as large as fits, its proportions kept, centered
   void basic_image_grid_cell::draw(context const& ctx)
   {
      auto const c = cell_size();
      auto const s = std::min(
         ctx.bounds.width() / c.x, ctx.bounds.height() / c.y);
      auto dest = rect{0, 0, c.x * s, c.y * s};
      draw_cell(ctx, value(), center(dest, ctx.bounds));
   }

   std::string basic_image_grid_cell::class_name() const
   {
      return "image_grid_cell";
   }
}
