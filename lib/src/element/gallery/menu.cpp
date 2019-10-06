/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/gallery/menu.hpp>

namespace cycfi { namespace elements
{
   basic_menu
   button_menu(std::string const& text, menu_position pos, color body_color)
   {
      auto icon =
         (pos == menu_position::bottom_right || pos == menu_position::bottom_left)?
         icons::down_dir : icons::up_dir
         ;
      auto menu = make_button<basic_menu>(text, icon, 1.0, body_color);
      menu.position(pos);
      return std::move(menu);
   }

   basic_menu
   button_menu(menu_position pos, color body_color)
   {
      auto icon =
         (pos == menu_position::bottom_right || pos == menu_position::bottom_left)?
         icons::down_dir : icons::up_dir
         ;
      auto menu = make_button<basic_menu>(icon, 1.0, body_color);
      menu.position(pos);
      return std::move(menu);
   }

   namespace
   {
      void draw_menu_background(cairo_t& _context, rect bounds, float radius)
      {
         auto x = bounds.left;
         auto y = bounds.top;
         auto r = bounds.right;
         auto b = bounds.bottom;
         auto const a = M_PI/180.0;

         cairo_new_sub_path(&_context);
         cairo_move_to(&_context, r, y);
         cairo_arc(&_context, r-radius, b-radius, radius, 0*a, 90*a);
         cairo_arc(&_context, x+radius, b-radius, radius, 90*a, 180*a);
         cairo_line_to(&_context, x, y);
         cairo_close_path(&_context);
      }
   }

   void menu_background::draw(context const& ctx)
   {
      auto&       canvas_ = ctx.canvas;
      auto const& bounds = ctx.bounds;

      canvas_.begin_path();
      draw_menu_background(canvas_.cairo_context(), bounds, 5);
      canvas_.fill_style(get_theme().panel_color.opacity(0.8));
      canvas_.fill();
   }

   view_limits menu_item_spacer_element::limits(basic_context const& ctx) const
   {
      return { { 0, 0 }, { full_extent, get_theme().label_font_size } };
   }

   void menu_item_spacer_element::draw(context const& ctx)
   {
      auto& canvas_ = ctx.canvas;
      float y = ctx.bounds.top + ctx.bounds.height() / 2;

      canvas_.begin_path();
      canvas_.move_to({ ctx.bounds.left, y });
      canvas_.line_to({ ctx.bounds.right, y });
      canvas_.stroke_style(get_theme().frame_color.opacity(0.15));
      canvas_.line_width(1);
      canvas_.stroke();
   }
}}
