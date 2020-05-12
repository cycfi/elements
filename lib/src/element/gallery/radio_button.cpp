/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/gallery/radio_button.hpp>

namespace cycfi { namespace elements
{
   void draw_radio_button(
     context const& ctx, std::string const& text, bool state, bool hilite
   )
   {
      auto&       canvas_ = ctx.canvas;
      auto        canvas_state = canvas_.new_state();
      auto const& theme_ = get_theme();
      color const indicator_color = theme_.indicator_color;
      float const bg_alfa = theme_.box_widget_bg_opacity;
      rect        box = ctx.bounds.move(15, 0);
      float       size = box.height();
      float       radius = size/2;
      float       dot_radius = radius/2.5;

      box.width(size);
      point       center = center_point(box);

      color c1 = state ? indicator_color.level(1.5) : rgb(0, 0, 0).opacity(bg_alfa);

      if (state)
      {
         canvas_.begin_path();
         canvas_.fill_style(c1.level(hilite ? 2.0 : 1.0));
         canvas_.circle(circle(center, dot_radius));
         canvas_.fill();
      }

      color outline_color = theme_.frame_color;
      canvas_.begin_path();
      canvas_.circle(circle(center, radius-1));
      canvas_.stroke_style(outline_color);
      canvas_.stroke();

      // Pseudo glow
      canvas_.circle(circle(center, radius));
      canvas_.stroke_style(outline_color.opacity(0.1));
      canvas_.stroke();

      canvas_.fill_style(theme_.label_font_color);
      canvas_.font(
         theme_.label_font,
         theme_.label_font_size
      );
      canvas_.text_align(canvas_.left | canvas_.middle);
      float cx = box.right + 10;
      float cy = ctx.bounds.top + (ctx.bounds.height() / 2);
      canvas_.fill_text(point{ cx, cy }, text.c_str());
   }

   bool basic_radio_button::is_selected() const
   {
      return value();
   }

   void basic_radio_button::select(bool state)
   {
      if (state != is_selected())
         value(state);
   }

   element* basic_radio_button::click(context const& ctx, mouse_button btn)
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
               if (auto e = find_element<basic_radio_button*>(&c->at(i)))
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
                        // Reset the radio button
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
