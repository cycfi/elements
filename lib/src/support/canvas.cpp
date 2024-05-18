/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

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
   {
      cairo_get_matrix(&context_, &_inv_affine);
      cairo_matrix_invert(&_inv_affine);
   }

   canvas::canvas(canvas&& rhs)
    : _context{rhs._context}
    , _inv_affine{rhs._inv_affine}
   {}

   canvas::~canvas()
   {
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
      // Get the current matrix
      cairo_matrix_t affine;
      cairo_get_matrix(&_context, &affine);

      // Undo the initial transform
      cairo_matrix_t xaf;
      cairo_matrix_multiply(&xaf, &affine, &_inv_affine);

      // Invert the`xaf` transform
      cairo_matrix_invert(&xaf);

      // Map the point to the `xaf` transform
      double x = p.x;
      double y = p.y;
      cairo_matrix_transform_point(&xaf, &x, &y);
      return { float(x), float(y) };
   }

   point canvas::user_to_device(point p)
   {
      // Get the current matrix
      cairo_matrix_t affine;
      cairo_get_matrix(&_context, &affine);

      // Undo the initial transform
      cairo_matrix_t xaf;
      cairo_matrix_multiply(&xaf, &affine, &_inv_affine);

      // Map the point to the `xaf` transform
      double x = p.x;
      double y = p.y;
      cairo_matrix_transform_point(&xaf, &x, &y);
      return { float(x), float(y) };
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
      return {float(x1), float(y1), float(x2), float(y2)};
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

   void canvas::add_rect(struct rect r)
   {
      cairo_rectangle(&_context, r.left, r.top, r.width(), r.height());
   }

   void canvas::add_round_rect(struct rect r_, float radius)
   {
      auto x = r_.left;
      auto y = r_.top;
      auto r = r_.right;
      auto b = r_.bottom;
      auto const a = M_PI/180.0;
      radius = std::min(radius, std::min(r_.width(), r_.height()) / 2);

      cairo_new_sub_path(&_context);
      cairo_arc(&_context, r-radius, y+radius, radius, -90*a, 0*a);
      cairo_arc(&_context, r-radius, b-radius, radius, 0*a, 90*a);
      cairo_arc(&_context, x+radius, b-radius, radius, 90*a, 180*a);
      cairo_arc(&_context, x+radius, y+radius, radius, 180*a, 270*a);
      cairo_close_path(&_context);
   }

   void canvas::add_round_rect(struct rect r_, std::array<float, 4> radius)
   {
      auto x = r_.left;
      auto y = r_.top;
      auto r = r_.right;
      auto b = r_.bottom;
      auto const a = M_PI/180.0;
      for (auto& r : radius) {
         r = std::min(r, std::min(r_.width(), r_.height()) / 2);
      }

      // radii array follow the unit circle defined counterclockwise with 0 at (x, y) = (1, 0)
      // we have to map this to the cairo definition
      cairo_new_sub_path(&_context);
      cairo_arc(&_context, r-radius[0], y+radius[0], radius[0], -90*a, 0*a);
      cairo_arc(&_context, r-radius[3], b-radius[3], radius[3], 0*a, 90*a);
      cairo_arc(&_context, x+radius[2], b-radius[2], radius[2], 90*a, 180*a);
      cairo_arc(&_context, x+radius[1], y+radius[1], radius[1], 180*a, 270*a);
      cairo_close_path(&_context);
   }

   void canvas::round_rect(struct rect r, float radius)
   {
      add_round_rect(r, radius);
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
      font_size(font_._size);
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

   void canvas::fill_text(std::string_view utf8_, point p)
   {
      apply_fill_style();
      std::string utf8(utf8_);
      p = get_text_start(_context, p, _state.align, utf8.c_str());
      cairo_move_to(&_context, p.x, p.y);
      cairo_show_text(&_context, utf8.c_str());
   }

   void canvas::stroke_text(std::string_view utf8_, point p)
   {
      apply_stroke_style();
      std::string utf8(utf8_);
      p = get_text_start(_context, p, _state.align, utf8.c_str());
      cairo_move_to(&_context, p.x, p.y);
      cairo_text_path(&_context, utf8.c_str());
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
         /*size=*/      {float(extents.x_advance + extents.x_bearing), float(extents.height)}
      };
   }

   void canvas::draw(pixmap const& pm, elements::rect src, elements::rect dest)
   {
      auto  state = new_state();
      auto  w = dest.width();
      auto  h = dest.height();
      translate(dest.top_left());
      auto scale_ = point{w/src.width(), h/src.height()};
      scale(scale_);
      cairo_set_source_surface(&_context, pm._surface, -src.left, -src.top);
      add_rect({0, 0, w/scale_.x, h/scale_.y});
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

