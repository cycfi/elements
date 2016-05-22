/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/theme.hpp>
#include <photon/support.hpp>
#include <nanovg.h>
#include <vector>
#include <cmath>

namespace photon
{
   namespace
   {
      void draw_text(
         canvas& _canvas, rect b, char const* text
       , char const* font, float font_size, color color_)
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
         auto state = _canvas.new_state();

         float x = b.left;
         float y = b.top;
         float w = b.width();

         _canvas.clip(b);

         _canvas.font_size(font_size);
         _canvas.font_face(font);
         _canvas.fill_color(color);

         _canvas.text_box(point{ x, y+font_size }, w, text);
      }
   }

   void theme::draw_label(rect b, char const* text)
   {
      draw_text(_canvas, b, text, label_font, label_font_size, label_font_color);
   }

   point theme::measure_label(char const* text)
   {
      return measure_text(_canvas, text, label_font, label_font_size);
   }

   void theme::draw_heading(rect b, char const* text)
   {
      draw_text(_canvas, b, text, heading_font, heading_font_size, heading_font_color);
   }

   point theme::measure_heading(char const* text)
   {
      return measure_text(_canvas, text, heading_font, heading_font_size);
   }

   void theme::draw_icon(rect b, uint32_t code, int size)
   {
      char icon[8];
      char const* text = codepoint_to_UTF8(code, icon);

      float x = b.left;
      float y = b.top;
      float h = b.height();
      float w = b.width();
      float cx = x+(w/2);
      float cy = y+(h/2);
      float sh = h/16;

      _canvas.font_size(size);
      _canvas.font_face(icon_font);
      _canvas.text_align(canvas::align_middle | canvas::align_center);

      // Shadow
      _canvas.font_blur(2);
      _canvas.fill_color(color{ 0, 0, 0, 128 });
      _canvas.text(point{ cx, cy+sh }, text, 0);

      // Text
      _canvas.font_blur(0);
      _canvas.fill_color(icon_color);
      _canvas.text(point{ cx, cy }, text);
   }

   point theme::measure_icon(uint32_t code, int size)
   {
      char icon[8];
      return measure_text(_canvas, codepoint_to_UTF8(code,icon), icon_font, size);
   }

   void theme::draw_text_box(rect b, char const* text)
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
         void for_each_row(F f, float& y, float& lineh)
         {
            auto state = _canvas.new_state();

            y = this->y;
            char const* cp = start;

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
         }

         void draw_caret(float lineh, float y, float row_width, char const* rstart, char const* rend)
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

         void draw_selection(float lineh, float y, char const* rstart, char const* rend)
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

         void draw()
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

         float height()
         {
            auto height_f = [&](auto& row, auto i, auto y, auto lineh) { return true; };

            float y;
            float lineh;
            for_each_row(height_f, y, lineh);
            return y + lineh;
         }

         char const* caret_position(point p)
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

         theme::glyph_info glyph_bounds(char const* cp)
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

   void theme::draw_edit_text_box(rect b, text_draw_info const& text)
   {
      edit_text_box_renderer r{ *this, b, text };
      r.draw();
   }

   char const* theme::caret_position(rect b, text_info const& text, point p)
   {
      edit_text_box_renderer r{ *this, b, text };
      return r.caret_position(p);
   }

   float theme::edit_text_box_height(rect b, text_info const& text)
   {
      edit_text_box_renderer r{ *this, b, text };
      return r.height();
   }

   theme::glyph_info theme::glyph_bounds(rect b, text_info const& text, char const* cp)
   {
      edit_text_box_renderer r{ *this, b, text };
      return r.glyph_bounds(cp);
   }

   void theme::draw_edit_box_base(rect b)
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

   void theme::load_fonts()
   {
      _canvas.new_font("icons", "./assets/fonts/entypo.ttf");
      _canvas.new_font("sans", "./assets/fonts/Roboto-Regular.ttf");
      _canvas.new_font("sans-bold", "./assets/fonts/Roboto-Bold.ttf");
   }
}
