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
   inline NVGcolor nvgRGBA(color const& c)
   {
      NVGcolor color;
      color.r = c.red;
      color.g = c.green;
      color.b = c.blue;
      color.a = c.alpha;
      return color;
   }

   // Returns true if col.rgba is 0.0f, 0.0f, 0.0f, 0.0f, false otherwise
   inline bool is_black(NVGcolor col)
   {
      return (col.r == 0.0f && col.g == 0.0f && col.b == 0.0f && col.a == 0.0f);
   }

   void theme::draw_panel(rect const& b) const
   {
      double   x = b.left;
      double   y = b.top;
      double   w = b.width();
      double   h = b.height();
      double   r = panel_corner_radius;
      rect     sh = panel_shadow_offset;

      // Round Rectangle
      nvgBeginPath(_vg);
      nvgRoundedRect(_vg, x, y, w, h, r);
      nvgFillColor(_vg, nvgRGBA(panel_color));
      nvgFill(_vg);

      // Drop shadow
      nvgBeginPath(_vg);
      nvgRect(_vg, x+sh.left, y+sh.top, w+sh.right, h+sh.bottom);
      nvgRoundedRect(_vg, x, y, w, h, r);
      nvgPathWinding(_vg, NVG_HOLE);

      NVGpaint shadow_paint
         = nvgBoxGradient(
               _vg, x, y+2, w, h, r*2, 10
             , ::nvgRGBA(0, 0, 0, 128), ::nvgRGBA(0, 0, 0, 0)
            );

      nvgFillPaint(_vg, shadow_paint);
      nvgFill(_vg);
   }

   void theme::draw_frame(rect const& b) const
   {
      double   x = b.left;
      double   y = b.top;
      double   w = b.width();
      double   h = b.height();
      double   r = frame_corner_radius;

      // Round Rectangle
      nvgBeginPath(_vg);
      nvgRoundedRect(_vg, x, y, w, h, r);
      nvgStrokeColor(_vg, nvgRGBA(frame_color));
      nvgStrokeWidth(_vg, frame_stroke_width);
      nvgStroke(_vg);
   }

   namespace
   {
      void draw_slot(NVGcontext* vg, double x, double y, double w, double h, double r)
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
         NVGcontext* vg, double cx, double cy, double r,
         color const& fill_color,
         color const& outline_color
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

   void theme::draw_slider(double pos, rect const& b) const
   {
      double   x = b.left;
      double   y = b.top;
      double   w = b.width();
      double   h = b.height();

      nvgSave(_vg);

      if (w > h)
      {
         // horizontal
         double   cy = y + h * 0.5;
         double   sl = h * slider_slot_size;
         draw_slot(_vg, x, cy-(sl/2), w, sl, sl/3);
      }
      else
      {
         // vertical
         double   cx = x + w * 0.5;
         double   sl = w * slider_slot_size;
         draw_slot(_vg, cx-(sl/2), y, sl, h, sl/3);
      }

      draw_slider_knob(pos, b);
      nvgRestore(_vg);
   }

   void theme::draw_slider_knob(double pos, rect const& b) const
   {
      circle cp = slider_knob_position(pos, b);
      draw_knob(
         _vg, cp.cx, cp.cy, cp.radius,
         slider_knob_fill_color, slider_knob_outline_color
      );
   }

   circle theme::slider_knob_position(double pos, rect const& b) const
   {
      double   x = b.left;
      double   y = b.top;
      double   w = b.width();
      double   h = b.height();

      if (w > h)
      {
         // horizontal
         double   cy = y + h * 0.5;
         double   kr = h * slider_knob_radius;

         // inset by radius;
         w -= kr * 2;
         x += kr;

         return { x+(pos*w), cy, kr };
      }
      else
      {
         // vertical
         double   cx = x + w * 0.5;
         double   kr = w * slider_knob_radius;

         // inset by radius;
         h -= kr * 2;
         y += kr;

         return { cx, y+(pos*h), kr };
      }
   }

   namespace
   {
      void draw_scrollbar_fill(NVGcontext* vg, double x, double y, double w, double h)
      {
         nvgBeginPath(vg);
         nvgRect(vg, x, y, w, h);
         nvgFillColor(vg, ::nvgRGBA(200, 200, 200, 30));
         nvgFill(vg);
      }

      void draw_scrollbar(
         NVGcontext* vg, double x, double y, double w, double h, double r,
         color const& outline_color_, color const& fill_color_
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

   void theme::draw_scroll_bar(double pos, double ext, rect const& b) const
   {
      double   x = b.left;
      double   y = b.top;
      double   w = b.width();
      double   h = b.height();

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

   rect theme::scroll_bar_position(double pos, double ext, rect const& b) const
   {
      double   x = b.left;
      double   y = b.top;
      double   w = b.width();
      double   h = b.height();

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
         NVGcontext* vg, rect const& b, char const* text
       , char const* font, double font_size, color const& color
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
       , char const* font, double font_size
      )
      {
         nvgFontSize(vg, font_size);
         nvgFontFace(vg, font);

         float w = nvgTextBounds(vg, 0, 0, text, 0, 0);
         float h = font_size;
         return { w, h };
      }

      void draw_text_box(
         NVGcontext* vg, rect const& b, char const* text
       , char const* font, double font_size, color const& color
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

   void theme::draw_label(rect const& b, char const* text) const
   {
      draw_text(_vg, b, text, label_font, label_font_size, label_font_color);
   }

   point theme::measure_label(char const* text) const
   {
      return measure_text(_vg, text, label_font, label_font_size);
   }

   void theme::draw_heading(rect const& b, char const* text) const
   {
      draw_text(_vg, b, text, heading_font, heading_font_size, heading_font_color);
   }

   point theme::measure_heading(char const* text) const
   {
      return measure_text(_vg, text, heading_font, heading_font_size);
   }

   void theme::draw_icon(rect const& b, uint32_t code, int size) const
   {
      char icon[8];
      draw_text(_vg, b, codepoint_to_UTF8(code, icon), icon_font, size, icon_color);
   }

   point theme::measure_icon(uint32_t code, int size) const
   {
      char icon[8];
      return measure_text(_vg, codepoint_to_UTF8(code,icon), icon_font, size);
   }

   void theme::draw_text_box(rect const& b, char const* text) const
   {
      photon::draw_text_box(_vg, b, text, text_box_font, text_box_font_size, text_box_font_color);
   }

   namespace
   {
      struct edit_text_box_renderer
      {
         float const right_pad = 5;

         edit_text_box_renderer(theme const& th, rect const& b, theme::text_draw_info const& text)
          : vg(th.canvas())
          , x(b.left)
          , y(b.top)
          , w(b.width())
          , h(b.height())
          , font(th.text_box_font)
          , font_size(th.text_box_font_size)
          , font_color(th.text_box_font_color)
          , start(text.first)
          , end(text.last)
          , sstart(text.select_first)
          , send(text.select_last)
          , hilite_color(th.text_box_hilite_color)
          , caret_color(th.text_box_caret_color)
          , is_focus(text.is_focus)
         {
            if (sstart > send)
               std::swap(sstart, send);
         }

         edit_text_box_renderer(theme const& th, rect const& b, theme::text_info const& text)
          : vg(th.canvas())
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

               nvgFillColor(vg, nvgRGBA(font_color));
               nvgText(vg, x, y, row.start, row.end);
               return true;
            };

            float y;
            float lineh;
            for_each_row(draw_f, y, lineh);
         }

         double height() const
         {
            auto height_f = [&](auto& row, auto i, auto y, auto lineh) { return true; };

            float y;
            float lineh;
            for_each_row(height_f, y, lineh);
            return y + lineh;
         }

         char const* caret_position(point const& p) const
         {
            float       mx = p.x;
            float       my = p.y;
            char const* result = 0;

            auto caret_f = [&](auto& row, auto i, auto y, auto lineh)
            {
               if (mx >= x && mx < (x+w) && my >= y && my < (y+lineh))
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
                  result = { row.start, x, y, lineh, row.minx, row.maxx };
                  return false;
               }
               else if (row.end == cp)
               {
                  result = { row.end, row.maxx, y, lineh, row.maxx, row.maxx };
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

         char const*    start;
         char const*    end;
         char const*    sstart;
         char const*    send;
         color          hilite_color;
         color          caret_color;
         bool           is_focus;
      };
   }

   void theme::draw_edit_text_box(rect const& b, text_draw_info const& text) const
   {
      edit_text_box_renderer r{ *this, b, text };
      r.draw();
   }

   char const* theme::caret_position(rect const& b, text_info const& text, point const& p) const
   {
      edit_text_box_renderer r{ *this, b, text };
      return r.caret_position(p);
   }

   double theme::edit_text_box_height(rect const& b, text_info const& text) const
   {
      edit_text_box_renderer r{ *this, b, text };
      return r.height();
   }

   theme::glyph_info theme::glyph_bounds(rect const& b, text_info const& text, char const* cp) const
   {
      edit_text_box_renderer r{ *this, b, text };
      return r.glyph_bounds(cp);
   }

   void theme::draw_button(rect const& b, color const& button_color) const
   {
      double   x = b.left;
      double   y = b.top;
      double   w = b.width();
      double   h = b.height();
      NVGcolor col = nvgRGBA(button_color);
      bool     black = is_black(col);

      NVGpaint bg =
         nvgLinearGradient(
            _vg, x, y, x, y+h,
            ::nvgRGBA(255, 255, 255, black ? 16 : 32),
            ::nvgRGBA(0, 0, 0, black ? 16 : 32));

      nvgBeginPath(_vg);
      nvgRoundedRect(_vg, x+1, y+1, w-2, h-2, button_corner_radius-1);
      if (!black)
      {
         nvgFillColor(_vg, col);
         nvgFill(_vg);
      }
      nvgFillPaint(_vg, bg);
      nvgFill(_vg);

      nvgBeginPath(_vg);
      nvgRoundedRect(_vg, x+0.5, y+0.5, w-1, h-1, button_corner_radius-0.5);
      nvgStrokeColor(_vg, ::nvgRGBA(0, 0, 0, 48));
      nvgStroke(_vg);
   }

   void theme::load_fonts() const
   {
      if (nvgCreateFont(_vg, "icons", "./assets/fonts/entypo.ttf") == -1)
      {
         printf("Could not add font icons.\n");
          // $$$ throw $$;
      }

      if (nvgCreateFont(_vg, "sans", "./assets/fonts/Roboto-Regular.ttf") == -1)
      {
         printf("Could not add font italic.\n");
          // $$$ throw $$;
      }

      if (nvgCreateFont(_vg, "sans-bold", "./assets/fonts/Roboto-Bold.ttf") == -1)
      {
         printf("Could not add font bold.\n");
          // $$$ throw $$;
      }
   }
}
