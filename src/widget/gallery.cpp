/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/gallery.hpp>

namespace photon
{
   float basic_button_body::corner_radius = 4.0;

   void draw_button_base(context const& ctx, rect bounds, color color_, float corner_radius)
   {
      canvas&  canvas_ = ctx.canvas();

      paint bg
         = canvas_.linear_gradient(bounds.top_left(), bounds.bottom_left()
          , color(255, 255, 255, 32)
          , color(0, 0, 0, 32)
         );

      canvas_.begin_path();
      canvas_.round_rect(bounds.inset(1, 1), corner_radius-1);
      canvas_.fill_color(color_);
      canvas_.fill();
      canvas_.fill_paint(bg);
      canvas_.fill();

      canvas_.begin_path();
      canvas_.round_rect(bounds.inset(0.5, 0.5), corner_radius-0.5);
      canvas_.stroke_color(color{ 0, 0, 0, 48 });
      canvas_.stroke();
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
      std::string const& text
    , color body_color
   )
   {
      return make_button<basic_dropdown_menu>(text, icons::caretdown, body_color);
   }

   void draw_check_box(
      context const& ctx, std::string const& text, bool state, bool hilite
   )
   {
      canvas&  canvas_ = ctx.canvas();
      theme&   theme_ = ctx.theme();
      color    indicator_color = theme_.indicator_color;
      rect     box = ctx.bounds.move(15, 0);

      box.width(box.height());

      color c1 = state ? indicator_color.level(1.5) : color(0, 0, 0, 32);
      color c2 = state ? indicator_color : color(0, 0, 0, 92);

      if (state && hilite)
      {
         c1 = c1.level(1.3);
         c2 = c2.level(1.3);
      }
      paint bg = canvas_.box_gradient(box.inset(1, 1).move(0, 1.5), 3, 4, c1, c2);

      canvas_.begin_path();
      canvas_.round_rect(box.inset(1, 1), 3);
      canvas_.fill_paint(bg);
      canvas_.fill();

      if (state || hilite)
      {
         float const glow = 2;
         color glow_color = hilite ? indicator_color : c2;

         paint glow_paint
            = canvas_.box_gradient(box, 3, 3, glow_color, color(0, 0, 0, 0)
            );

         canvas_.begin_path();
         canvas_.rect(box.inset(-glow, -glow));
         canvas_.round_rect(box.inset(1.5, 1.5), 3);
         canvas_.path_winding(canvas::hole);
         canvas_.fill_paint(glow_paint);
         canvas_.fill();
      }

      if (state)
      {
         auto save = set(theme_.icon_color, c1.level(2.0));
         text_utils(theme_).draw_icon(box, icons::check, 14);
      }
      else
      {
         color outline_color = hilite ? theme_.frame_color : color{ 0, 0, 0, 48 };
         canvas_.begin_path();
         canvas_.round_rect(box.inset(1, 1), 3);
         canvas_.stroke_color(outline_color);
         canvas_.stroke();
      }

      rect  text_bounds = ctx.bounds.move(45, 0);
      text_utils(theme_)
         .draw_text(
            text_bounds, text.c_str(), theme_.label_font,
            theme_.label_font_size, theme_.label_font_color
         );
   }

   void draw_icon_button(
      context const& ctx, uint32_t code, float size, bool state, bool hilite
   )
   {
      float corner_radius = 6;
      color indicator_color = ctx.theme().indicator_color.level(0.7);
      color body_color = indicator_color;

      if (state)
         body_color = body_color.level(2.0);
      else if (hilite)
         body_color = body_color.level(1.5);

      // Draw Button Body
      draw_button_base(ctx, ctx.bounds, body_color, corner_radius);

      canvas&  canvas_ = ctx.canvas();
      theme&   theme_ = ctx.theme();
      rect     bounds = ctx.bounds;

      // Draw Glow
      if (state)
      {
         float const glow = 3;
         color glow_color = indicator_color.level(2.0);

         paint glow_paint
               = canvas_.box_gradient(bounds, 5, 4, glow_color, color(0, 0, 0, 0)
            );

         canvas_.begin_path();
         canvas_.round_rect(bounds.inset(-glow, -glow), corner_radius+3);
         canvas_.round_rect(bounds.inset(1.5, 1.5), corner_radius-2);
         canvas_.path_winding(canvas::hole);
         canvas_.fill_paint(glow_paint);
         canvas_.fill();
      }

      // Draw Icon
      color icon_color = state ? indicator_color.level(4.0) : indicator_color.level(0.2);
      auto  save = set(theme_.icon_color, icon_color);
      text_utils(theme_).draw_icon(bounds.move(0.5, 0.5), code, size, false);

   }

   rect menu_item_spacer_widget::limits(basic_context const& ctx) const
   {
      return { 0, 0, full_extent, ctx.theme().heading_font_size };
   }

   void menu_item_spacer_widget::draw(context const& ctx)
   {
      auto& canvas_ = ctx.canvas();
      float y = ctx.bounds.top + ctx.bounds.height() / 2;

      canvas_.begin_path();
      canvas_.move_to({ ctx.bounds.left, y });
      canvas_.line_to({ ctx.bounds.right, y });
      canvas_.stroke_color(ctx.theme().frame_color.opacity(0.15));
      canvas_.stroke_width(1);
      canvas_.stroke();
   }
}
