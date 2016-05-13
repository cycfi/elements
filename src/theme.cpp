/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/theme.hpp>
#include <photon/support.hpp>
#include <nanovg.h>
#include <vector>

namespace photon
{
   void theme::draw_panel(rect b) const
   {
      // Round Rectangle
      _canvas.begin_path();
      _canvas.round_rect(b, panel_corner_radius);
      _canvas.fill_color(panel_color);
      _canvas.fill();

      // Drop shadow
      rect offs = panel_shadow_offset;
      _canvas.begin_path();
      _canvas.rect({ b.left+offs.left, b.top+offs.top, b.right+offs.right, b.bottom+offs.bottom });
      _canvas.round_rect(b, panel_corner_radius);
      _canvas.path_winding(canvas::hole);

      rect  sh_fillr = b;
      sh_fillr.top += 2;
      paint shadow_paint
         = _canvas.box_gradient(sh_fillr, panel_corner_radius*2, 10
          , color(0, 0, 0, 128), color(0, 0, 0, 0)
         );

      _canvas.fill_paint(shadow_paint);
      _canvas.fill();
   }

   void theme::draw_frame(rect b) const
   {
      _canvas.begin_path();
      _canvas.round_rect(b, frame_corner_radius);
      _canvas.stroke_color(frame_color);
      _canvas.stroke_width(frame_stroke_width);
      _canvas.stroke();
   }

   void theme::draw_title_bar(rect b) const
   {
      auto bg = _canvas.linear_gradient(
          point{ b.left, b.top },
          point{ b.left, b.bottom },
          color{ 255, 255, 255, 16 },
          color{ 0, 0, 0, 16 }
      );

      _canvas.begin_path();
      _canvas.round_rect(b.inset(1, 1), panel_corner_radius);
      _canvas.fill_paint(bg);
      _canvas.fill();

      _canvas.begin_path();
      _canvas.move_to(point{ b.left+0.5f, b.bottom-0.5f });
      _canvas.line_to(point{ b.right-0.5f, b.bottom-0.5f });
      _canvas.stroke_color(color{ 0, 0, 0, 32 });
      _canvas.stroke();
   }

   namespace
   {
      void draw_slot(canvas& _canvas, rect b, float r)
      {
         paint bg
            = _canvas.box_gradient(b.inset(0.5, 0.5).move(1, 1), r, 2
             , color(0, 0, 0, 32), color(0, 0, 0, 128)
            );

         _canvas.begin_path();
         _canvas.round_rect(b, r);
         _canvas.fill_paint(bg);
         _canvas.fill();
      }

      void draw_knob(canvas& _canvas, circle cp, color fill_color, color outline_color)
      {
         // Knob Shadow
         paint bg
            = _canvas.radial_gradient(
                  point{ cp.cx, cp.cy+1 }, cp.radius-3, cp.radius+3,
                  color{ 0, 0, 0, 64 }, color{ 0, 0, 0, 0 }
               );

         _canvas.begin_path();
         _canvas.rect(cp.bounds().inset(-5, -5).move(0, 3));
         _canvas.circle(cp);
         _canvas.path_winding(canvas::hole);
         _canvas.fill_paint(bg);
         _canvas.fill();

         // Knob
         paint knob
            = _canvas.linear_gradient(
                  point{ cp.cx, cp.cy-cp.radius }, point{ cp.cx, cp.cy+cp.radius },
                  color{ 255, 255, 255, 16 }, color{ 0, 0, 0, 16 }
               );

         _canvas.begin_path();
         _canvas.circle(circle{ cp.cx, cp.cy, cp.radius-1 });
         _canvas.fill_color(fill_color);
         _canvas.fill();
         _canvas.fill_paint(knob);
         _canvas.fill();

         _canvas.begin_path();
         _canvas.circle(circle{ cp.cx, cp.cy, cp.radius-0.5f });
         _canvas.stroke_color(outline_color);
         _canvas.stroke();
      }
   }

   void theme::draw_slider(float pos, rect b) const
   {
      float w = b.width();
      float h = b.height();

      _canvas.save();

      if (w > h)
      {
         // horizontal
         float sl = h * slider_slot_size;
         draw_slot(_canvas, b.inset(0, (h-sl)/2), sl/2);

      }
      else
      {
         // vertical
         float sl = w * slider_slot_size;
         draw_slot(_canvas, b.inset((w-sl)/2, 0), sl/2);
      }

      draw_slider_knob(pos, b);
      _canvas.restore();
   }

   void theme::draw_slider_knob(float pos, rect b) const
   {
      draw_knob(
         _canvas, slider_knob_position(pos, b),
         slider_knob_fill_color, slider_knob_outline_color
      );
   }

   circle theme::slider_knob_position(float pos, rect b) const
   {
      float x = b.left;
      float y = b.top;
      float w = b.width();
      float h = b.height();

      if (w > h)
      {
         // horizontal
         float    cy = y + h * 0.5;
         float    kr = h * slider_knob_radius;

         // inset by radius;
         w -= kr * 2;
         x += kr;

         return { x+(pos*w), cy, kr };
      }
      else
      {
         // vertical
         float    cx = x + w * 0.5;
         float    kr = w * slider_knob_radius;

         // inset by radius;
         h -= kr * 2;
         y += kr;

         return { cx, y+(pos*h), kr };
      }
   }

   namespace
   {
      void draw_scrollbar_fill(canvas& _canvas, rect r)
      {
         _canvas.begin_path();
         _canvas.rect(r);
         _canvas.fill_color(color{ 200, 200, 200, 30 });
         _canvas.fill();
      }

      void draw_scrollbar(
         canvas& _canvas, rect b, float radius,
         color outline_color, color fill_color
      )
      {
         _canvas.begin_path();
         _canvas.round_rect(b, radius);

         _canvas.fill_color(fill_color);
         _canvas.fill();

         _canvas.stroke_color(outline_color);
         _canvas.stroke();
      }
   }

   void theme::draw_scroll_bar(float pos, float ext, rect b) const
   {
      float x = b.left;
      float y = b.top;
      float w = b.width();
      float h = b.height();

      draw_scrollbar_fill(_canvas, b);

      if (w > h)
      {
         w *= w/ext;
         min_limit(w, 20);
         x += pos * (b.width()-w);
      }
      else
      {
         h *= h/ext;
         min_limit(h, 20);
         y += pos * (b.height()-h);
      }

      draw_scrollbar(_canvas, rect{ x, y, x+w, y+h }, scroll_bar_width/3,
         scroll_bar_outline_color, scroll_bar_fill_color);
   }

   rect theme::scroll_bar_position(float pos, float ext, rect b) const
   {
      float x = b.left;
      float y = b.top;
      float w = b.width();
      float h = b.height();

      if (w > h)
      {
         w *= w/ext;
         min_limit(w, 20);
         x += pos * (b.width()-w);
      }
      else
      {
         h *= h/ext;
         min_limit(h, 20);
         y += pos * (b.height()-h);
      }
      return rect{ x, y, x+w, y+h };
   }

   namespace
   {
      void draw_text(
         canvas& _canvas, rect b, char const* text
       , char const* font, float font_size, color color_
      )
      {
         float x = b.left;
         float y = b.top;
         float h = b.height();
         float cy = y+(h/2);
         float sh = h/16;

         _canvas.font_size(font_size);
         _canvas.font_face(font);
         _canvas.text_align(canvas::align_middle);

         // Shadow
         _canvas.font_blur(2);
         _canvas.fill_color(color{ 0, 0, 0, 128 });
         _canvas.text(point{ x, cy+sh }, text, 0);

         // Text
         _canvas.font_blur(0);
         _canvas.fill_color(color_);
         _canvas.text(point{ x, cy }, text);
      }

      point measure_text(
         canvas& _canvas, char const* text
       , char const* font, float font_size
      )
      {
         _canvas.font_size(font_size);
         _canvas.font_face(font);

         float w = _canvas.text_width(text);
         float h = font_size;
         return { w, h };
      }

      void draw_text_box(
         canvas& _canvas, rect b, char const* text
       , char const* font, float font_size, color color
      )
      {
         float x = b.left;
         float y = b.top;
         float w = b.width();

         _canvas.save();
         _canvas.clip(b);

         _canvas.font_size(font_size);
         _canvas.font_face(font);
         _canvas.fill_color(color);

         _canvas.text_box(point{ x, y+font_size }, w, text);
         _canvas.restore();
      }
   }

   void theme::draw_label(rect b, char const* text) const
   {
      draw_text(_canvas, b, text, label_font, label_font_size, label_font_color);
   }

   point theme::measure_label(char const* text) const
   {
      return measure_text(_canvas, text, label_font, label_font_size);
   }

   void theme::draw_heading(rect b, char const* text) const
   {
      draw_text(_canvas, b, text, heading_font, heading_font_size, heading_font_color);
   }

   point theme::measure_heading(char const* text) const
   {
      return measure_text(_canvas, text, heading_font, heading_font_size);
   }

   void theme::draw_icon(rect b, uint32_t code, int size) const
   {
      char icon[8];
      draw_text(_canvas, b, codepoint_to_UTF8(code, icon), icon_font, size, icon_color);
   }

   point theme::measure_icon(uint32_t code, int size) const
   {
      char icon[8];
      return measure_text(_canvas, codepoint_to_UTF8(code,icon), icon_font, size);
   }

   void theme::draw_text_box(rect b, char const* text) const
   {
      photon::draw_text_box(_canvas, b, text, text_box_font, text_box_font_size, text_box_font_color);
   }

   namespace
   {
      struct edit_text_box_renderer
      {
         float const right_pad = 5;

         edit_text_box_renderer(theme const& th, rect b, theme::text_draw_info const& text)
          : th(th)
          , _canvas(th.canvas())
          , x(b.left)
          , y(b.top)
          , w(b.width())
          , h(b.height())
          , start(text.first)
          , end(text.last)
          , sstart(text.select_first)
          , send(text.select_last)
          , is_focus(text.is_focus)
          , is_active(text.is_active)
         {
            if (sstart > send)
               std::swap(sstart, send);
         }

         edit_text_box_renderer(theme const& th, rect b, theme::text_info const& text)
          : th(th)
          , _canvas(th.canvas())
          , x(b.left)
          , y(b.top)
          , w(b.width())
          , h(b.height())
          , start(text.first)
          , end(text.last)
         {
            if (sstart > send)
               std::swap(sstart, send);
         }

         template <typename F>
         void for_each_row(F f, float& y, float& lineh) const
         {
            y = this->y;
            char const* cp = start;

            _canvas.save();
            _canvas.font_size(th.text_box_font_size);
            _canvas.font_face(th.text_box_font);
            _canvas.text_align(canvas::align_left | canvas::align_top);
            lineh = _canvas.line_height();

            auto line_f = [&y, &lineh, f](auto& row, auto i)
            {
               if (!f(row, i, y, lineh))
                  return false;
               y += lineh;
               return true;
            };

            _canvas.for_each_line(line_f, cp, end, w-right_pad);
            _canvas.restore();
         }

         void draw_caret(float lineh, float y, float row_width, char const* rstart, char const* rend) const
         {
            if (sstart >= rstart && sstart <= rend)
            {
               std::vector<glyph_position>
                  glyphs = _canvas.glyphs(point{ x, y }, rstart, rend);

               float caretx = x+row_width;
               for (int i = 0; i < glyphs.size(); ++i)
               {
                  auto const& glyph = glyphs[i];
                  if (sstart == glyph.str)
                     caretx = glyphs[i].x;
               }
               _canvas.begin_path();
               _canvas.fill_color(th.text_box_caret_color);
               _canvas.rect(rect{ caretx, y, caretx+1, y+lineh });
               _canvas.fill();
            }
         }

         void draw_selection(float lineh, float y, char const* rstart, char const* rend) const
         {
            bool  start_hilite = sstart >= rstart && sstart <= rend;
            bool  end_hilite = send >= rstart && send <= rend;
            bool  mid_hilite = sstart <= rstart && send >= rend;

            if (start_hilite || end_hilite || mid_hilite)
            {
               float x_hilite = x;
               float w_hilite = w;
               if (start_hilite || end_hilite)
               {
                  std::vector<glyph_position>
                     glyphs = _canvas.glyphs(point{ x, y }, rstart, rend);

                  for (int i = 0; i < glyphs.size(); ++i)
                  {
                     auto const& glyph = glyphs[i];
                     if (sstart == glyph.str)
                     {
                        x_hilite = glyph.x;
                        w_hilite -= glyph.x-x;
                     }
                     else if (send == glyph.str)
                     {
                        w_hilite = glyphs[i].x - x_hilite;
                     }
                  }
               }

               _canvas.begin_path();
               _canvas.rect(rect{ x_hilite, y, x_hilite+w_hilite, y+lineh });
               auto c = is_focus? th.text_box_hilite_color : color{ 255, 255, 255, 16 };
               _canvas.fill_color(c);
               _canvas.fill();
            }
         }

         void draw() const
         {
            auto draw_f = [&](auto& row, auto i, auto y, auto lineh)
            {
               if (sstart != 0)
               {
                  if (sstart == send)
                  {
                     if (is_focus)
                        draw_caret(lineh, y, row.width, row.start, row.end);
                  }
                  else
                  {
                     draw_selection(lineh, y, row.start, row.end);
                  }
               }

               _canvas.fill_color(is_active ? th.text_box_font_color : th.inactive_font_color);
               _canvas.text(point{ x, y }, row.start, row.end);
               return true;
            };

            float lineh;
            float y;
            for_each_row(draw_f, y, lineh);
         }

         float height() const
         {
            auto height_f = [&](auto& row, auto i, auto y, auto lineh) { return true; };

            float y;
            float lineh;
            for_each_row(height_f, y, lineh);
            return y + lineh;
         }

         char const* caret_position(point p) const
         {
            float       mx = p.x;
            float       my = p.y;
            char const* result = 0;

            auto caret_f = [&](auto& row, auto i, auto y, auto lineh)
            {
               // note: we compare the point against x-1...x+w+2. i.e. the row bounds
               // expanded by 1 pixel in the x axis. This allows for roundoff errors.
               if (mx >= x-1 && mx < (x+w+2) && my >= y && my < (y+lineh))
               {
                  std::vector<glyph_position>
                     glyphs = _canvas.glyphs(point{ x, y }, row.start, row.end);

                  if (glyphs.size() == 0)
                  {
                     result = row.start;
                     return false;
                  }

                  float px = x;
                  for (int i = 0; i < glyphs.size(); ++i)
                  {
                     bool  last = i+1 >= glyphs.size();
                     float x0 = glyphs[i].x;
                     float x1 = !last? glyphs[i+1].x : x+row.width;
                     float gx = x0 * 0.3f + x1 * 0.7f;
                     if (mx >= px)
                     {
                        if (mx < gx)
                        {
                           result = glyphs[i].str;
                           return false;
                        }
                        else if (last)
                        {
                           result = row.end;
                           return false;
                        }
                     }
                     px = gx;
                  }
               }
               return true;
            };

            float y;
            float lineh;
            for_each_row(caret_f, y, lineh);
            return result;
         }

         theme::glyph_info glyph_bounds(char const* cp) const
         {
            theme::glyph_info result;
            auto glyph_f = [&](auto& row, auto i, auto y, auto lineh)
            {
               std::vector<glyph_position>
                  glyphs = _canvas.glyphs(point{ x, y }, row.start, row.end);

               if (glyphs.size() == 0 && row.start == cp)
               {
                  result = { row.start, x, y, lineh, x, x };
                  return false;
               }
               else if (row.end == cp)
               {
                  float rightmost = x+row.width;
                  result = { row.end, rightmost, y, lineh, rightmost, rightmost };
                  return false;
               }

               for (int i = 0; i < glyphs.size(); ++i)
               {
                  auto const& glyph = glyphs[i];
                  if (cp == glyph.str)
                  {
                     auto& glyph = glyphs[i];
                     result = { glyph.str, glyph.x, y, lineh, glyph.minx, glyph.maxx };
                     return false;
                  }
               }
               return true;
            };

            float y;
            float lineh;
            for_each_row(glyph_f, y, lineh);
            return result;
         }

         theme const&   th;
         canvas&        _canvas;
         float          x;
         float          y;
         float          w;
         float          h;

         char const*    start;
         char const*    end;
         char const*    sstart;
         char const*    send;
         bool           is_focus;
         bool           is_active;
      };
   }

   void theme::draw_edit_text_box(rect b, text_draw_info const& text) const
   {
      edit_text_box_renderer r{ *this, b, text };
      r.draw();
   }

   char const* theme::caret_position(rect b, text_info const& text, point p) const
   {
      edit_text_box_renderer r{ *this, b, text };
      return r.caret_position(p);
   }

   float theme::edit_text_box_height(rect b, text_info const& text) const
   {
      edit_text_box_renderer r{ *this, b, text };
      return r.height();
   }

   theme::glyph_info theme::glyph_bounds(rect b, text_info const& text, char const* cp) const
   {
      edit_text_box_renderer r{ *this, b, text };
      return r.glyph_bounds(cp);
   }

   void theme::draw_edit_box_base(rect b) const
   {
      paint bg
         = _canvas.box_gradient(b.inset(1, 1).move(0, 1.5), 3, 4
          , color(255, 255, 255, 32), edit_box_fill_color
         );

      _canvas.begin_path();
      _canvas.round_rect(b.inset(1, 1), 4-1);
      _canvas.fill_paint(bg);
      _canvas.fill();

      _canvas.begin_path();
      _canvas.round_rect(b.inset(0.5, 0.5), 4-0.5f);
      _canvas.stroke_color(color{ 0, 0, 0, 48 });
      _canvas.stroke();
   }

   void theme::draw_button(rect b, color button_color) const
   {
      bool  black = button_color == colors::black;
      paint bg
         = _canvas.linear_gradient(b.top_left(), point{ b.left, b.bottom }
          , color(255, 255, 255, black ? 16 : 32)
          , color(0, 0, 0, black ? 16 : 32)
         );

      _canvas.begin_path();
      _canvas.round_rect(b.inset(1, 1), button_corner_radius-1);

      if (!black)
      {
         _canvas.fill_color(button_color);
         _canvas.fill();
      }
      _canvas.fill_paint(bg);
      _canvas.fill();

      _canvas.begin_path();
      _canvas.round_rect(b.inset(0.5, 0.5), button_corner_radius-0.5);
      _canvas.stroke_color(color{ 0, 0, 0, 48 });
      _canvas.stroke();
   }

   void theme::load_fonts() const
   {
      _canvas.new_font("icons", "./assets/fonts/entypo.ttf");
      _canvas.new_font("sans", "./assets/fonts/Roboto-Regular.ttf");
      _canvas.new_font("sans-bold", "./assets/fonts/Roboto-Bold.ttf");
   }
}
