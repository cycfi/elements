/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_DETAIL_CANVAS_IMPL_MAY_3_2016)
#define ELEMENTS_DETAIL_CANVAS_IMPL_MAY_3_2016

#ifndef M_PI
# define M_PI 3.14159265358979323846
#endif

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Inlines
   ////////////////////////////////////////////////////////////////////////////
   inline cairo_t& canvas::cairo_context() const
   {
      return _context;
   }

   inline void canvas::circle(struct circle c)
   {
      arc(point{ c.cx, c.cy }, c.radius, 0.0, 2 * M_PI);
   }

   inline void canvas::linear_gradient::add_color_stop(color_stop cs)
   {
      space.push_back(cs);
   }

   inline void canvas::radial_gradient::add_color_stop(color_stop cs)
   {
      space.push_back(cs);
   }

   inline void canvas::fill_rect(struct rect r)
   {
      rect(r);
      fill();
   }

   inline void canvas::fill_round_rect(struct rect r, float radius)
   {
      round_rect(r, radius);
      fill();
   }

   inline void canvas::stroke_rect(struct rect r)
   {
      rect(r);
      stroke();
   }

   inline void canvas::stroke_round_rect(struct rect r, float radius)
   {
      round_rect(r, radius);
      stroke();
   }

   inline void canvas::text_align(int align)
   {
      _state.align = align;
   }

   inline void canvas::draw(pixmap const& pm, elements::rect dest)
   {
      draw(pm, { 0, 0, pm.size() }, dest);
   }

   inline void canvas::draw(pixmap const& pm, point pos)
   {
      draw(pm, { 0, 0, pm.size() }, { pos, pm.size() });
   }

   inline canvas::state::state(canvas& cnv_)
     : cnv(&cnv_)
   {
      cnv->save();
   }

   inline canvas::state::state(state&& rhs) noexcept
    : cnv(rhs.cnv)
   {
      rhs.cnv = 0;
   }

   inline canvas::state::~state()
   {
      if (cnv)
         cnv->restore();
   }

   inline canvas::state& canvas::state::operator=(state&& rhs) noexcept
   {
      if (this != &rhs)
      {
         cnv = rhs.cnv;
         rhs.cnv = nullptr;
      }
      return *this;
   }

   inline void canvas::apply_fill_style()
   {
      if (_state.pattern_set != _state.fill_set && _state.fill_style)
      {
         _state.fill_style();
         _state.pattern_set = _state.fill_set;
      }
   }

   inline void canvas::apply_stroke_style()
   {
      if (_state.pattern_set != _state.stroke_set && _state.stroke_style)
      {
         _state.stroke_style();
         _state.pattern_set = _state.stroke_set;
      }
   }

   // Declared in context.hpp
   inline rect device_to_user(rect const& r, canvas& cnv)
   {
      return {
         cnv.device_to_user(r.top_left())
       , cnv.device_to_user(r.size())
      };
   }

   // Declared in context.hpp
   inline point device_to_user(point p, canvas& cnv)
   {
      return cnv.device_to_user(p);
   }
}}

#endif
