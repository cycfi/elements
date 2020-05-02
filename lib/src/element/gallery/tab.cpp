/*=============================================================================
   Copyright (c) 2020 Michal Urbanski

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/gallery/tab.hpp>

namespace cycfi { namespace elements
{
   void draw_tab(
     context const& ctx, std::string const& text, bool state, bool hilite
   )
   {
      auto&       canvas_ = ctx.canvas;
      auto        canvas_state = canvas_.new_state();
      auto const& theme_ = get_theme();
      color const indicator_color = theme_.indicator_color;
      float const bg_alfa = theme_.box_widget_bg_opacity;
      rect        box = ctx.bounds;
      auto        corner_radius = get_theme().frame_corner_radius;

      color c1 = state ? indicator_color.level(1.5) : rgb(0, 0, 0).opacity(bg_alfa);

      canvas_.fill_style(theme_.label_font_color);
      canvas_.font(
         theme_.label_font,
         theme_.label_font_size
      );
      canvas_.text_align(canvas_.center | canvas_.middle);
      float cx = box.left;
      float cy = box.top + (box.height() / 2);
      canvas_.fill_text(point{ cx, cy }, text.c_str());

      auto gradient = canvas::linear_gradient{
         box.top_left(),
         box.bottom_left()
      };

      float const bg_alpha = get_theme().box_widget_bg_opacity;
      gradient.add_color_stop({ 0.0, rgb(255, 255, 255).opacity(bg_alpha) });
      gradient.add_color_stop({ 1.0, rgb(0, 0, 0).opacity(bg_alpha) });
      canvas_.fill_style(gradient);

      canvas_.begin_path();
      canvas_.round_rect(box.inset(1, 1), corner_radius-1);
      canvas_.fill_style(c1);
      canvas_.fill();
      canvas_.round_rect(box.inset(1, 1), corner_radius-1);
      canvas_.fill_style(gradient);
      canvas_.fill();

      canvas_.begin_path();
      canvas_.round_rect(box.inset(0.5, 0.5), corner_radius-0.5);
      canvas_.stroke_style(rgba(0, 0, 0, 48));
      canvas_.stroke();
   }

   bool basic_tab::is_selected() const
   {
      return value();
   }

   void basic_tab::select(bool state)
   {
      if (state != is_selected())
         value(state);
   }

   element* basic_tab::click(context const& ctx, mouse_button btn)
   {
      bool was_selected = is_selected();
      auto r = basic_latching_button<>::click(ctx, btn);
      if (!was_selected && value())
      {
         auto [c, cctx] = find_composite(ctx);
         if (c)
         {
            for (std::size_t i = 0; i != c->size(); ++i)
            {
               if (auto e = find_subject<basic_tab*>(&c->at(i)))
               {
                  if (e == this)
                  {
                     // Set the radio button
                     e->select(true);
                     // The base class::click should have called on_click already
                  }
                  else
                  {
                     if (e->is_selected())
                     {
                        // Reset the tab
                        e->select(false);
                        if (e->on_click)
                           e->on_click(false);
                     }
                  }
               }
            }
         }
         cctx->view.refresh(*cctx);
      }
      return r;
   }
}}
