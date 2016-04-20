/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/theme.hpp>
#include <nanovg.h>

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

   char* cpToUTF8(int cp, char* str)
   {
      int n = 0;
      if (cp < 0x80) n = 1;
      else if (cp < 0x800) n = 2;
      else if (cp < 0x10000) n = 3;
      else if (cp < 0x200000) n = 4;
      else if (cp < 0x4000000) n = 5;
      else if (cp <= 0x7fffffff) n = 6;
      str[n] = '\0';

      switch (n)
      {
         case 6: str[5] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0x4000000;
         case 5: str[4] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0x200000;
         case 4: str[3] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0x10000;
         case 3: str[2] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0x800;
         case 2: str[1] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0xc0;
         case 1: str[0] = cp;
      }
      return str;
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
      draw_text(_vg, b, text, label_font, label_font_size, label_color);
   }

   point theme::measure_label(char const* text) const
   {
      return measure_text(_vg, text, label_font, label_font_size);
   }

   void theme::draw_heading(rect const& b, char const* text) const
   {
      draw_text(_vg, b, text, heading_font, heading_font_size, heading_color);
   }

   point theme::measure_heading(char const* text) const
   {
      return measure_text(_vg, text, heading_font, heading_font_size);
   }

   void theme::draw_icon(rect const& b, uint32_t code, int size) const
   {
      char icon[8];
      draw_text(_vg, b, cpToUTF8(code, icon), icon_font, size, icon_color);
   }

   point theme::measure_icon(uint32_t code, int size) const
   {
      char icon[8];
      return measure_text(_vg, cpToUTF8(code,icon), icon_font, size);
   }

   void theme::draw_text_box(rect const& b, char const* text) const
   {
      photon::draw_text_box(_vg, b, text, text_box_font, text_box_font_size, text_box_color);
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
