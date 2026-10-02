/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_CURVE_EDITOR_OCTOBER_1_2026)
#define ELEMENTS_CURVE_EDITOR_OCTOBER_1_2026

#include <elements/element/proxy.hpp>
#include <elements/element/tracker.hpp>
#include <elements/element/traversal.hpp>
#include <algorithm>
#include <functional>
#include <memory>
#include <vector>

namespace cycfi::elements
{
   /**
    * \struct curve_point
    *
    * \brief
    *    One point of a curve_editor, in a unit square with y up.
    *
    *    A point with `offset_x` holds its x as an offset from the x of the
    *    point before it, so dragging it moves every point after it: an
    *    envelope's segments, held as durations. An offset may be negative;
    *    the default constraint keeps the points in order.
    */
   struct curve_point
   {
      point          pos;
      bool           offset_x = false;
   };

   /**
    * \class basic_curve_editor
    *
    * \brief
    *    Points the user drags to shape a curve: an envelope, an EQ band, a
    *    transfer curve, an automation lane, an XY pad.
    *
    *    The proxy's subject draws the curve, the line styler, and a handle
    *    styler draws each point. Both find the editor with find_parent; the
    *    handle styler gets the point it is drawing from drawing(), and that
    *    point's state from state().
    *
    *    Where a point may go is the client's rule: `constrain` is given the
    *    points, which one is being dragged and where to, and returns where
    *    it lands. All of these are places in the unit square, offsets
    *    summed; the editor turns the result back into an offset for an
    *    offset point. The default keeps a point in the unit square and
    *    between its neighbors. `movable` says which points may be dragged.
    *
    *    Each point is a node with its own callbacks, one per axis, which
    *    x_of and y_of hand out as controls of their own, for a model binder.
    *    on_change gives the point the user moved and its new value, for
    *    code that binds nothing. Both give x as the point holds it: an
    *    offset for an offset point. Points may be inserted and erased, by
    *    code or, where on_insert and on_erase allow it, by a double click,
    *    on_insert given the place in the unit square.
    */
   class basic_curve_editor : public tracker<proxy_base>
   {
   public:

      using points_type = std::vector<point>;
      using change_function = std::function<void(float)>;
      using gesture_function = std::function<void(bool begin)>;
      using point_function = std::function<void(std::size_t i, point pos)>;
      using insert_function = std::function<bool(std::size_t i, point pos)>;
      using erase_function = std::function<bool(std::size_t i)>;
      using constrain_function = std::function<
                                 point(points_type const& pts
                                  , std::size_t i, point to)>;
      using movable_function = std::function<bool(std::size_t i)>;

      enum node_state { normal, hot, dragging };

      struct node
      {
         curve_point       spec;
         change_function   on_x_change;
         change_function   on_y_change;
         gesture_function  on_x_gesture;
         gesture_function  on_y_gesture;
      };

      using node_ptr = std::shared_ptr<node>;

                              basic_curve_editor(
                                 std::vector<curve_point> points
                               , element_ptr handle);

      // The points, in order
      std::size_t             size() const   { return _nodes.size(); }
      node&                   operator[](std::size_t i);
      node const&             operator[](std::size_t i) const;
      node_ptr                node_at(std::size_t i) const;

      // A point's place in the unit square, offsets summed
      point                   position(std::size_t i) const;
      points_type             positions() const;

      // An axis as the point holds it: an offset x is the offset. Set
      // as given; no callback is called.
      float                   x(std::size_t i) const;
      void                    x(std::size_t i, float v);
      float                   y(std::size_t i) const;
      void                    y(std::size_t i, float v);

      bool                    insert(std::size_t i, curve_point p);
      bool                    erase(std::size_t i);
      bool                    is_movable(std::size_t i) const;

      // For the stylers: the point being drawn, and a point's state
      std::size_t             drawing() const   { return _drawing; }
      node_state              state(std::size_t i) const;

      // Where the unit square is drawn: the bounds, inset so a handle at
      // an edge is drawn whole.
      rect                    plot(context const& ctx) const;
      point                   to_screen(point p, rect plot) const;
      point                   from_screen(point p, rect plot) const;

      constrain_function      constrain;
      movable_function        movable;
      point_function          on_change;
      insert_function         on_insert;
      erase_function          on_erase;

      float                   reach = 12.0f;    // to take hold of a point
      float                   inset = 8.0f;

      // Spread the points across the plot's width: x scaled so the
      // farthest point reaches the right edge, for a preview of a curve
      // whose span varies.
      bool                    fit_width = false;

      element*                hit_test(
                                 context const& ctx, point p
                               , bool leaf, bool control) override;
      void                    draw(context const& ctx) override;
      bool                    click(
                                 context const& ctx
                               , mouse_button btn) override;
      void                    keep_tracking(
                                 context const& ctx
                               , tracker_info& track) override;
      bool                    cursor(
                                 context const& ctx, point p
                               , cursor_tracking status) override;
      bool                    wants_control() const override { return true; }
      std::string             class_name() const override;

   private:

      int                     pick(context const& ctx, point p) const;
      float                   x_scale() const;
      void                    move(std::size_t i, point to);
      void                    end_gesture(std::size_t i);
      bool                    double_click(context const& ctx, point p);

      std::vector<node_ptr>   _nodes;
      element_ptr             _handle;
      node const*             _hot = nullptr;
      node const*             _dragging = nullptr;
      node const*             _last = nullptr;
      bool                    _x_gesture = false;
      bool                    _y_gesture = false;
      std::size_t             _drawing = 0;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Pieces for a constrain function
   ////////////////////////////////////////////////////////////////////////////

   // Within the unit square
   inline point in_unit_square(point p)
   {
      return {std::clamp(p.x, 0.0f, 1.0f), std::clamp(p.y, 0.0f, 1.0f)};
   }

   // No further left than the point before, nor right than the one after
   inline point between_neighbors(
      basic_curve_editor::points_type const& pts, std::size_t i, point p)
   {
      auto lo = i > 0? pts[i - 1].x : 0.0f;
      auto hi = i + 1 < pts.size()? pts[i + 1].x : 1.0f;
      p.x = std::clamp(p.x, lo, std::max(lo, hi));
      return p;
   }

   // The default: in the unit square, between the neighbors
   inline point default_constraint(
      basic_curve_editor::points_type const& pts, std::size_t i, point to)
   {
      return between_neighbors(pts, i, in_unit_square(to));
   }

   /**
    * \brief
    *    Make a curve editor: its points, the element that draws the curve
    *    and the element that draws each point. See curve_lines and
    *    curve_handle (elements/element/style/curve_editor.hpp).
    */
   template <concepts::Element LineStyler, concepts::Element HandleStyler>
   inline proxy<remove_cvref_t<LineStyler>, basic_curve_editor>
   curve_editor(
      std::vector<curve_point> points
    , LineStyler&& lines
    , HandleStyler&& handle)
   {
      return {
         std::forward<LineStyler>(lines)
       , std::move(points)
       , share(std::forward<HandleStyler>(handle))
      };
   }

   /**
    * \class curve_value
    *
    * \brief
    *    One axis of one point of a curve editor, as a control of its own
    *    for a model binder: a value(v) setter, an on_change and a gesture
    *    callback, the point's own. It holds its node, so a point erased
    *    from the editor leaves it harmless rather than dangling.
    */
   class curve_value
   {
   public:

      using node_ptr = basic_curve_editor::node_ptr;
      using change_function = basic_curve_editor::change_function;
      using gesture_function = basic_curve_editor::gesture_function;

                              curve_value(
                                 std::shared_ptr<basic_curve_editor> editor
                               , node_ptr n, bool is_y);

      void                    value(float v);
      element*                refresh_target() const;

      change_function&        on_change;
      gesture_function*       on_gesture;

   private:

      std::weak_ptr<basic_curve_editor> _editor;
      node_ptr                _node;
      bool                    _is_y;
   };

   template <typename Editor>
   inline std::shared_ptr<curve_value>
   x_of(std::shared_ptr<Editor> editor, std::size_t i)
   {
      auto n = editor->node_at(i);
      return std::make_shared<curve_value>(editor, n, false);
   }

   template <typename Editor>
   inline std::shared_ptr<curve_value>
   y_of(std::shared_ptr<Editor> editor, std::size_t i)
   {
      auto n = editor->node_at(i);
      return std::make_shared<curve_value>(editor, n, true);
   }

   ////////////////////////////////////////////////////////////////////////////
   // Inline implementation
   ////////////////////////////////////////////////////////////////////////////
   inline basic_curve_editor::node&
   basic_curve_editor::operator[](std::size_t i)
   {
      return *_nodes[i];
   }

   inline basic_curve_editor::node const&
   basic_curve_editor::operator[](std::size_t i) const
   {
      return *_nodes[i];
   }

   inline basic_curve_editor::node_ptr
   basic_curve_editor::node_at(std::size_t i) const
   {
      return _nodes[i];
   }

   inline bool basic_curve_editor::is_movable(std::size_t i) const
   {
      return !movable || movable(i);
   }
}

#endif
