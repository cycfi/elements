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

   namespace
   {
      void draw_slot(NVGcontext* vg, float x, float y, float w, float h, float r)
      {
         // Slot
         NVGpaint bg
            = nvgBoxGradient(
                  vg, x, y+1, w, h, 2, 2,
                  ::nvgRGBA(0, 0, 0, 32), ::nvgRGBA(0, 0, 0, 128)
               );

         nvgBeginPath(vg);
         nvgRoundedRect(vg, x, y, w, h, r);
         nvgFillPaint(vg, bg);
         nvgFill(vg);
      }

      void draw_knob(
         NVGcontext* vg, float cx, float cy, float r,
         color fill_color,
         color outline_color
      )
      {
         // Knob Shadow
         NVGpaint bg
            = nvgRadialGradient(
                  vg, cx, cy+1, r-3, r+3,
                  ::nvgRGBA(0, 0, 0, 64), ::nvgRGBA(0, 0, 0, 0)
               );

         nvgBeginPath(vg);
         nvgRect(vg, cx-r-5, cy-r-5, r*2+5+5, r*2+5+5+3);
         nvgCircle(vg, cx, cy, r);
         nvgPathWinding(vg, NVG_HOLE);
         nvgFillPaint(vg, bg);
         nvgFill(vg);

         // Knob
         NVGpaint knob
            = nvgLinearGradient(
                  vg, cx, cy-r, cx, cy+r,
                  ::nvgRGBA(255, 255, 255, 16), ::nvgRGBA(0, 0, 0, 16)
               );

         nvgBeginPath(vg);
         nvgCircle(vg, cx, cy, r-1);
         nvgFillColor(vg, nvgRGBA(fill_color));
         nvgFill(vg);
         nvgFillPaint(vg, knob);
         nvgFill(vg);

         nvgBeginPath(vg);
         nvgCircle(vg, cx, cy, r-0.5);
         nvgStrokeColor(vg, nvgRGBA(outline_color));
         nvgStroke(vg);
      }
   }

   void theme::draw_slider(float pos, rect b) const
   {
      NVGcontext* _vg = _canvas.context();
      float       x = b.left;
      float       y = b.top;
      float       w = b.width();
      float       h = b.height();

      nvgSave(_vg);

      if (w > h)
      {
         // horizontal
         float    cy = y + h * 0.5;
         float    sl = h * slider_slot_size;
         draw_slot(_vg, x, cy-(sl/2), w, sl, sl/3);
      }
      else
      {
         // vertical
         float    cx = x + w * 0.5;
         float    sl = w * slider_slot_size;
         draw_slot(_vg, cx-(sl/2), y, sl, h, sl/3);
      }

      draw_slider_knob(pos, b);
      nvgRestore(_vg);
   }

   void theme::draw_slider_knob(float pos, rect b) const
   {
      NVGcontext* _vg = _canvas.context();
      circle      cp = slider_knob_position(pos, b);
      draw_knob(
         _vg, cp.cx, cp.cy, cp.radius,
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
      void draw_scrollbar_fill(NVGcontext* vg, float x, float y, float w, float h)
      {
         nvgBeginPath(vg);
         nvgRect(vg, x, y, w, h);
         nvgFillColor(vg, ::nvgRGBA(200, 200, 200, 30));
         nvgFill(vg);
      }

      void draw_scrollbar(
         NVGcontext* vg, float x, float y, float w, float h, float r,
         color outline_color_, color fill_color_
      )
      {
         auto outline_color = nvgRGBA(outline_color_);
         auto fill_color = nvgRGBA(fill_color_);

         nvgBeginPath(vg);
         nvgRoundedRect(vg, x, y, w, h, r);

         nvgFillColor(vg, fill_color);
         nvgFill(vg);

         nvgStrokeColor(vg, outline_color);
         nvgStroke(vg);
      }
   }

   void theme::draw_scroll_bar(float pos, float ext, rect b) const
   {
      NVGcontext* _vg = _canvas.context();
      float       x = b.left;
      float       y = b.top;
      float       w = b.width();
      float       h = b.height();

      draw_scrollbar_fill(_vg, x, y, w, h);

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

      draw_scrollbar(_vg, x, y, w, h, scroll_bar_width/3,
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
         NVGcontext* vg, rect b, char const* text
       , char const* font, float font_size, color color
      )
      {
         float x = b.left;
         float y = b.top;
         float h = b.height();
         float cy = y+(h/2);
         float sh = h/16;

         nvgFontSize(vg, font_size);
         nvgFontFace(vg, font);
         nvgTextAlign(vg, NVG_ALIGN_MIDDLE);

         // Shadow
         nvgFontBlur(vg, 2);
         nvgFillColor(vg, ::nvgRGBA(0, 0, 0, 128));
         nvgText(vg, x, cy+sh, text, 0);

         // Text
         nvgFontBlur(vg, 0);
         nvgFillColor(vg, nvgRGBA(color));
         nvgText(vg, x, cy, text, 0);
      }

      point measure_text(
         NVGcontext* vg, char const* text
       , char const* font, float font_size
      )
      {
         nvgFontSize(vg, font_size);
         nvgFontFace(vg, font);

         float w = nvgTextBounds(vg, 0, 0, text, 0, 0);
         float h = font_size;
         return { w, h };
      }

      void draw_text_box(
         NVGcontext* vg, rect b, char const* text
       , char const* font, float font_size, color color
      )
      {
         float x = b.left;
         float y = b.top;
         float w = b.width();
         float h = b.height();

         nvgSave(vg);
         nvgScissor(vg, x, y, w, h);

         nvgFontSize(vg, font_size);
         nvgFontFace(vg, font);
         nvgFillColor(vg, nvgRGBA(color));

         nvgTextBox(vg, x, y+font_size, w, text, 0);
         nvgRestore(vg);
      }
   }

   void theme::draw_label(rect b, char const* text) const
   {
      draw_text(_canvas.context(), b, text, label_font, label_font_size, label_font_color);
   }

   point theme::measure_label(char const* text) const
   {
      return measure_text(_canvas.context(), text, label_font, label_font_size);
   }

   void theme::draw_heading(rect b, char const* text) const
   {
      draw_text(_canvas.context(), b, text, heading_font, heading_font_size, heading_font_color);
   }

   point theme::measure_heading(char const* text) const
   {
      return measure_text(_canvas.context(), text, heading_font, heading_font_size);
   }

   void theme::draw_icon(rect b, uint32_t code, int size) const
   {
      char icon[8];
      draw_text(_canvas.context(), b, codepoint_to_UTF8(code, icon), icon_font, size, icon_color);
   }

   point theme::measure_icon(uint32_t code, int size) const
   {
      char icon[8];
      return measure_text(_canvas.context(), codepoint_to_UTF8(code,icon), icon_font, size);
   }

   void theme::draw_text_box(rect b, char const* text) const
   {
      photon::draw_text_box(_canvas.context(), b, text, text_box_font, text_box_font_size, text_box_font_color);
   }

   namespace
   {
      struct edit_text_box_renderer
      {
         float const right_pad = 5;

         edit_text_box_renderer(theme const& th, rect b, theme::text_draw_info const& text)
          : vg(th.canvas().context())
          , x(b.left)
          , y(b.top)
          , w(b.width())
          , h(b.height())
          , font(th.text_box_font)
          , font_size(th.text_box_font_size)
          , font_color(th.text_box_font_color)
          , inactive_font_color(th.inactive_font_color)
          , start(text.first)
          , end(text.last)
          , sstart(text.select_first)
          , send(text.select_last)
          , hilite_color(th.text_box_hilite_color)
          , caret_color(th.text_box_caret_color)
          , is_focus(text.is_focus)
          , is_active(text.is_active)
         {
            if (sstart > send)
               std::swap(sstart, send);
         }

         edit_text_box_renderer(theme const& th, rect b, theme::text_info const& text)
          : vg(th.canvas().context())
          , x(b.left)
          , y(b.top)
          , w(b.width())
          , h(b.height())
          , font(th.text_box_font)
          , font_size(th.text_box_font_size)
          , font_color(th.text_box_font_color)
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

            nvgSave(vg);
            nvgFontSize(vg, font_size);
            nvgFontFace(vg, font);
            nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
            nvgTextMetrics(vg, 0, 0, &lineh);

            NVGtextRow rows[3];

            while (int nrows = nvgTextBreakLines(vg, cp, end, w-right_pad, rows, 3))
            {
               for (std::size_t i = 0; i < nrows; ++i)
               {
                  if (!f(rows[i], i, y, lineh))
                     return;
                  y += lineh;
               }
               // Keep going...
               cp = rows[nrows-1].next;
            }

            nvgRestore(vg);
         }

         void draw_caret(float lineh, float y, float row_width, char const* rstart, char const* rend) const
         {
            if (sstart >= rstart && sstart <= rend)
            {
               std::vector<NVGglyphPosition> glyphs{ std::size_t(rend-rstart) };
               int nglyphs =
                  nvgTextGlyphPositions(
                     vg, x, y, rstart, rend, &glyphs[0], int(glyphs.size())
                  );

               float caretx = x+row_width;
               for (int i = 0; i < nglyphs; ++i)
               {
                  auto const& glyph = glyphs[i];
                  if (sstart == glyph.str)
                     caretx = glyphs[i].x;
               }
               nvgBeginPath(vg);
               nvgFillColor(vg, nvgRGBA(caret_color));
               nvgRect(vg, caretx, y, 1, lineh);
               nvgFill(vg);
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
                  std::vector<NVGglyphPosition> glyphs{ std::size_t(rend-rstart) };
                  int nglyphs =
                     nvgTextGlyphPositions(
                        vg, x, y, rstart, rend, &glyphs[0], int(glyphs.size())
                     );

                  for (int i = 0; i < nglyphs; ++i)
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

               nvgBeginPath(vg);
               nvgRect(vg, x_hilite, y, w_hilite, lineh);
               auto color = is_focus? nvgRGBA(hilite_color) : ::nvgRGBA(255, 255, 255, 16);
               nvgFillColor(vg, color);
               nvgFill(vg);
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

               nvgFillColor(vg, nvgRGBA(is_active ? font_color : inactive_font_color));
               nvgText(vg, x, y, row.start, row.end);
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
                  std::vector<NVGglyphPosition> glyphs{ std::size_t(row.end-row.start) };
                  int nglyphs =
                     nvgTextGlyphPositions(
                        vg, x, y, row.start, row.end, &glyphs[0], int(glyphs.size())
                     );

                  if (nglyphs == 0)
                  {
                     result = row.start;
                     return false;
                  }

                  float px = x;
                  for (int i = 0; i < nglyphs; ++i)
                  {
                     bool  last = i+1 >= nglyphs;
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
               std::vector<NVGglyphPosition> glyphs{ std::size_t(row.end-row.start) };
               int nglyphs =
                  nvgTextGlyphPositions(
                     vg, x, y, row.start, row.end, &glyphs[0], int(glyphs.size())
                  );

               if (nglyphs == 0 && row.start == cp)
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

               for (int i = 0; i < nglyphs; ++i)
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

         NVGcontext*    vg;
         float          x;
         float          y;
         float          w;
         float          h;

         char const*    font;
         float          font_size;
         color          font_color;
         color          inactive_font_color;

         char const*    start;
         char const*    end;
         char const*    sstart;
         char const*    send;
         color          hilite_color;
         color          caret_color;
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
