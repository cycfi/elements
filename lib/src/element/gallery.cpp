/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/gallery.hpp>
#include <elements/support/text_utils.hpp>
#include <elements/support/theme.hpp>

namespace cycfi { namespace elements
{
   float basic_button_body::corner_radius = 4.0;

   void draw_button_base(context const& ctx, rect bounds, color color_, float corner_radius)
   {
      draw_button(ctx.canvas, bounds, color_, corner_radius);
   }

   void basic_button_body::draw(context const& ctx)
   {
      draw_button_base(ctx, ctx.bounds, body_color, corner_radius);
   }

   layered_button
   button(
      std::string const& text
    , float size
    , color body_color
   )
   {
      return make_button<layered_button>(text, size, body_color);
   }

   layered_button
   button(
      std::uint32_t icon_code
    , float size
    , color body_color
   )
   {
      return make_button<layered_button>(icon_code, size, body_color);
   }

   layered_button
   button(
      std::uint32_t icon_code
    , std::string const& text
    , float size
    , color body_color
   )
   {
      return make_button<layered_button>(icon_code, text, size, body_color);
   }

   layered_button
   button(
      std::string const& text
    , std::uint32_t icon_code
    , float size
    , color body_color
   )
   {
      return make_button<layered_button>(text, icon_code, size, body_color);
   }

   basic_toggle_button<>
   toggle_button(
      std::string const& text
    , float size
    , color body_color
   )
   {
      return make_button<basic_toggle_button<>>(text, size, body_color);
   }

   basic_toggle_button<>
   toggle_button(
      std::uint32_t icon_code
    , float size
    , color body_color
   )
   {
      return make_button<basic_toggle_button<>>(icon_code, size, body_color);
   }

   basic_toggle_button<>
   toggle_button(
      std::uint32_t icon_code
    , std::string const& text
    , float size
    , color body_color
   )
   {
      return make_button<basic_toggle_button<>>(icon_code, text, size, body_color);
   }

   basic_toggle_button<>
   toggle_button(
      std::string const& text
    , std::uint32_t icon_code
    , float size
    , color body_color
   )
   {
      return make_button<basic_toggle_button<>>(text, icon_code, size, body_color);
   }

   basic_latching_button<>
   latching_button(
      std::string const& text
    , float size
    , color body_color
   )
   {
      return make_button<basic_latching_button<>>(text, size, body_color);
   }

   basic_latching_button<>
   latching_button(
      std::uint32_t icon_code
    , float size
    , color body_color
   )
   {
      return make_button<basic_latching_button<>>(icon_code, size, body_color);
   }

   basic_latching_button<>
   latching_button(
      std::uint32_t icon_code
    , std::string const& text
    , float size
    , color body_color
   )
   {
      return make_button<basic_latching_button<>>(icon_code, text, size, body_color);
   }

   basic_latching_button<>
   latching_button(
      std::string const& text
    , std::uint32_t icon_code
    , float size
    , color body_color
   )
   {
      return make_button<basic_latching_button<>>(text, icon_code, size, body_color);
   }

   basic_dropdown_menu
   dropdown_menu(
      std::string const &text, color body_color
   )
   {
      return make_button<basic_dropdown_menu>(text, icons::down_dir, 1.0, body_color);
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
      context const& ctx
    , uint32_t code
    , float size
    , color body_color_
    , bool state
    , bool hilite
   )
   {
      float corner_radius = 6;
      auto const& theme = get_theme();

      // Draw Button Body
      color body_color = state? body_color_ : body_color_.opacity(0.5).level(0.8);
      draw_button_base(ctx, ctx.bounds, body_color, corner_radius);

      canvas& canvas_ = ctx.canvas;
      rect bounds = ctx.bounds;

      // Draw Icon
      color icon_color = hilite?
         theme.icon_color.level(1.5) :
         theme.icon_color.level(0.9);
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
}}
