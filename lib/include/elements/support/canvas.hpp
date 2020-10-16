/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_CANVAS_MAY_3_2016)
#define ELEMENTS_CANVAS_MAY_3_2016

#include <elements/support/color.hpp>
#include <elements/support/rect.hpp>
#include <elements/support/circle.hpp>
#include <elements/support/pixmap.hpp>
#include <elements/support/font.hpp>
#include <infra/filesystem.hpp>

#include <vector>
#include <functional>
#include <stack>
#include <cmath>
#include <cassert>

extern "C"
{
   typedef struct _cairo cairo_t;
}

namespace cycfi { namespace elements
{
   class canvas
   {
   public:

      explicit          canvas(cairo_t& context_);
                        canvas(canvas&& rhs);
                        ~canvas();

                        canvas(canvas const& rhs) = delete;
      canvas&           operator=(canvas const& rhs) = delete;
      cairo_t&          cairo_context() const;

      void              pre_scale(float sc);
      float             pre_scale() const;

      ///////////////////////////////////////////////////////////////////////////////////
      // Transforms
      void              translate(point p);
      void              rotate(float rad);
      void              scale(point p);
      void              skew(float sx, float sy);
      point             device_to_user(point p);
      point             user_to_device(point p);

      ///////////////////////////////////////////////////////////////////////////////////
      // Paths
      void              begin_path();
      void              close_path();
      void              fill();
      void              fill_preserve();
      void              stroke();
      void              stroke_preserve();
      void              clip();
      elements::rect    clip_extent() const;
      bool              hit_test(point p) const;
      elements::rect    fill_extent() const;

      void              move_to(point p);
      void              line_to(point p);
      void              arc_to(point p1, point p2, float radius);
      void              arc(
                           point p, float radius,
                           float start_angle, float end_angle,
                           bool ccw = false
                        );
      void              rect(elements::rect r);
      void              round_rect(elements::rect r, float radius);
      void              circle(elements::circle c);

      ///////////////////////////////////////////////////////////////////////////////////
      // Styles
      void              fill_style(color c);
      void              stroke_style(color c);
      void              line_width(float w);

      ///////////////////////////////////////////////////////////////////////////////////
      // Gradients
      struct color_stop
      {
         float             offset;
         elements::color   color;
      };

      struct linear_gradient
      {
         point start = {};
         point end = {};

         void  add_color_stop(color_stop cs);
         std::vector<color_stop> space = {};
      };

      struct radial_gradient
      {
         point c1 = {};
         float c1_radius = {};
         point c2 = c1;
         float c2_radius = c1_radius;

         void  add_color_stop(color_stop cs);
         std::vector<color_stop> space = {};
      };

      void              fill_style(linear_gradient const& gr);
      void              fill_style(radial_gradient const& gr);

      enum fill_rule_enum
      {
         fill_winding,
         fill_odd_even
      };

      void              fill_rule(fill_rule_enum rule);

      ///////////////////////////////////////////////////////////////////////////////////
      // Rectangles
      void              fill_rect(elements::rect r);
      void              fill_round_rect(elements::rect r, float radius);
      void              stroke_rect(elements::rect r);
      void              stroke_round_rect(elements::rect r, float radius);

      ///////////////////////////////////////////////////////////////////////////////////
      // Font
      void              font(elements::font const& font_);
      void              font(elements::font const& font_, float size);
      void              font_size(float size);

      ///////////////////////////////////////////////////////////////////////////////////
      // Text
      enum text_alignment
      {
         // Horizontal align
         left     = 0,        // Default, align text horizontally to left.
         center   = 1,        // Align text horizontally to center.
         right    = 2,        // Align text horizontally to right.

         // Vertical align
         baseline = 4,        // Default, align text vertically to baseline.
         top      = 8,        // Align text vertically to top.
         middle   = 12,       // Align text vertically to middle.
         bottom   = 16        // Align text vertically to bottom.
      };

      struct text_metrics
      {
         float       ascent;
         float       descent;
         float       leading;
         point       size;
      };

      void              fill_text(point p, char const* utf8);
      void              stroke_text(point p, char const* utf8);
      text_metrics      measure_text(char const* utf8);
      void              text_align(int align);

      ///////////////////////////////////////////////////////////////////////////////////
      // Pixmaps

      void              draw(pixmap const& pm, elements::rect src, elements::rect dest);
      void              draw(pixmap const& pm, elements::rect dest);
      void              draw(pixmap const& pm, point pos);

      ///////////////////////////////////////////////////////////////////////////////////
      // States
      class state
      {
      public:
                        state(canvas& cnv_);
                        state(state&& rhs) noexcept;
                        state(state const&) = delete;
                        ~state();

         state&         operator=(state const&) = delete;
         state&         operator=(state&& rhs) noexcept;

      private:

         canvas* cnv;
      };

      state             new_state()   { return state{ *this }; }
      void              save();
      void              restore();

   private:

      friend class glyphs;

      void              apply_fill_style();
      void              apply_stroke_style();

      struct canvas_state
      {
         std::function<void()>   stroke_style;
         std::function<void()>   fill_style;
         int                     align          = 0;

         enum pattern_state { none_set, stroke_set, fill_set };
         pattern_state           pattern_set = none_set;
      };

      using state_stack = std::stack<canvas_state>;

      cairo_t&          _context;
      canvas_state      _state;
      state_stack       _state_stack;
      float             _pre_scale = 1.0f;
   };
}}

#include <elements/support/detail/canvas_impl.hpp>
#endif
