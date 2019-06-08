/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <photon/element/gallery.hpp>
#include <photon/support/text_utils.hpp>
#include <photon/support/theme.hpp>

namespace cycfi { namespace photon
{
   void background_fill::draw(context const& ctx)
   {
      auto&  cnv = ctx.canvas;
      cnv.fill_style(_color);
      cnv.fill_rect(ctx.bounds);
   }

   view_limits heading::limits(basic_context const& ctx) const
   {
      auto& thm = get_theme();
      auto  size = measure_text(
         ctx.canvas, _text.c_str(), thm.heading_font, thm.heading_font_size * _size
      );
      return { { size.x, size.y }, { size.x, size.y } };
   }

   void heading::draw(context const& ctx)
   {
      auto const&    theme_ = get_theme();
      auto&          canvas_ = ctx.canvas;
      auto           state = canvas_.new_state();

      canvas_.fill_style(theme_.heading_font_color);
      canvas_.font(
         theme_.heading_font,
         theme_.heading_font_size * _size,
         theme_.heading_style
      );
      canvas_.text_align(canvas_.middle | canvas_.center);

      float cx = ctx.bounds.left + (ctx.bounds.width() / 2);
      float cy = ctx.bounds.top + (ctx.bounds.height() / 2);

      canvas_.fill_text(point{ cx, cy }, _text.c_str());
   }

   void title_bar::draw(context const& ctx)
   {
      draw_box_vgradient(ctx.canvas, ctx.bounds, 4.0);
   }

   view_limits label::limits(basic_context const& ctx) const
   {
      auto& thm = get_theme();
      auto  size = measure_text(
         ctx.canvas, _text.c_str(), thm.label_font, thm.label_font_size * _size
      );
      return { { size.x, size.y }, { size.x, size.y } };
   }

   void label::draw(context const& ctx)
   {
      auto const&    theme_ = get_theme();
      auto&          canvas_ = ctx.canvas;
      auto           state = canvas_.new_state();

      canvas_.fill_style(theme_.label_font_color);
      canvas_.font(
         theme_.label_font,
         theme_.label_font_size * _size,
         theme_.label_style
      );
      canvas_.text_align(canvas_.middle | canvas_.center);

      float cx = ctx.bounds.left + (ctx.bounds.width() / 2);
      float cy = ctx.bounds.top + (ctx.bounds.height() / 2);

      canvas_.fill_text(point{ cx, cy }, _text.c_str());
   }

   void panel::draw(context const& ctx)
   {
      draw_panel(ctx.canvas, ctx.bounds, get_theme().panel_color, 4.0);
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

   float basic_button_body::corner_radius = 4.0;

   void draw_button_base(context const& ctx, rect bounds, color color_, float corner_radius)
   {
      draw_button(ctx.canvas, bounds, color_, corner_radius);
   }

   void basic_button_body::draw(context const& ctx)
   {
      draw_button_base(ctx, ctx.bounds, body_color, corner_radius);
   }

   basic_button
   button(
      std::string const& text
    , color body_color
   )
   {
      return make_button<basic_button>(text, body_color);
   }

   basic_button
   button(
      std::uint32_t icon_code
    , color body_color
   )
   {
      return make_button<basic_button>(icon_code, body_color);
   }

   basic_button
   button(
      std::uint32_t icon_code
    , std::string const& text
    , color body_color
   )
   {
      return make_button<basic_button>(icon_code, text, body_color);
   }

   basic_button
   button(
      std::string const& text
    , std::uint32_t icon_code
    , color body_color
   )
   {
      return make_button<basic_button>(text, icon_code, body_color);
   }

   basic_toggle_button
   toggle_button(
      std::string const& text
    , color body_color
   )
   {
      return make_button<basic_toggle_button>(text, body_color);
   }

   basic_toggle_button
   toggle_button(
      std::uint32_t icon_code
    , color body_color
   )
   {
      return make_button<basic_toggle_button>(icon_code, body_color);
   }

   basic_toggle_button
   toggle_button(
      std::uint32_t icon_code
    , std::string const& text
    , color body_color
   )
   {
      return make_button<basic_toggle_button>(icon_code, text, body_color);
   }

   basic_toggle_button
   toggle_button(
      std::string const& text
    , std::uint32_t icon_code
    , color body_color
   )
   {
      return make_button<basic_toggle_button>(text, icon_code, body_color);
   }

   basic_latching_button
   latching_button(
      std::string const& text
    , color body_color
   )
   {
      return make_button<basic_latching_button>(text, body_color);
   }

   basic_latching_button
   latching_button(
      std::uint32_t icon_code
    , color body_color
   )
   {
      return make_button<basic_latching_button>(icon_code, body_color);
   }

   basic_latching_button
   latching_button(
      std::uint32_t icon_code
    , std::string const& text
    , color body_color
   )
   {
      return make_button<basic_latching_button>(icon_code, text, body_color);
   }

   basic_latching_button
   latching_button(
      std::string const& text
    , std::uint32_t icon_code
    , color body_color
   )
   {
      return make_button<basic_latching_button>(text, icon_code, body_color);
   }

   basic_dropdown_menu
   dropdown_menu(
      std::string const &text, color body_color
   )
   {
      return make_button<basic_dropdown_menu>(text, icons::down_dir, body_color);
   }

   void draw_check_box(
     context const& ctx, std::string const& text, bool state, bool hilite
   )
   {
      auto&       canvas_ = ctx.canvas;
      auto        canvas_state = canvas_.new_state();
      auto const& theme_ = get_theme();
      color       indicator_color = theme_.indicator_color;
      rect        box = ctx.bounds.move(15, 0);

      box.width(box.height());

      color c1 = state ? indicator_color.level(1.5) : rgba(0, 0, 0, 32);

      if (state && hilite)
         c1 = c1.level(1.3);

      if (state)
         draw_icon(canvas_, box, icons::ok, 14, c1.level(2.0));

      color outline_color = theme_.frame_color;
      canvas_.begin_path();
      canvas_.round_rect(box.inset(1, 1), 3);
      canvas_.stroke_style(outline_color);
      canvas_.stroke();

      // Pseudo glow
      canvas_.round_rect(box, 4);
      canvas_.stroke_style(outline_color.opacity(0.1));
      canvas_.stroke();

      canvas_.fill_style(theme_.label_font_color);
      canvas_.font(
         theme_.label_font,
         theme_.label_font_size,
         theme_.label_style
      );
      canvas_.text_align(canvas_.left | canvas_.middle);
      float cx = box.right + 10;
      float cy = ctx.bounds.top + (ctx.bounds.height() / 2);
      canvas_.fill_text(point{ cx, cy }, text.c_str());
   }

   void draw_icon_button(
      context const& ctx, uint32_t code, float size, bool state, bool hilite
   )
   {
      float corner_radius = 6;
      auto const& theme = get_theme();

      // Draw Button Body
      color body_color = state?
         theme.icon_button_color :
         theme.icon_button_color.opacity(0.5);
      draw_button_base(ctx, ctx.bounds, body_color, corner_radius);

      canvas& canvas_ = ctx.canvas;
      rect bounds = ctx.bounds;

      // Draw Icon
      color icon_color = hilite?
         theme.icon_color :
         theme.icon_color.level(0.6);
      draw_icon(canvas_, bounds.move(0.5, 0.5), code, size, icon_color);
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
}}
