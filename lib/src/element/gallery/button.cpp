/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/gallery/button.hpp>
#include <utility>

namespace cycfi { namespace elements
{
   void draw_button_base(
      context const& ctx, rect bounds, color color_, bool enabled, float corner_radius)
   {
      draw_button(ctx.canvas, bounds, color_, enabled, corner_radius);
   }

   bool button_styler_base::cursor(context const& ctx, point /*p*/, cursor_tracking /*status*/)
   {
      ctx.view.refresh(ctx);
      return true;
   }

   bool button_styler_base::wants_control() const
   {
      return true;
   }

   view_limits default_button_styler::limits(basic_context const& ctx) const
   {
      auto const& theme = get_theme();
      auto margin = get_margin();
      auto rel_size = get_size();
      auto space = theme.button_text_icon_space * rel_size;
      auto font = theme.label_font;
      auto& cnv = ctx.canvas;

      // Measure the text width
      auto size = measure_text(cnv, get_text(), font.size(font._size * rel_size));

      // Add space for the icon if necessary
      if (get_icon_placement() != icon_none)
      {
         auto icon_size = measure_icon(cnv, get_icon(), rel_size * theme.icon_font._size);
         size.x += icon_size.x + space;
         size.y = std::max(size.y, icon_size.y);
      }

      auto x_space = margin.left + margin.right;
      auto y_space = margin.top + margin.bottom;
      return {
         {size.x + x_space, size.y + y_space}
       , {full_extent, size.y + y_space}
      };
   }

   void default_button_styler::draw(context const& ctx)
   {
      auto& cnv = ctx.canvas;
      auto canvas_state = cnv.new_state();
      auto const& theme = get_theme();
      auto bounds = ctx.bounds;
      auto margin = get_margin();
      auto rel_size = get_size();
      auto space = theme.button_text_icon_space * rel_size;

      auto state = value();
      auto value = state.value;
      auto hilite = state.hilite;
      auto enabled = ctx.enabled;
      auto body_color = get_body_color();

      // Draw the body
      if (value)
      {
         body_color = body_color.opacity(0.5);
         bounds = bounds.move(1, 1);
      }
      else
      {
         body_color = body_color.level(0.9);
      }
      draw_button_base(ctx, bounds, body_color, enabled, get_corner_radius() * rel_size);

      // Adjust the font size
      auto font = theme.label_font;
      font = font.size(font._size * rel_size);

      // Measure text and icon
      auto text_size = measure_text(ctx.canvas, get_text(), font);

      // Add space for the icon if necessary
      auto icon_width = 0.0f;
      auto icon_space = 0.0f;
      if (get_icon_placement() != icon_none)
      {
         auto icon_size = measure_icon(cnv, get_icon(), rel_size * theme.icon_font._size);
         icon_width += icon_size.x;
         icon_space = icon_width + space;
      }

      auto text_c = get_text_color();
      text_c = enabled?
         text_c :
         text_c.opacity(text_c.alpha * theme.disabled_opacity)
         ;

      if (hilite && enabled)
         text_c = text_c.level(1.5);

      auto mid_x = bounds.left + (bounds.width() / 2);
      auto mid_y = bounds.top + (bounds.height() / 2);

      // Draw label
      float text_pos = 0.0f;
      float icon_pos = 0.0f;
      int align = cnv.middle;
      auto icon_placement = get_icon_placement();
      auto label_alignment = get_label_alignment();

      switch (label_alignment)
      {
         case align_left:
         case align_center:
            {
               if (label_alignment == align_left)
                  text_pos = bounds.left + margin.left;
               else // align_center
                  text_pos = mid_x - ((text_size.x + icon_space) / 2);

               if (icon_placement != icon_none)
               {
                  if (icon_placement == icon_left)
                  {
                     icon_pos = text_pos;
                     text_pos += icon_space;
                  }
                  else
                  {
                     icon_pos = text_pos + text_size.x + space;
                  }
               }
               align += cnv.left;
            }
            break;

         case align_right:
            {
               text_pos = bounds.right - margin.right;
               if (icon_placement != icon_none)
               {
                  if (icon_placement == icon_right)
                  {
                     text_pos -= icon_space;
                     icon_pos = text_pos + space;
                  }
                  else
                  {
                     icon_pos = (text_pos - text_size.x) - icon_space;
                  }
               }
               align += cnv.right;
            }
            break;
      }

      cnv.fill_style(text_c);
      cnv.font(font);
      cnv.text_align(align);
      cnv.fill_text(get_text(), {text_pos, mid_y});

      // Draw icon
      if (icon_placement != icon_none)
      {
         auto icon_c = get_icon_color();
         icon_c = enabled?
            icon_c :
            icon_c.opacity(icon_c.alpha * theme.disabled_opacity)
            ;

         auto icon_font = theme.icon_font;
         cnv.fill_style(icon_c);
         cnv.text_align(cnv.middle + cnv.left);
         cnv.font(icon_font.size(rel_size * icon_font._size));
         cnv.fill_text(codepoint_to_utf8(get_icon()).c_str(), {icon_pos, mid_y});
      }
   }
}}

