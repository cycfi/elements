/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/split.hpp>
#include <elements/support/context.hpp>
#include <elements/view.hpp>
#include <elements/support/theme.hpp>
#include <algorithm>
#include <numeric>

namespace cycfi::elements
{
   namespace
   {
      struct span_info
      {
         float min, max, size;
      };

      // Fit the sizes to the extent, within each one's limits: what is too
      // much or too little is spread over the sizes that can still give or
      // take, in proportion to their weights.
      void fit(std::vector<span_info>& spans, std::vector<float> const& weight
       , float extent)
      {
         for (auto& s : spans)
            s.size = std::clamp(s.size, s.min, s.max);

         for (std::size_t pass = 0; pass != spans.size() + 1; ++pass)
         {
            float total = 0;
            for (auto& s : spans)
               total += s.size;
            float diff = extent - total;
            if (std::abs(diff) < 0.01f)
               return;

            float sum_w = 0;
            for (std::size_t i = 0; i != spans.size(); ++i)
            {
               auto& s = spans[i];
               if (diff > 0? s.size < s.max : s.size > s.min)
                  sum_w += weight[i];
            }
            if (sum_w <= 0)
               return;

            for (std::size_t i = 0; i != spans.size(); ++i)
            {
               auto& s = spans[i];
               if (diff > 0? s.size < s.max : s.size > s.min)
                  s.size = std::clamp(
                     s.size + diff * weight[i] / sum_w, s.min, s.max);
            }
         }
      }

      template <axis Axis>
      std::vector<span_info> spans_of(basic_context const& ctx
       , composite_base const& c)
      {
         std::vector<span_info> spans(c.size());
         for (std::size_t i = 0; i != c.size(); ++i)
         {
            auto l = c.at(i).limits(ctx);
            spans[i] = {l.min[Axis], l.max[Axis], l.min[Axis]};
         }
         return spans;
      }
   }

   template <axis Axis>
   view_limits split_element<Axis>::limits(basic_context const& ctx) const
   {
      constexpr auto Other = other(Axis);
      view_limits limits{{0, 0}, {0, 0}};
      limits.max[Other] = full_extent;
      for (std::size_t i = 0; i != size(); ++i)
      {
         auto l = at(i).limits(ctx);
         limits.min[Axis] += l.min[Axis];
         limits.max[Axis] += l.max[Axis];
         clamp_min(limits.min[Other], l.min[Other]);
         clamp_max(limits.max[Other], l.max[Other]);
      }
      clamp_min(limits.max[Other], limits.min[Other]);
      clamp_max(limits.max[Axis], full_extent);
      return limits;
   }

   template <axis Axis>
   void split_element<Axis>::layout(context const& ctx)
   {
      auto const n = size();
      auto const extent = axis_extent(ctx.bounds, Axis);
      auto spans = spans_of<Axis>(ctx, *this);

      std::vector<float> weight(n);
      if (_shares.size() == n)
      {
         // Each keeps its share, as far as the limits allow.
         for (std::size_t i = 0; i != n; ++i)
         {
            spans[i].size = _shares[i] * extent;
            weight[i] = std::max(_shares[i], 0.001f);
         }
      }
      else
      {
         // As a tile: the minimum, and the rest by stretch.
         _shares.clear();
         for (std::size_t i = 0; i != n; ++i)
            weight[i] = at(i).stretch()[Axis];
      }
      fit(spans, weight, extent);

      _tiles.resize(n);
      float pos = 0;
      auto const lo = axis_min(ctx.bounds, Axis);
      auto const other_lo = axis_min(ctx.bounds, other(Axis));
      auto const other_hi = axis_max(ctx.bounds, other(Axis));
      for (std::size_t i = 0; i != n; ++i)
      {
         auto const start = pos;
         pos += spans[i].size;
         _tiles[i] = pos;
         auto& e = at(i);
         auto b = make_rect(Axis, lo + start, other_lo, lo + pos, other_hi);
         e.layout(context{ctx, &e, b});
      }
   }

   template <axis Axis>
   void split_element<Axis>::draw(context const& ctx)
   {
      if (_tiles.size() != size())
         layout(ctx);
      composite_base::draw(ctx);

      // A line on each boundary, so it can be seen and found.
      if (_tiles.size() < 2)
         return;
      auto& cnv = ctx.canvas;
      auto const lo = axis_min(ctx.bounds, Axis);
      auto const other_lo = axis_min(ctx.bounds, other(Axis));
      auto const other_hi = axis_max(ctx.bounds, other(Axis));
      cnv.stroke_style(get_theme().frame_color);
      cnv.line_width(1);
      for (std::size_t i = 0; i + 1 < _tiles.size(); ++i)
      {
         auto const at_ = lo + _tiles[i];
         cnv.move_to(make_rect(Axis, at_, other_lo, at_, other_hi).top_left());
         cnv.line_to(
            make_rect(Axis, at_, other_lo, at_, other_hi).bottom_right());
      }
      cnv.stroke();
   }

   template <axis Axis>
   rect split_element<Axis>::bounds_of(
      context const& ctx, std::size_t index) const
   {
      if (index >= _tiles.size())
         return {};
      auto const lo = axis_min(ctx.bounds, Axis);
      return make_rect(Axis
       , lo + (index? _tiles[index-1] : 0)
       , axis_min(ctx.bounds, other(Axis))
       , lo + _tiles[index]
       , axis_max(ctx.bounds, other(Axis))
      );
   }

   template <axis Axis>
   void split_element<Axis>::shares(std::vector<float> shares_)
   {
      _shares = std::move(shares_);
      _tiles.clear();         // lay out again at the next draw
   }

   // The boundary within grip of p: the index of the child before it, or -1.
   template <axis Axis>
   int split_element<Axis>::boundary_at(context const& ctx, point p) const
   {
      if (!ctx.bounds.includes(p) || _tiles.size() != size())
         return -1;
      auto const at_ = p[Axis] - axis_min(ctx.bounds, Axis);
      for (std::size_t i = 0; i + 1 < _tiles.size(); ++i)
      {
         if (std::abs(at_ - _tiles[i]) <= grip / 2)
            return int(i);
      }
      return -1;
   }

   template <axis Axis>
   element* split_element<Axis>::hit_test(
      context const& ctx, point p, bool leaf, bool control)
   {
      if (boundary_at(ctx, p) >= 0)
         return this;
      return composite_base::hit_test(ctx, p, leaf, control);
   }

   template <axis Axis>
   bool split_element<Axis>::click(context const& ctx, mouse_button btn)
   {
      if (btn.down)
      {
         auto b = boundary_at(ctx, btn.pos);
         if (b >= 0)
         {
            _dragging = b;
            _drag_start = btn.pos[Axis];
            _start_sizes.resize(_tiles.size());
            for (std::size_t i = 0; i != _tiles.size(); ++i)
               _start_sizes[i] = _tiles[i] - (i? _tiles[i-1] : 0);
            return true;
         }
      }
      else if (_dragging >= 0)
      {
         _dragging = -1;
         return true;
      }
      return composite_base::click(ctx, btn);
   }

   template <axis Axis>
   void split_element<Axis>::drag(context const& ctx, mouse_button btn)
   {
      if (_dragging < 0)
      {
         composite_base::drag(ctx, btn);
         return;
      }

      // Trade space between the two children beside the boundary, within
      // both children's limits.
      auto const a = std::size_t(_dragging);
      auto const b = a + 1;
      auto la = at(a).limits(ctx);
      auto lb = at(b).limits(ctx);
      auto const sa = _start_sizes[a];
      auto const sb = _start_sizes[b];
      auto d = btn.pos[Axis] - _drag_start;
      auto const lo = std::max(la.min[Axis] - sa, sb - lb.max[Axis]);
      auto const hi = std::min(la.max[Axis] - sa, sb - lb.min[Axis]);
      d = std::clamp(d, lo, std::max(lo, hi));

      auto sizes = _start_sizes;
      sizes[a] = sa + d;
      sizes[b] = sb - d;
      auto const extent = axis_extent(ctx.bounds, Axis);
      _shares.resize(sizes.size());
      for (std::size_t i = 0; i != sizes.size(); ++i)
         _shares[i] = extent > 0? sizes[i] / extent : 0;

      layout(ctx);
      ctx.view.refresh(ctx);
   }

   template <axis Axis>
   bool split_element<Axis>::cursor(
      context const& ctx, point p, cursor_tracking status)
   {
      if (_dragging >= 0
         || (status != cursor_tracking::leaving && boundary_at(ctx, p) >= 0))
      {
         set_cursor(Axis == axis::x? cursor_type::h_resize
            : cursor_type::v_resize);
         return true;
      }
      return composite_base::cursor(ctx, p, status);
   }

   template <axis Axis>
   std::string split_element<Axis>::class_name() const
   {
      return Axis == axis::x? "hsplit" : "vsplit";
   }

   template class split_element<axis::x>;
   template class split_element<axis::y>;
}
