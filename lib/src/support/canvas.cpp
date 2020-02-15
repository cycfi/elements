/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/support/canvas.hpp>
#include <cairo.h>
#include <pango/pangocairo.h>

#ifdef __linux__
# include <map>
# include <cairo-ft.h>
# include <ft2build.h>
# include FT_SFNT_NAMES_H
# include FT_FREETYPE_H
# include FT_GLYPH_H
# include FT_OUTLINE_H
# include FT_BBOX_H
# include FT_TYPE1_TABLES_H
#endif

namespace cycfi { namespace elements
{
   canvas::canvas(cairo_t& context_)
    : _context(context_)
    , _pango_context(pango_cairo_create_context(&context_))
   {}

   canvas::canvas(canvas&& rhs)
    : _context(rhs._context)
    , _pango_context(rhs._pango_context)
   {}

   canvas::~canvas()
   {
      g_object_unref(_pango_context);
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

   point canvas::device_to_user(point p)
   {
      double x = p.x;
      double y = p.y;
      cairo_device_to_user(&_context, &x, &y);
      return { float(x), float(y) };
   }

   point canvas::user_to_device(point p)
   {
      double x = p.x;
      double y = p.y;
      cairo_user_to_device(&_context, &x, &y);
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
      _state.fill_style = [this, gr]()
      {
         cairo_pattern_t* pat =
            cairo_pattern_create_linear(
               gr.start.x, gr.start.y, gr.end.x, gr.end.y
            );

         for (auto cs : gr.space)
         {
            cairo_pattern_add_color_stop_rgba(
               pat, cs.offset,
               cs.color.red, cs.color.green, cs.color.blue, cs.color.alpha
            );
         }
         cairo_set_source(&_context, pat);
      };
      if (_state.pattern_set == _state.fill_set)
         _state.pattern_set = _state.none_set;
   }

   void canvas::fill_style(radial_gradient const& gr)
   {
      _state.fill_style = [this, gr]()
      {
         cairo_pattern_t* pat =
            cairo_pattern_create_radial(
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
         cairo_set_source(&_context, pat);
      };
      if (_state.pattern_set == _state.fill_set)
         _state.pattern_set = _state.none_set;
   }

   void canvas::fill_rule(fill_rule_enum rule)
   {
      cairo_set_fill_rule(
         &_context, rule == fill_winding ? CAIRO_FILL_RULE_WINDING : CAIRO_FILL_RULE_EVEN_ODD);
   }

   void canvas::font(char const* face)
   {
#if defined(__linux__) || defined(_WIN32)
      auto fi = _fonts.find(face);
      if (fi != _fonts.end())
      {
         cairo_font_face_t* ct = fi->second;
         cairo_set_font_face(&_context, ct);
         return;
      }
#endif
      cairo_select_font_face(&_context, face, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
   }

   void canvas::font(char const* face, float size)
   {
      font(face);
      font_size(size);
   }

   void canvas::font(elements::font_x const& font_)
   {
      if (font_._handle)
         cairo_set_font_face(&_context, font_._handle);
   }

   void canvas::font(elements::font_x const& font_, float size)
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
         /*size=*/      { float(extents.width), float(extents.height) }
      };
   }

   void canvas::fill(point p, text_layout& layout)
   {
      apply_fill_style();
      pango_cairo_update_context(&_context, _pango_context);
      pango_layout_context_changed(layout._ptr.get());

      cairo_move_to(&_context, p.x, p.y);
      pango_cairo_show_layout(&_context, layout._ptr.get());
   }

   void canvas::stroke(point p, text_layout& layout)
   {
      apply_stroke_style();
      pango_cairo_update_context(&_context, _pango_context);
      pango_layout_context_changed(layout._ptr.get());

      cairo_move_to(&_context, p.x, p.y);
      pango_cairo_layout_path(&_context, layout._ptr.get());
      stroke();
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

#if defined(__linux__) || defined(_WIN32)

#include <cairo-ft.h>
#include <ft2build.h>
#include <freetype/ttnameid.h>
#include FT_SFNT_NAMES_H
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H
#include FT_BBOX_H
#include FT_TYPE1_TABLES_H

namespace cycfi { namespace elements
{
   std::map<std::string, cairo_font_face_t*> canvas::_fonts;
   static std::map<FT_Face, cairo_font_face_t*> font_faces;

   struct font_deleter
   {
      ~font_deleter()
      {
         for (auto& p : font_faces)
         {
            FT_Done_Face(p.first);
            cairo_font_face_destroy(p.second);
         }
      }
   };

   static font_deleter font_deleter_;

   void activate_font(
      std::map<std::string, cairo_font_face_t*>& fonts
    , FT_Library& value
    , fs::path font_path)
   {
      cairo_font_face_t* ct = nullptr;
      FT_Face face;
      FT_Error status = FT_New_Face(value, font_path.string().c_str(), 0, &face);
      if (status != 0)
         return;
      ct = cairo_ft_font_face_create_for_ft_face(face, 0);

      auto count = FT_Get_Sfnt_Name_Count(face);
      for (int i = 0; i < count; ++i)
      {
         FT_SfntName name;
         FT_Get_Sfnt_Name(face, i, &name);
         FT_Get_Sfnt_Name(face, i, &name);
         if (name.name_id == TT_NAME_ID_FULL_NAME)
         {
            font_faces[face] = ct;
            fonts[std::string((char const*)name.string, name.string_len)] = ct;
            break;
         }
      }
   }

   void canvas::load_fonts(fs::path resource_path)
   {
      FT_Library value;
      FT_Error status = FT_Init_FreeType(&value);
      if (status != 0)
         return;

      // Load the user fonts from the Resource folder. Normally this is automatically
      // done on application startup, but for plugins, we need to explicitly load
      // the user fonts ourself.
      for (fs::directory_iterator it{ resource_path }; it != fs::directory_iterator{}; ++it)
         if (it->path().extension() == ".ttf")
            activate_font(_fonts, value, it->path());
   }
}}

#endif
