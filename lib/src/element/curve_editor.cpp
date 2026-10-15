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
    : _handle(std::move(handle))
    , _anchor(this)
   {
      _nodes.reserve(points.size());
      for (auto const& p : points)
         _nodes.push_back(make_node(p));
   }

   // A copy is an editor of its own: its points are new nodes, as the
   // original's were, with no undo steps and no axes handed out.
   basic_curve_editor::basic_curve_editor(basic_curve_editor const& rhs)
    : tracker<proxy_base>(rhs)
    , constrain(rhs.constrain)
    , movable(rhs.movable)
    , on_change(rhs.on_change)
    , on_insert(rhs.on_insert)
    , on_erase(rhs.on_erase)
    , reach(rhs.reach)
    , inset(rhs.inset)
    , fit_width(rhs.fit_width)
    , _handle(rhs._handle)
    , _anchor(rhs._anchor)
   {
      _nodes.reserve(rhs._nodes.size());
      for (auto const& n : rhs._nodes)
         _nodes.push_back(std::make_shared<node>(*n));
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

   // A point inserted or erased before an offset point leaves that point,
   // and so the points after it, where they were: its offset is taken
   // again from the point now before it.
   bool basic_curve_editor::insert(std::size_t i, curve_point p)
   {
      return insert_node(i, make_node(p));
   }

   bool basic_curve_editor::insert_node(std::size_t i, node_ptr n)
   {
      if (i > _nodes.size())
         return false;
      if (i < _nodes.size() && _nodes[i]->spec.offset_x)
      {
         auto const& p = n->spec;
         auto const at = p.offset_x && i > 0?
            position(i - 1).x + p.pos.x : p.pos.x;
         _nodes[i]->spec.pos.x = position(i).x - at;
      }
      _nodes.insert(_nodes.begin() + i, std::move(n));
      return true;
   }

   int basic_curve_editor::index_of(node_ptr const& n) const
   {
      auto i = std::find(_nodes.begin(), _nodes.end(), n);
      return i == _nodes.end()? -1 : int(i - _nodes.begin());
   }

   bool basic_curve_editor::erase(std::size_t i)
   {
      if (i >= _nodes.size())
         return false;
      if (i + 1 < _nodes.size() && _nodes[i + 1]->spec.offset_x)
      {
         auto const next = position(i + 1).x;
         _nodes[i + 1]->spec.pos.x = i > 0? next - position(i - 1).x : next;
      }
      auto const* n = _nodes[i].get();
      std::erase_if(_values,
         [n](auto const& v) { return v->_node.get() == n; });
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
      auto const at = constrain? constrain(pts, i, to) : default_place(pts, i, to);

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

   // In the unit square, between the neighbors. A point followed by an
   // offset point is bounded only by the point before it: the points after
   // it move with it.
   point basic_curve_editor::default_place(
      points_type const& pts, std::size_t i, point to) const
   {
      if (i + 1 < _nodes.size() && _nodes[i + 1]->spec.offset_x)
      {
         to = in_unit_square(to);
         to.x = std::max(to.x, i > 0? pts[i - 1].x : 0.0f);
         return to;
      }
      return default_constraint(pts, i, to);
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
         auto const i = std::size_t(at);
         auto const n = _nodes[i];
         auto const place = position(i);
         auto values = std::make_shared<values_type>();
         take_out(n, values);
         add_point_undo(ctx, false, i, n, place, values);
         return true;
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

      auto const n = make_node(np);
      auto const next = i < _nodes.size()? _nodes[i] : nullptr;
      auto const old = next? next->spec.pos : point{};
      insert_node(i, n);
      if (next)
         report(*next, old);
      add_point_undo(ctx, true, i, n, {u.x, u.y}
       , std::make_shared<values_type>());
      return true;
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

   ////////////////////////////////////////////////////////////////////////////
   // Undo and redo
   ////////////////////////////////////////////////////////////////////////////

   // Report a node's held values changed from `old`, as a drag does: each
   // axis that changed, between its gesture's begin and end.
   void basic_curve_editor::report(node const& n, point old) const
   {
      auto axis = [](auto const& gesture, auto const& change, float v)
      {
         if (gesture)
            gesture(true);
         if (change)
            change(v);
         if (gesture)
            gesture(false);
      };
      auto const now = n.spec.pos;
      if (now.x != old.x)
         axis(n.on_x_gesture, n.on_x_change, now.x);
      if (now.y != old.y)
         axis(n.on_y_gesture, n.on_y_change, now.y);
   }

   // Put a dragged point back as it held itself before or after the drag,
   // reported as a drag is. false if the point has been erased since.
   bool basic_curve_editor::restore(node_ptr const& n, point pos)
   {
      auto const at = index_of(n);
      if (at < 0)
         return false;
      auto const i = std::size_t(at);
      auto const old = n->spec.pos;
      auto const old_place = position(i);
      n->spec.pos = pos;
      report(*n, old);
      auto const place = position(i);
      if ((place.x != old_place.x || place.y != old_place.y) && on_change)
         on_change(i, place);
      return true;
   }

   // Insert a node erased before, with the axes handed out for it, if it
   // is not in the editor and on_insert allows it. The point after it,
   // an offset point, takes its offset from it again.
   bool basic_curve_editor::put_back(
      std::size_t i, node_ptr const& n, point place, values_ptr const& values)
   {
      i = std::min(i, _nodes.size());
      if (index_of(n) >= 0 || !on_insert || !on_insert(i, place))
         return false;
      auto const next = i < _nodes.size()? _nodes[i] : nullptr;
      auto const old = next? next->spec.pos : point{};
      insert_node(i, n);
      _values.insert(_values.end(), values->begin(), values->end());
      values->clear();
      if (next)
         report(*next, old);
      return true;
   }

   // Erase a node, keeping the axes handed out for it. The point after it,
   // an offset point, takes its offset from the point before.
   bool basic_curve_editor::take_out(
      node_ptr const& n, values_ptr const& values)
   {
      auto const at = index_of(n);
      if (at < 0)
         return false;
      auto const i = std::size_t(at);
      for (auto const& v : _values)
      {
         if (v->_node == n)
            values->push_back(v);
      }
      auto const next = i + 1 < _nodes.size()? _nodes[i + 1] : nullptr;
      auto const old = next? next->spec.pos : point{};
      erase(i);
      if (next)
         report(*next, old);
      return true;
   }

   void basic_curve_editor::add_drag_undo(
      context const& ctx, node_ptr n, point before, point after)
   {
      auto& view_ = ctx.view;
      auto step = [&view_, self = _anchor.ref(), n](point pos)
      {
         auto* e = *self;
         if (!e || !e->restore(n, pos))
            return false;
         view_.refresh(*e);
         return true;
      };
      view_.add_undo({
         [step, before]() { return step(before); }
       , [step, after]() { return step(after); }
      });
   }

   void basic_curve_editor::add_point_undo(
      context const& ctx, bool inserted, std::size_t i, node_ptr n
    , point place, values_ptr values)
   {
      auto& view_ = ctx.view;
      // Stale, the editor gone or the point already where the step would
      // put it: false. Refused by the client: true, the press used up.
      auto in = [&view_, self = _anchor.ref(), i, n, place, values]()
      {
         auto* e = *self;
         if (!e || e->index_of(n) >= 0)
            return false;
         if (e->put_back(i, n, place, values))
            view_.refresh(*e);
         return true;
      };
      auto out = [&view_, self = _anchor.ref(), n, values]()
      {
         auto* e = *self;
         if (!e)
            return false;
         auto const at = e->index_of(n);
         if (at < 0)
            return false;
         if (e->on_erase && e->on_erase(std::size_t(at)) && e->take_out(n, values))
            view_.refresh(*e);
         return true;
      };
      if (inserted)
         view_.add_undo({out, in});
      else
         view_.add_undo({in, out});
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
            _drag_start = _nodes[at]->spec.pos;
         }
      }
      else if (_dragging)
      {
         for (std::size_t i = 0; i != _nodes.size(); ++i)
         {
            if (_nodes[i].get() == _dragging)
            {
               end_gesture(i);
               auto const now = _nodes[i]->spec.pos;
               if (now.x != _drag_start.x || now.y != _drag_start.y)
                  add_drag_undo(ctx, _nodes[i], _drag_start, now);
            }
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

   std::shared_ptr<curve_value> basic_curve_editor::value_of(
      std::shared_ptr<basic_curve_editor> editor, std::size_t i, bool is_y)
   {
      auto n = editor->node_at(i);
      for (auto const& v : editor->_values)
      {
         if (v->_node == n && v->_is_y == is_y)
            return v;
      }
      auto v = std::make_shared<curve_value>(editor, n, is_y);
      editor->_values.push_back(v);
      return v;
   }

   std::shared_ptr<curve_value>
   x_of(std::shared_ptr<basic_curve_editor> editor, std::size_t i)
   {
      return basic_curve_editor::value_of(std::move(editor), i, false);
   }

   std::shared_ptr<curve_value>
   y_of(std::shared_ptr<basic_curve_editor> editor, std::size_t i)
   {
      return basic_curve_editor::value_of(std::move(editor), i, true);
   }
}
