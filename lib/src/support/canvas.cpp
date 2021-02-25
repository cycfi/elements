/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/support/canvas.hpp>
#include <cairo.h>

#include <memory>

namespace cycfi { namespace elements
{
   namespace
   {
      struct cairo_pattern_deleter
      {
         void operator()(cairo_pattern_t* pattern) const
         {
            if (pattern)
               cairo_pattern_destroy(pattern);
         }
      };

      // use shared pointer instead of unique because std::function wants copyable types
      using cairo_pattern_ptr = std::shared_ptr<cairo_pattern_t>;

      cairo_pattern_ptr make_linear_pattern(canvas::linear_gradient const& gr)
      {
         cairo_pattern_t* pat = cairo_pattern_create_linear(
            gr.start.x, gr.start.y, gr.end.x, gr.end.y
         );

         for (auto cs : gr.space)
         {
            cairo_pattern_add_color_stop_rgba(
               pat, cs.offset,
               cs.color.red, cs.color.green, cs.color.blue, cs.color.alpha
            );
         }

         return cairo_pattern_ptr(pat, cairo_pattern_deleter{});
      }

      cairo_pattern_ptr make_radial_pattern(canvas::radial_gradient const& gr)
      {
         cairo_pattern_t* pat = cairo_pattern_create_radial(
            gr.c1.x, gr.c1.y, gr.c1_radius,
            gr.c2.x, gr.c2.y, gr.c2_radius
         );

         for (auto cs : gr.space)
         {
            cairo_pattern_add_color_stop_rgba(
               pat, cs.offset,
               cs.color.red, cs.color.green, cs.color.blue, cs.color.alpha
            );
         }

         return cairo_pattern_ptr(pat, cairo_pattern_deleter{});
      }
   }

   canvas::canvas(cairo_t& context_)
    : _context(context_)
   {}

   canvas::canvas(canvas&& rhs)
    : _context(rhs._context)
   {}

   canvas::~canvas()
   {
   }

   void canvas::pre_scale(float sc)
   {
      scale({ sc, sc });
      _pre_scale = sc;
   }

   float canvas::pre_scale() const
   {
      return _pre_scale;
   }

   void canvas::translate(point p)
   {
      cairo_translate(&_context, p.x, p.y);
   }

   void canvas::rotate(float rad)
   {
      cairo_rotate(&_context, rad);
   }

   void canvas::scale(point p)
   {
      cairo_scale(&_context, p.x, p.y);
   }

   void canvas::skew(float sx, float sy)
   {
      cairo_matrix_t mat;
      cairo_matrix_init(&mat, 1, 0, sx, 1, 0, sy);
      cairo_transform(&_context, &mat);
   }

   point canvas::device_to_user(point p)
   {
      double x = p.x * _pre_scale;
      double y = p.y * _pre_scale;
      cairo_device_to_user_distance(&_context, &x, &y);
      return { float(x), float(y) };
   }

   point canvas::user_to_device(point p)
   {
      double x = p.x;
      double y = p.y;
      cairo_user_to_device_distance(&_context, &x, &y);
      return { float(x / _pre_scale), float(y / _pre_scale) };
   }

   void canvas::begin_path()
   {
      cairo_new_path(&_context);
   }

   void canvas::close_path()
   {
      cairo_close_path(&_context);
   }

   void canvas::fill()
   {
      apply_fill_style();
      cairo_fill(&_context);
   }

   void canvas::fill_preserve()
   {
      apply_fill_style();
      cairo_fill_preserve(&_context);
   }

   void canvas::stroke()
   {
      apply_stroke_style();
      cairo_stroke(&_context);
   }

   void canvas::stroke_preserve()
   {
      apply_stroke_style();
      cairo_stroke_preserve(&_context);
   }

   void canvas::clip()
   {
      cairo_clip(&_context);
   }

   rect canvas::clip_extent() const
   {
      double x1, y1, x2, y2;
      cairo_clip_extents(&_context, &x1, &y1, &x2, &y2);
      return { float(x1), float(y1), float(x2), float(y2) };
   }

   bool canvas::hit_test(point p) const
   {
      return cairo_in_fill(&_context, p.x, p.y);
   }

   rect canvas::fill_extent() const
   {
      double x1, y1, x2, y2;
      cairo_fill_extents(&_context, &x1, &y1, &x2, &y2);
      return elements::rect(x1, y1, x2, y2);
   }

   void canvas::move_to(point p)
   {
      cairo_move_to(&_context, p.x, p.y);
   }

   void canvas::line_to(point p)
   {
      cairo_line_to(&_context, p.x, p.y);
   }

   void canvas::arc_to(point /* p1 */, point /* p2 */, float /* radius */)
   {
      assert(false); // unimplemented
   }

   void canvas::arc(
      point p, float radius,
      float start_angle, float end_angle,
      bool ccw
   )
   {
      if (ccw)
         cairo_arc_negative(&_context, p.x, p.y, radius, start_angle, end_angle);
      else
         cairo_arc(&_context, p.x, p.y, radius, start_angle, end_angle);
   }

   void canvas::rect(struct rect r)
   {
      cairo_rectangle(&_context, r.left, r.top, r.width(), r.height());
   }

   void canvas::round_rect(struct rect bounds, float radius)
   {
      auto x = bounds.left;
      auto y = bounds.top;
      auto r = bounds.right;
      auto b = bounds.bottom;
      auto const a = M_PI/180.0;
      radius = std::min(radius, std::min(bounds.width(), bounds.height()));

      cairo_new_sub_path(&_context);
      cairo_arc(&_context, r-radius, y+radius, radius, -90*a, 0*a);
      cairo_arc(&_context, r-radius, b-radius, radius, 0*a, 90*a);
      cairo_arc(&_context, x+radius, b-radius, radius, 90*a, 180*a);
      cairo_arc(&_context, x+radius, y+radius, radius, 180*a, 270*a);
      cairo_close_path(&_context);
   }

   void canvas::fill_style(color c)
   {
      _state.fill_style = [this, c]()
      {
         cairo_set_source_rgba(&_context, c.red, c.green, c.blue, c.alpha);
      };
      if (_state.pattern_set == _state.fill_set)
         _state.pattern_set = _state.none_set;
   }

   void canvas::stroke_style(color c)
   {
      _state.stroke_style = [this, c]()
      {
         cairo_set_source_rgba(&_context, c.red, c.green, c.blue, c.alpha);
      };
      if (_state.pattern_set == _state.stroke_set)
         _state.pattern_set = _state.none_set;
   }

   void canvas::line_width(float w)
   {
      cairo_set_line_width(&_context, w);
   }

   void canvas::fill_style(linear_gradient const& gr)
   {
      _state.fill_style = [this, pat = make_linear_pattern(gr)]()
      {
         cairo_set_source(&_context, pat.get());
      };
      if (_state.pattern_set == _state.fill_set)
         _state.pattern_set = _state.none_set;
   }

   void canvas::fill_style(radial_gradient const& gr)
   {
      _state.fill_style = [this, pat = make_radial_pattern(gr)]()
      {
         cairo_set_source(&_context, pat.get());
      };
      if (_state.pattern_set == _state.fill_set)
         _state.pattern_set = _state.none_set;
   }

   void canvas::fill_rule(fill_rule_enum rule)
   {
      cairo_set_fill_rule(
         &_context, rule == fill_winding ? CAIRO_FILL_RULE_WINDING : CAIRO_FILL_RULE_EVEN_ODD);
   }

   void canvas::font(elements::font const& font_)
   {
      if (font_._handle)
         cairo_set_font_face(&_context, font_._handle);
   }

   void canvas::font(elements::font const& font_, float size)
   {
      font(font_);
      font_size(size);
   }

   void canvas::font_size(float size)
   {
      cairo_set_font_size(&_context, size);
   }

   namespace
   {
      point get_text_start(cairo_t& _context, point p, int align, char const* utf8)
      {
         cairo_text_extents_t extents;
         cairo_text_extents(&_context, utf8, &extents);

         cairo_font_extents_t font_extents;
         cairo_scaled_font_extents(cairo_get_scaled_font(&_context), &font_extents);

         switch (align & 0x3)
         {
            case canvas::text_alignment::right:
               p.x -= extents.width;
               break;
            case canvas::text_alignment::center:
               p.x -= extents.width/2;
               break;
            default:
               break;
         }

         switch (align & 0x1C)
         {
            case canvas::text_alignment::top:
               p.y += font_extents.ascent;
               break;

            case canvas::text_alignment::middle:
               p.y += font_extents.ascent/2 - font_extents.descent/2;
               break;

            case canvas::text_alignment::bottom:
               p.y -= font_extents.descent;
               break;

            default:
               break;
         }

         return p;
      }
   }

   void canvas::fill_text(point p, char const* utf8)
   {
      apply_fill_style();
      p = get_text_start(_context, p, _state.align, utf8);
      cairo_move_to(&_context, p.x, p.y);
      cairo_show_text(&_context, utf8);
   }

   void canvas::stroke_text(point p, char const* utf8)
   {
      apply_stroke_style();
      p = get_text_start(_context, p, _state.align, utf8);
      cairo_move_to(&_context, p.x, p.y);
      cairo_text_path(&_context, utf8);
      stroke();
   }

   canvas::text_metrics canvas::measure_text(char const* utf8)
   {
      cairo_text_extents_t extents;
      cairo_text_extents(&_context, utf8, &extents);

      cairo_font_extents_t font_extents;
      cairo_scaled_font_extents(cairo_get_scaled_font(&_context), &font_extents);

      return {
         /*ascent=*/    float(font_extents.ascent),
         /*descent=*/   float(font_extents.descent),
         /*leading=*/   float(font_extents.height-(font_extents.ascent+font_extents.descent)),
         /*size=*/      { float(extents.x_advance + extents.x_bearing), float(extents.height) }
      };
   }

   void canvas::draw(pixmap const& pm, elements::rect src, elements::rect dest)
   {
      auto  state = new_state();
      auto  w = dest.width();
      auto  h = dest.height();
      translate(dest.top_left());
      auto scale_ = point{ w/src.width(), h/src.height() };
      scale(scale_);
      cairo_set_source_surface(&_context, pm._surface, -src.left, -src.top);
      rect({ 0, 0, w/scale_.x, h/scale_.y });
      cairo_fill(&_context);
   }

   void canvas::save()
   {
      cairo_save(&_context);
      _state_stack.push(_state);
   }

   void canvas::restore()
   {
      _state = _state_stack.top();
      _state_stack.pop();
      cairo_restore(&_context);
   }
}}

