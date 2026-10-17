/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/tooltip.hpp>
#include <elements/view.hpp>
#include <elements/element/traversal.hpp>
#include <elements/support/theme.hpp>
#include <algorithm>

namespace cycfi::elements
{
   rect tooltip_element::tip_bounds(context const& ctx, tooltip_position& side) const
   {
      auto limits_ = _tip->limits(ctx);
      auto w = limits_.min.x;
      auto h = limits_.min.y;
      auto center = ctx.bounds.left + (ctx.bounds.width()/2);
      auto middle = ctx.bounds.top + (ctx.bounds.height()/2);
      rect bounds{0, 0, w, h};

      auto const view = ctx.view_bounds();
      auto at = [&](tooltip_position pos)
      {
         switch (pos)
         {
            case tooltip_position::left:
               return bounds.move_to(ctx.bounds.left-w, middle-(h/2));
            case tooltip_position::top:
               return bounds.move_to(center-(w/2), ctx.bounds.top-h);
            case tooltip_position::right:
               return bounds.move_to(ctx.bounds.right, middle-(h/2));
            default:
               return bounds.move_to(center-(w/2), ctx.bounds.bottom);
         }
      };
      auto opposite = [](tooltip_position pos)
      {
         switch (pos)
         {
            case tooltip_position::left:  return tooltip_position::right;
            case tooltip_position::top:   return tooltip_position::bottom;
            case tooltip_position::right: return tooltip_position::left;
            default:                      return tooltip_position::top;
         }
      };

      // On the side asked for, or the opposite one if it does not fit
      // there and does on the other; then moved into the view.
      bool const across = _position == tooltip_position::left
         || _position == tooltip_position::right;
      auto fits = [&](rect r)
      {
         return across
            ? r.left >= view.left && r.right <= view.right
            : r.top >= view.top && r.bottom <= view.bottom;
      };
      side = _position;
      auto r = at(_position);
      if (!fits(r))
      {
         if (auto other = at(opposite(_position)); fits(other))
         {
            r = other;
            side = opposite(_position);
         }
      }
      float dx = 0, dy = 0;
      if (r.right > view.right)
         dx = view.right - r.right;
      if (r.left + dx < view.left)
         dx = view.left - r.left;
      if (r.bottom > view.bottom)
         dy = view.bottom - r.bottom;
      if (r.top + dy < view.top)
         dy = view.top - r.top;
      return r.move(dx, dy);
   }

   ////////////////////////////////////////////////////////////////////////////
   // tooltip_bubble_base
   ////////////////////////////////////////////////////////////////////////////
   view_limits tooltip_bubble_base::limits(basic_context const& ctx) const
   {
      auto l = subject().limits(ctx);
      auto const room = 2 * pointer;
      l.min.x += room;
      l.min.y += room;
      l.max.x = std::min<float>(l.max.x + room, full_extent);
      l.max.y = std::min<float>(l.max.y + room, full_extent);
      return l;
   }

   void tooltip_bubble_base::prepare_subject(context& ctx)
   {
      ctx.bounds = ctx.bounds.inset(pointer);
   }

   void tooltip_bubble_base::anchor(tooltip_position side, point at)
   {
      _side = side;
      _at = at;
   }

   void tooltip_bubble_base::draw(context const& ctx)
   {
      auto const& thm = get_theme();
      auto& cnv = ctx.canvas;
      {
         auto state = cnv.new_state();
         float const radius = 6;
         auto const b = ctx.bounds;
         auto const body = b.inset(pointer);

         // The pointer, from the side of the body facing the element, kept
         // clear of the body's rounded corners.
         auto along = [&](float v, float lo, float hi)
         {
            auto const a = lo + radius + pointer;
            auto const z = hi - radius - pointer;
            return a < z? std::clamp(v, a, z) : (lo + hi) / 2;
         };

         // One outline, clockwise from the top left corner, with the pointer
         // on the edge facing the element: a separate pointer path would
         // leave a seam where the two meet.
         auto const l = body.left, t = body.top, r = body.right, bt = body.bottom;
         auto tip = [&](tooltip_position side, point p0, point apex, point p1)
         {
            if (_side == side)
            {
               cnv.line_to(p0);
               cnv.line_to(apex);
               cnv.line_to(p1);
            }
         };
         auto const x = along(_at.x, l, r);
         auto const y = along(_at.y, t, bt);

         cnv.move_to(l + radius, t);
         tip(tooltip_position::bottom, {x - pointer, t}, {x, b.top}, {x + pointer, t});
         cnv.arc_to({r, t}, {r, t + radius}, radius);
         tip(tooltip_position::left, {r, y - pointer}, {b.right, y}, {r, y + pointer});
         cnv.arc_to({r, bt}, {r - radius, bt}, radius);
         tip(tooltip_position::top, {x + pointer, bt}, {x, b.bottom}, {x - pointer, bt});
         cnv.arc_to({l, bt}, {l, bt - radius}, radius);
         tip(tooltip_position::right, {l, y + pointer}, {b.left, y}, {l, y - pointer});
         cnv.arc_to({l, t}, {l + radius, t}, radius);
         cnv.close_path();

         cnv.shadow_style({0, 5}, 14, thm.tooltip_shadow_color);
         cnv.fill_style(thm.tooltip_color);
         cnv.fill();
      }
      proxy_base::draw(ctx);
   }

   std::string tooltip_bubble_base::class_name() const
   {
      return "tooltip_bubble";
   }

   tooltip_element::tooltip_element(tooltip_element const& rhs)
    : base_type{rhs}
    , on_hover{rhs.on_hover}
    , _tip{rhs._tip}
    , _delay{rhs._delay}
    , _position{rhs._position}
   {
   }

   tooltip_element::~tooltip_element()
   {
      if (_alive)
         *_alive = false;
   }

   bool tooltip_element::cursor(context const& ctx, point p, cursor_tracking status)
   {
      if (status != cursor_tracking::leaving)
      {
         if (_tip_status != tip_visible)
         {
            _tip_status = tip_delayed;
            auto side = _position;
            _tip->bounds(tip_bounds(ctx, side));

            // A tip's height can follow its width, as wrapped text does,
            // and is known only once it is laid out: lay it out where it
            // would go, then place it again.
            _tip->layout(context{ctx, _tip.get(), _tip->bounds()});
            _tip->bounds(tip_bounds(ctx, side));

            // A bubble points at the element, from the side the tip is on.
            if (auto b = find_subject<tooltip_bubble_base*>(_tip.get()))
            {
               auto const& e = ctx.bounds;
               auto cx = (e.left + e.right) / 2;
               auto cy = (e.top + e.bottom) / 2;
               switch (side)
               {
                  case tooltip_position::left:   b->anchor(side, {e.left, cy}); break;
                  case tooltip_position::top:    b->anchor(side, {cx, e.top}); break;
                  case tooltip_position::right:  b->anchor(side, {e.right, cy}); break;
                  case tooltip_position::bottom: b->anchor(side, {cx, e.bottom}); break;
               }
            }
            ctx.view.post(std::chrono::duration_cast<std::chrono::milliseconds>(_delay),
               [this, alive = _alive, &view_ = ctx.view, bounds = ctx.bounds]()
               {
                  if (!*alive)
                     return;
                  if (_tip_status == tip_delayed)
                  {
                     _tip->on_cursor =
                        [this, alive, tip = std::weak_ptr{_tip}, &view_, bounds]
                        (point p, cursor_tracking status)
                        {
                           if (!*alive)
                           {
                              // Its tooltip is gone: so is the tip.
                              if (auto t = tip.lock())
                                 t->close(view_);
                              return;
                           }
                           _cursor_in_tip = status != cursor_tracking::leaving;
                           if (!_cursor_in_tip && !bounds.includes(p))
                              close_tip(view_);
                        };
                     on_hover(true);
                     _tip_status = tip_visible;

                     // Opened without the focus: the element under the
                     // cursor may be the one being typed in.
                     view_.add(_tip, false);
                     view_.refresh();

                     // A bubble's shadow reaches past its bounds, which is
                     // all that opening the tip repaints.
                     if (find_subject<tooltip_bubble_base*>(_tip.get()))
                        view_.post(
                           [&view_, r = _tip->bounds().inset(-tooltip_bubble_base::shadow)]
                           { view_.refresh(r); }
                        );
                  }
               }
            );
         }
      }
      else
      {
         ctx.view.post(
            [this, alive = _alive, &view_ = ctx.view]()
            {
               if (*alive && !_cursor_in_tip)
                  close_tip(view_);
            }
         );
      }
      return base_type::cursor(ctx, p, status);
   }

   void tooltip_element::close_tip(view& view_)
   {
      if (_tip_status != tip_hidden)
      {
         if (_tip_status == tip_visible)
         {
            on_hover(false);
            _tip->close(view_);
         }
         _tip_status = tip_hidden;
         _cursor_in_tip = false;
      }
   }

   bool tooltip_element::key(context const& ctx, key_info k)
   {
      auto r = base_type::key(ctx, k);
      if (!r && k.key == key_code::escape)
         close_tip(ctx.view);
      return r;
   }

   std::string tooltip_element::class_name() const
   {
      return "tooltip";
   }
}
