/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/curve_editor.hpp>
#include <elements/view.hpp>
#include <algorithm>
#include <cmath>

namespace cycfi::elements
{
   namespace
   {
      basic_curve_editor::node_ptr make_node(curve_point p)
      {
         auto n = std::make_shared<basic_curve_editor::node>();
         n->spec = p;
         return n;
      }
   }

   basic_curve_editor::basic_curve_editor(
      std::vector<curve_point> points, element_ptr handle)
    : constrain(default_constraint)
    , _handle(std::move(handle))
   {
      _nodes.reserve(points.size());
      for (auto const& p : points)
         _nodes.push_back(make_node(p));
   }

   ////////////////////////////////////////////////////////////////////////////
   // The points
   ////////////////////////////////////////////////////////////////////////////
   point basic_curve_editor::position(std::size_t i) const
   {
      auto const& s = _nodes[i]->spec;
      if (s.offset_x && i > 0)
         return {position(i - 1).x + s.pos.x, s.pos.y};
      return s.pos;
   }

   basic_curve_editor::points_type basic_curve_editor::positions() const
   {
      points_type pts;
      pts.reserve(_nodes.size());
      auto x = 0.0f;
      for (auto const& n : _nodes)
      {
         auto const& s = n->spec;
         x = s.offset_x && !pts.empty()? x + s.pos.x : s.pos.x;
         pts.push_back({x, s.pos.y});
      }
      return pts;
   }

   float basic_curve_editor::x(std::size_t i) const
   {
      return _nodes[i]->spec.pos.x;
   }

   void basic_curve_editor::x(std::size_t i, float v)
   {
      _nodes[i]->spec.pos.x = v;
   }

   float basic_curve_editor::y(std::size_t i) const
   {
      return _nodes[i]->spec.pos.y;
   }

   void basic_curve_editor::y(std::size_t i, float v)
   {
      _nodes[i]->spec.pos.y = v;
   }

   // An offset point inserted splits the segment it lands in, and one
   // erased merges two, so the points after it stay where they were.
   bool basic_curve_editor::insert(std::size_t i, curve_point p)
   {
      if (i > _nodes.size())
         return false;
      if (p.offset_x && i > 0)
      {
         auto const at = position(i - 1).x + p.pos.x;
         if (i < _nodes.size() && _nodes[i]->spec.offset_x)
            _nodes[i]->spec.pos.x = position(i).x - at;
      }
      _nodes.insert(_nodes.begin() + i, make_node(p));
      return true;
   }

   bool basic_curve_editor::erase(std::size_t i)
   {
      if (i >= _nodes.size())
         return false;
      if (i + 1 < _nodes.size() && _nodes[i + 1]->spec.offset_x
         && _nodes[i]->spec.offset_x)
         _nodes[i + 1]->spec.pos.x += _nodes[i]->spec.pos.x;
      auto const* n = _nodes[i].get();
      if (_hot == n)
         _hot = nullptr;
      if (_dragging == n)
         _dragging = nullptr;
      if (_last == n)
         _last = nullptr;
      _nodes.erase(_nodes.begin() + i);
      return true;
   }

   basic_curve_editor::node_state
   basic_curve_editor::state(std::size_t i) const
   {
      auto const* n = _nodes[i].get();
      if (n == _dragging)
         return dragging;
      if (n == _hot)
         return hot;
      return normal;
   }

   ////////////////////////////////////////////////////////////////////////////
   // Between the unit square and the screen
   ////////////////////////////////////////////////////////////////////////////
   rect basic_curve_editor::plot(context const& ctx) const
   {
      return ctx.bounds.inset(inset, inset);
   }

   // With fit_width, the points' span is the plot's width
   float basic_curve_editor::x_scale() const
   {
      if (!fit_width)
         return 1.0f;
      auto span = 0.0f;
      for (auto const& p : positions())
         span = std::max(span, p.x);
      return span > 0.0f? 1.0f / span : 1.0f;
   }

   point basic_curve_editor::to_screen(point p, rect b) const
   {
      return {
         b.left + p.x * x_scale() * b.width()
       , b.bottom - p.y * b.height()
      };
   }

   point basic_curve_editor::from_screen(point p, rect b) const
   {
      return {
         (p.x - b.left) / (b.width() * x_scale())
       , (b.bottom - p.y) / b.height()
      };
   }

   // The nearest movable point within reach. Points that coincide, within
   // half a pixel, go to the one last dragged, else to the later one.
   int basic_curve_editor::pick(context const& ctx, point p) const
   {
      auto const b = plot(ctx);
      auto distance = [&](std::size_t i)
      {
         auto const c = to_screen(position(i), b);
         return std::hypot(p.x - c.x, p.y - c.y);
      };

      auto nearest = reach;
      for (std::size_t i = 0; i != _nodes.size(); ++i)
      {
         if (is_movable(i))
            nearest = std::min(nearest, distance(i));
      }

      int best = -1;
      for (std::size_t i = 0; i != _nodes.size(); ++i)
      {
         if (!is_movable(i))
            continue;
         if (distance(i) > reach || distance(i) - nearest > 0.5f)
            continue;
         if (_nodes[i].get() == _last)
            return int(i);
         best = int(i);
      }
      return best;
   }

   // Move a point toward `to`, as the constraint allows, and report what
   // changed. A drag is a gesture on each axis it moves the point along,
   // begun the first time that axis changes. The callbacks report where
   // the drag put the point, which a callback may set again meanwhile:
   // the axis callbacks as the point holds it, on_change in the square.
   void basic_curve_editor::move(std::size_t i, point to)
   {
      auto const pts = positions();
      auto const at = constrain? constrain(pts, i, to) : to;

      auto& n = *_nodes[i];
      auto const old = n.spec.pos;
      n.spec.pos.x = n.spec.offset_x && i > 0?
         at.x - pts[i - 1].x : at.x;
      n.spec.pos.y = at.y;
      auto const now = n.spec.pos;

      if (now.x != old.x)
      {
         if (!_x_gesture && n.on_x_gesture)
            n.on_x_gesture(true);
         _x_gesture = true;
         if (n.on_x_change)
            n.on_x_change(now.x);
      }
      if (now.y != old.y)
      {
         if (!_y_gesture && n.on_y_gesture)
            n.on_y_gesture(true);
         _y_gesture = true;
         if (n.on_y_change)
            n.on_y_change(now.y);
      }
      if ((now.x != old.x || now.y != old.y) && on_change)
         on_change(i, at);
   }

   void basic_curve_editor::end_gesture(std::size_t i)
   {
      auto const& n = *_nodes[i];
      if (_x_gesture && n.on_x_gesture)
         n.on_x_gesture(false);
      if (_y_gesture && n.on_y_gesture)
         n.on_y_gesture(false);
      _x_gesture = _y_gesture = false;
   }

   // A double click on a point erases it, on empty space inserts one,
   // where on_erase or on_insert allows it.
   bool basic_curve_editor::double_click(context const& ctx, point p)
   {
      auto const at = pick(ctx, p);
      if (at >= 0)
      {
         if (!on_erase || !on_erase(std::size_t(at)))
            return false;
         return erase(std::size_t(at));
      }

      if (!on_insert)
         return false;
      auto const u = from_screen(p, plot(ctx));
      std::size_t i = 0;
      while (i != _nodes.size() && position(i).x <= u.x)
         ++i;

      // An offset if the point after it is one, so the segment is split
      auto offset = i < _nodes.size() && i > 0
         && _nodes[i]->spec.offset_x;
      curve_point np;
      np.offset_x = offset;
      np.pos = {offset? u.x - position(i - 1).x : u.x, u.y};
      if (!on_insert(i, {u.x, u.y}))
         return false;
      return insert(i, np);
   }

   ////////////////////////////////////////////////////////////////////////////
   // The element
   ////////////////////////////////////////////////////////////////////////////
   element* basic_curve_editor::hit_test(
      context const& ctx, point p, bool /*leaf*/, bool /*control*/)
   {
      return ctx.bounds.includes(p)? this : nullptr;
   }

   // The curve, then a handle at each point
   void basic_curve_editor::draw(context const& ctx)
   {
      proxy_base::draw(ctx);
      if (!_handle)
         return;

      auto const b = plot(ctx);
      auto const r = reach;
      for (std::size_t i = 0; i != _nodes.size(); ++i)
      {
         auto const c = to_screen(position(i), b);
         context hctx{ctx, _handle.get()
          , {c.x - r, c.y - r, c.x + r, c.y + r}};
         _drawing = i;
         _handle->draw(hctx);
      }
   }

   bool basic_curve_editor::click(context const& ctx, mouse_button btn)
   {
      if (btn.down && btn.num_clicks == 2)
      {
         if (double_click(ctx, btn.pos))
            ctx.view.refresh(ctx);
         return true;
      }

      if (btn.down)
      {
         auto const at = pick(ctx, btn.pos);
         if (at >= 0)
         {
            _dragging = _nodes[at].get();
            _last = _dragging;
            _x_gesture = _y_gesture = false;
         }
      }
      else if (_dragging)
      {
         for (std::size_t i = 0; i != _nodes.size(); ++i)
         {
            if (_nodes[i].get() == _dragging)
               end_gesture(i);
         }
         _dragging = nullptr;
         ctx.view.refresh(ctx);
      }

      tracker<proxy_base>::click(ctx, btn);
      return true;
   }

   void basic_curve_editor::keep_tracking(
      context const& ctx, tracker_info& track)
   {
      if (!_dragging)
         return;
      for (std::size_t i = 0; i != _nodes.size(); ++i)
      {
         if (_nodes[i].get() == _dragging)
         {
            move(i, from_screen(track.current, plot(ctx)));
            ctx.view.refresh(ctx);
            return;
         }
      }
   }

   bool basic_curve_editor::cursor(
      context const& ctx, point p, cursor_tracking status)
   {
      auto const was = _hot;
      auto const at = (status == cursor_tracking::leaving)?
         -1 : pick(ctx, p);
      _hot = at >= 0? _nodes[at].get() : nullptr;
      if (_hot != was)
         ctx.view.refresh(ctx);
      return _hot != nullptr;
   }

   std::string basic_curve_editor::class_name() const
   {
      return "curve_editor";
   }

   ////////////////////////////////////////////////////////////////////////////
   // One axis of one point, for a model binder
   ////////////////////////////////////////////////////////////////////////////
   curve_value::curve_value(
      std::shared_ptr<basic_curve_editor> editor, node_ptr n, bool is_y)
    : on_change(is_y? n->on_y_change : n->on_x_change)
    , on_gesture(is_y? &n->on_y_gesture : &n->on_x_gesture)
    , _editor(editor)
    , _node(std::move(n))
    , _is_y(is_y)
   {}

   // Set on the point if it is still in the editor; an erased one is left
   // alone.
   void curve_value::value(float v)
   {
      auto e = _editor.lock();
      if (!e)
         return;
      for (std::size_t i = 0; i != e->size(); ++i)
      {
         if (e->node_at(i) == _node)
         {
            if (_is_y)
               e->y(i, v);
            else
               e->x(i, v);
            return;
         }
      }
   }

   element* curve_value::refresh_target() const
   {
      return _editor.lock().get();
   }
}
