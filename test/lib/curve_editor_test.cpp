/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   A curve editor driven the way a user drives it: presses, drags and
   releases on its points, placed through the editor's own mapping from
   its unit square to the screen, as laid out.
=============================================================================*/
#include "test_support.hpp"

#include <cmath>
#include <string>
#include <vector>

namespace cycfi::elements::test
{
   namespace
   {
      using cp = curve_point;
      using pts_type = basic_curve_editor::points_type;

      struct editor_view : test_view
      {
         explicit editor_view(std::vector<curve_point> points)
          : test_view{{216, 116}}
          , editor{share(curve_editor(std::move(points)))}
         {
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
            headless::open(view_);
#endif
            view_.content(hold(editor));
            view_.layout();
            draw();

            // Where the editor drew its unit square, as laid out
            view_.in_context_do(*editor, [this](context const& ctx)
            {
               square = editor->plot(ctx);
            });
         }

         point screen(point u) const
         {
            return editor->to_screen(u, square);
         }

         static mouse_button press(point p, bool down, int clicks = 1)
         {
            mouse_button btn{};
            btn.down = down;
            btn.state = mouse_button::left;
            btn.num_clicks = clicks;
            btn.pos = p;
            return btn;
         }

         // Press on a point of the unit square, drag to another, release
         void drag(point from, point to)
         {
            view_.click(press(screen(from), true));
            view_.drag(press(screen(to), true));
            view_.click(press(screen(to), false));
            draw();
         }

         void double_click(point at)
         {
            view_.click(press(screen(at), true));
            view_.click(press(screen(at), false));
            view_.click(press(screen(at), true, 2));
            view_.click(press(screen(at), false, 2));
            draw();
         }

         using editor_type =
            decltype(curve_editor(std::vector<curve_point>{}));
         std::shared_ptr<editor_type> editor;
         rect square;
      };

      constexpr float eps = 1e-3f;
   }
}

using namespace cycfi::elements;
using namespace cycfi::elements::test;

TEST_CASE("curve_editor: a free point follows the drag, in the unit square")
{
   editor_view v{{cp{{0.5f, 0.5f}}}};
   v.drag({0.5f, 0.5f}, {0.75f, 0.25f});
   CHECK(v.editor->x(0) == Approx(0.75f).margin(eps));
   CHECK(v.editor->y(0) == Approx(0.25f).margin(eps));

   v.drag({0.75f, 0.25f}, {1.5f, -0.5f});
   CHECK(v.editor->x(0) == Approx(1.0f).margin(eps));
   CHECK(v.editor->y(0) == Approx(0.0f).margin(eps));
}

TEST_CASE("curve_editor: the constraint says where a point lands")
{
   editor_view v{{cp{{0.25f, 0.5f}}, cp{{0.75f, 0.5f}}}};

   // Point 0 keeps its height, point 1 its place
   v.editor->constrain =
      [](pts_type const& pts, std::size_t i, point to)
      {
         to = default_constraint(pts, i, to);
         if (i == 0)
            to.y = pts[0].y;
         else
            to.x = pts[1].x;
         return to;
      };

   v.drag({0.25f, 0.5f}, {0.4f, 0.9f});
   CHECK(v.editor->x(0) == Approx(0.4f).margin(eps));
   CHECK(v.editor->y(0) == Approx(0.5f).margin(eps));

   v.drag({0.75f, 0.5f}, {0.6f, 0.9f});
   CHECK(v.editor->x(1) == Approx(0.75f).margin(eps));
   CHECK(v.editor->y(1) == Approx(0.9f).margin(eps));
}

TEST_CASE("curve_editor: by default a point stays between its neighbors")
{
   editor_view v{{
      cp{{0.2f, 0.5f}}, cp{{0.5f, 0.5f}}, cp{{0.8f, 0.5f}}
   }};
   v.drag({0.5f, 0.5f}, {0.95f, 0.5f});
   CHECK(v.editor->x(1) == Approx(0.8f).margin(eps));
   v.drag({0.8f, 0.5f}, {0.1f, 0.5f});
   CHECK(v.editor->x(1) == Approx(0.2f).margin(eps));
}

TEST_CASE("curve_editor: an offset point moves the points after it")
{
   cp start{{0.0f, 0.0f}};
   cp a{{0.2f, 1.0f}, true};
   cp b{{0.3f, 0.5f}, true};
   editor_view v{{start, a, b}};
   CHECK(v.editor->position(2).x == Approx(0.5f).margin(eps));

   // Segments no longer than 0.5; the constraint sees absolute places
   v.editor->constrain =
      [](pts_type const& pts, std::size_t i, point to)
      {
         to = in_unit_square(to);
         if (i > 0)
            to.x = std::min(to.x, pts[i - 1].x + 0.5f);
         return to;
      };

   v.drag({0.2f, 1.0f}, {0.4f, 1.0f});
   CHECK(v.editor->x(1) == Approx(0.4f).margin(eps));     // a distance
   CHECK(v.editor->x(2) == Approx(0.3f).margin(eps));     // unchanged
   CHECK(v.editor->position(2).x == Approx(0.7f).margin(eps));

   v.drag({0.7f, 0.5f}, {1.0f, 0.5f});
   CHECK(v.editor->x(2) == Approx(0.5f).margin(eps));
}

TEST_CASE("curve_editor: a point that is not movable is not taken hold of")
{
   int gestures = 0;
   editor_view v{{cp{{0.5f, 0.5f}}}};
   v.editor->movable = [](std::size_t) { return false; };
   (*v.editor)[0].on_x_gesture = [&](bool) { ++gestures; };
   (*v.editor)[0].on_y_gesture = [&](bool) { ++gestures; };
   v.drag({0.5f, 0.5f}, {0.9f, 0.9f});
   CHECK(gestures == 0);
   CHECK(v.editor->x(0) == Approx(0.5f).margin(eps));
}

TEST_CASE("curve_editor: a drag is a gesture on each axis it moves along")
{
   std::vector<std::string> log;
   editor_view v{{cp{{0.25f, 0.5f}}, cp{{0.75f, 0.5f}}}};
   v.editor->constrain =
      [](pts_type const& pts, std::size_t i, point to)
      {
         to = default_constraint(pts, i, to);
         if (i == 1)
            to.y = pts[1].y;
         return to;
      };
   (*v.editor)[0].on_x_gesture =
      [&](bool b) { log.push_back(b? "x0 begin" : "x0 end"); };
   (*v.editor)[0].on_y_gesture =
      [&](bool b) { log.push_back(b? "y0 begin" : "y0 end"); };
   (*v.editor)[1].on_x_gesture =
      [&](bool b) { log.push_back(b? "x1 begin" : "x1 end"); };
   (*v.editor)[1].on_y_gesture =
      [&](bool b) { log.push_back(b? "y1 begin" : "y1 end"); };

   v.drag({0.25f, 0.5f}, {0.3f, 0.6f});
   v.drag({0.75f, 0.5f}, {0.8f, 0.6f});
   std::vector<std::string> const expect =
   {
      "x0 begin", "y0 begin", "x0 end", "y0 end", "x1 begin", "x1 end"
   };
   CHECK(log == expect);
}

TEST_CASE("curve_editor: the callbacks say what moved")
{
   float x = -1, y = -1;
   std::size_t which = 99;
   editor_view v{{cp{{0.25f, 0.5f}}, cp{{0.75f, 0.5f}}}};
   (*v.editor)[1].on_x_change = [&](float val) { x = val; };
   (*v.editor)[1].on_y_change = [&](float val) { y = val; };
   v.editor->on_change = [&](std::size_t i, point) { which = i; };

   v.drag({0.75f, 0.5f}, {0.6f, 0.2f});
   CHECK(which == 1);
   CHECK(x == Approx(0.6f).margin(eps));
   CHECK(y == Approx(0.2f).margin(eps));
}

TEST_CASE("curve_editor: points that coincide go to the one last dragged")
{
   editor_view v{{cp{{0.3f, 0.5f}}, cp{{0.6f, 0.5f}}}};

   // Point 0 dragged onto point 1, which stops it at point 1's x
   v.drag({0.3f, 0.5f}, {0.6f, 0.5f});
   REQUIRE(v.editor->x(0) == Approx(0.6f).margin(eps));

   // Point 0 was the last dragged, so it is the one taken again
   v.drag({0.6f, 0.5f}, {0.6f, 0.9f});
   CHECK(v.editor->y(0) == Approx(0.9f).margin(eps));
   CHECK(v.editor->y(1) == Approx(0.5f).margin(eps));
}

TEST_CASE("curve_editor: a double click inserts or erases, where allowed")
{
   editor_view v{{cp{{0.2f, 0.5f}}, cp{{0.8f, 0.5f}}}};

   // Refused without the callbacks
   v.double_click({0.5f, 0.3f});
   CHECK(v.editor->size() == 2);

   v.editor->on_insert = [](std::size_t, point) { return true; };
   v.editor->on_erase = [](std::size_t) { return true; };

   v.double_click({0.5f, 0.3f});
   REQUIRE(v.editor->size() == 3);
   CHECK(v.editor->position(1).x == Approx(0.5f).margin(eps));
   CHECK(v.editor->position(1).y == Approx(0.3f).margin(eps));

   v.double_click({0.2f, 0.5f});
   REQUIRE(v.editor->size() == 2);
   CHECK(v.editor->position(0).x == Approx(0.5f).margin(eps));
}

TEST_CASE("curve_editor: inserting and erasing keeps offset points in place")
{
   cp start{{0.0f, 0.0f}};
   cp a{{0.4f, 1.0f}, true};
   cp b{{0.4f, 0.0f}, true};
   editor_view v{{start, a, b}};
   REQUIRE(v.editor->position(2).x == Approx(0.8f).margin(eps));

   // An offset point inserted at 0.2 splits the first segment
   cp mid{{0.2f, 0.5f}, true};
   v.editor->insert(1, mid);
   CHECK(v.editor->position(1).x == Approx(0.2f).margin(eps));
   CHECK(v.editor->position(2).x == Approx(0.4f).margin(eps));
   CHECK(v.editor->position(3).x == Approx(0.8f).margin(eps));

   // and erased, the segments merge again
   v.editor->erase(1);
   CHECK(v.editor->position(1).x == Approx(0.4f).margin(eps));
   CHECK(v.editor->position(2).x == Approx(0.8f).margin(eps));
}

TEST_CASE("curve_editor: a point that is not an offset, inserted or erased, keeps offset points in place")
{
   cp start{{0.0f, 0.0f}};
   cp a{{0.4f, 1.0f}, true};
   cp b{{0.4f, 0.0f}, true};
   editor_view v{{start, a, b}};

   // An absolute point inserted at 0.2, before the offset point at 0.4
   v.editor->insert(1, cp{{0.2f, 0.5f}});
   CHECK(v.editor->position(1).x == Approx(0.2f).margin(eps));
   CHECK(v.editor->position(2).x == Approx(0.4f).margin(eps));
   CHECK(v.editor->position(3).x == Approx(0.8f).margin(eps));

   // and erased
   v.editor->erase(1);
   CHECK(v.editor->position(1).x == Approx(0.4f).margin(eps));
   CHECK(v.editor->position(2).x == Approx(0.8f).margin(eps));

   // The first point erased: the offset point after it keeps its place
   v.editor->erase(0);
   CHECK(v.editor->position(0).x == Approx(0.4f).margin(eps));
   CHECK(v.editor->position(1).x == Approx(0.8f).margin(eps));
}

TEST_CASE("curve_editor: by default, a point followed by an offset point is not held back by it")
{
   cp start{{0.0f, 0.0f}};
   cp a{{0.2f, 1.0f}, true};
   cp b{{0.3f, 0.5f}, true};
   editor_view v{{start, a, b}};

   // Past the next point's place: the next point moves with it
   v.drag({0.2f, 1.0f}, {0.6f, 1.0f});
   CHECK(v.editor->position(1).x == Approx(0.6f).margin(eps));
   CHECK(v.editor->position(2).x == Approx(0.9f).margin(eps));

   // and no further left than the point before
   v.drag({0.6f, 1.0f}, {-0.2f, 1.0f});
   CHECK(v.editor->position(1).x == Approx(0.0f).margin(eps));
}

namespace
{
   // The whole state the user edits: each point's node and how it holds
   // itself, and its place in the square
   struct editor_state
   {
      std::vector<basic_curve_editor::node_ptr> nodes;
      std::vector<point> held;
      pts_type at;
   };

   template <typename Editor>
   editor_state state_of(Editor const& e)
   {
      editor_state st;
      for (std::size_t i = 0; i != e.size(); ++i)
      {
         st.nodes.push_back(e.node_at(i));
         st.held.push_back({e.x(i), e.y(i)});
      }
      st.at = e.positions();
      return st;
   }

   bool same(editor_state const& a, editor_state const& b)
   {
      if (a.nodes != b.nodes || a.held.size() != b.held.size())
         return false;
      for (std::size_t i = 0; i != a.held.size(); ++i)
      {
         if (std::abs(a.held[i].x - b.held[i].x) > eps
            || std::abs(a.held[i].y - b.held[i].y) > eps
            || std::abs(a.at[i].x - b.at[i].x) > eps
            || std::abs(a.at[i].y - b.at[i].y) > eps)
            return false;
      }
      return true;
   }

   // An envelope: a fixed start and three offset points at 0.3, 0.6, 0.9
   std::vector<curve_point> undo_envelope()
   {
      return {
         cp{{0.0f, 0.0f}}
       , cp{{0.3f, 1.0f}, true}
       , cp{{0.3f, 0.5f}, true}
       , cp{{0.3f, 0.0f}, true}
      };
   }

   // The callbacks of every point and of the editor, in the order called.
   // A point's callbacks are labelled with its index when attached: they
   // name the node, wherever it moves.
   struct recorder
   {
      std::vector<std::string> log;

      template <typename Editor>
      void attach(Editor& e)
      {
         for (std::size_t i = 0; i != e.size(); ++i)
            attach_node(e[i], std::to_string(i));
         e.on_change = [this](std::size_t i, point)
            { log.push_back("change " + std::to_string(i)); };
         e.on_insert = [this](std::size_t i, point)
            { log.push_back("insert " + std::to_string(i)); return true; };
         e.on_erase = [this](std::size_t i)
            { log.push_back("erase " + std::to_string(i)); return true; };
      }

      void attach_node(basic_curve_editor::node& n, std::string id)
      {
         n.on_x_gesture = [this, id](bool b) { log.push_back(id + (b? " x(" : " x)")); };
         n.on_y_gesture = [this, id](bool b) { log.push_back(id + (b? " y(" : " y)")); };
         n.on_x_change = [this, id](float) { log.push_back(id + " x"); };
         n.on_y_change = [this, id](float) { log.push_back(id + " y"); };
      }
   };

   using log_type = std::vector<std::string>;
}

TEST_CASE("curve_editor undo: a drag, undone and redone, reported as a drag is")
{
   editor_view v{undo_envelope()};
   recorder r;
   r.attach(*v.editor);
   auto const start = state_of(*v.editor);

   // Point 2 up only: its x is untouched
   v.drag({0.6f, 0.5f}, {0.6f, 0.8f});
   auto const end = state_of(*v.editor);
   REQUIRE(v.view_.has_undo());

   r.log.clear();
   CHECK(v.view_.undo());
   CHECK(same(state_of(*v.editor), start));
   CHECK(r.log == log_type{"2 y(", "2 y", "2 y)", "change 2"});

   r.log.clear();
   CHECK(v.view_.redo());
   CHECK(same(state_of(*v.editor), end));
   CHECK(r.log == log_type{"2 y(", "2 y", "2 y)", "change 2"});
}

TEST_CASE("curve_editor undo: an offset point's drag, undone, puts the points after it back")
{
   editor_view v{undo_envelope()};
   auto const start = state_of(*v.editor);

   v.drag({0.3f, 1.0f}, {0.4f, 1.0f});
   REQUIRE(v.editor->position(3).x == Approx(1.0f).margin(eps));
   CHECK(v.view_.undo());
   CHECK(same(state_of(*v.editor), start));
   CHECK(v.editor->position(3).x == Approx(0.9f).margin(eps));
}

TEST_CASE("curve_editor undo: an erase, undone, brings back the same point")
{
   editor_view v{undo_envelope()};
   recorder r;
   r.attach(*v.editor);
   auto const start = state_of(*v.editor);
   std::weak_ptr<curve_value> axis = y_of(v.editor, 2);

   r.log.clear();
   v.double_click({0.6f, 0.5f});
   REQUIRE(v.editor->size() == 3);
   auto const end = state_of(*v.editor);
   CHECK(v.editor->position(2).x == Approx(0.9f).margin(eps));   // in place

   // The erase itself reports the merged segment, which the point first
   // at 3 now ends
   CHECK(r.log == log_type{"erase 2", "3 x(", "3 x", "3 x)"});

   // Point 3's offset is taken again from the point now before it: its
   // segment's length is reported, though its place in the square is not
   r.log.clear();
   CHECK(v.view_.undo());
   CHECK(same(state_of(*v.editor), start));
   CHECK(r.log == log_type{"insert 2", "3 x(", "3 x", "3 x)"});

   // The point's callbacks and its axis came back with it
   REQUIRE(!axis.expired());
   axis.lock()->value(0.25f);
   CHECK(v.editor->y(2) == Approx(0.25f).margin(eps));
   axis.lock()->value(0.5f);

   r.log.clear();
   CHECK(v.view_.redo());
   CHECK(same(state_of(*v.editor), end));
   CHECK(r.log == log_type{"erase 2", "3 x(", "3 x", "3 x)"});
}

TEST_CASE("curve_editor undo: an insert, undone and redone")
{
   editor_view v{undo_envelope()};
   recorder r;
   r.attach(*v.editor);
   auto const start = state_of(*v.editor);

   // Splitting the segment from 0.3 to 0.6, which point 2 ends: its
   // segment is shorter now
   r.log.clear();
   v.double_click({0.45f, 0.2f});
   CHECK(r.log == log_type{"insert 2", "2 x(", "2 x", "2 x)"});
   REQUIRE(v.editor->size() == 5);
   auto const end = state_of(*v.editor);
   CHECK(v.editor->position(2).x == Approx(0.45f).margin(eps));
   CHECK(v.editor->position(3).x == Approx(0.6f).margin(eps));

   r.log.clear();
   CHECK(v.view_.undo());
   CHECK(same(state_of(*v.editor), start));
   CHECK(r.log == log_type{"erase 2", "2 x(", "2 x", "2 x)"});

   r.log.clear();
   CHECK(v.view_.redo());
   CHECK(same(state_of(*v.editor), end));
   CHECK(r.log == log_type{"insert 2", "2 x(", "2 x", "2 x)"});
}

TEST_CASE("curve_editor undo: a run of edits, undone to the start and redone to the end")
{
   editor_view v{undo_envelope()};
   recorder r;
   r.attach(*v.editor);

   std::vector<editor_state> states{state_of(*v.editor)};
   v.drag({0.3f, 1.0f}, {0.2f, 0.9f});          // point 1, both axes
   states.push_back(state_of(*v.editor));
   v.double_click({0.35f, 0.3f});               // insert between 1 and 2
   states.push_back(state_of(*v.editor));
   v.drag({0.35f, 0.3f}, {0.35f, 0.6f});        // the new point
   states.push_back(state_of(*v.editor));
   v.double_click({0.2f, 0.9f});                // erase point 1
   states.push_back(state_of(*v.editor));
   REQUIRE(v.editor->size() == 4);

   for (int k = int(states.size()) - 2; k >= 0; --k)
   {
      CHECK(v.view_.undo());
      CHECK(same(state_of(*v.editor), states[k]));
   }
   CHECK(!v.view_.has_undo());

   for (std::size_t k = 1; k != states.size(); ++k)
   {
      CHECK(v.view_.redo());
      CHECK(same(state_of(*v.editor), states[k]));
   }
   CHECK(!v.view_.has_redo());
}

TEST_CASE("curve_editor undo: a step the client refuses changes nothing")
{
   editor_view v{undo_envelope()};
   recorder r;
   r.attach(*v.editor);

   v.double_click({0.6f, 0.5f});                // erase point 2
   auto const end = state_of(*v.editor);
   v.editor->on_insert = [](std::size_t, point) { return false; };
   CHECK(v.view_.undo());
   CHECK(same(state_of(*v.editor), end));
}

TEST_CASE("curve_editor undo: only what the user does is a step")
{
   editor_view v{undo_envelope()};

   v.drag({0.6f, 0.5f}, {0.6f, 0.5f});          // a press that moves nothing
   CHECK(!v.view_.has_undo());

   v.editor->y(2, 0.7f);                        // code
   v.editor->insert(4, cp{{0.05f, 0.0f}, true});
   v.editor->erase(4);
   CHECK(!v.view_.has_undo());
}

TEST_CASE("curve_editor undo: a step leaves alone what code has set since")
{
   editor_view v{undo_envelope()};
   v.drag({0.6f, 0.5f}, {0.6f, 0.8f});          // the user moves point 2

   // Code sets point 1 and point 3 afterwards: a model, automation
   v.editor->y(1, 0.4f);
   v.editor->x(3, 0.2f);
   auto const by_code = state_of(*v.editor);

   CHECK(v.view_.undo());
   CHECK(v.editor->y(2) == Approx(0.5f).margin(eps));   // the user's edit
   CHECK(v.editor->y(1) == Approx(0.4f).margin(eps));   // code's, kept
   CHECK(v.editor->x(3) == Approx(0.2f).margin(eps));

   CHECK(v.view_.redo());
   CHECK(same(state_of(*v.editor), by_code));
}

TEST_CASE("curve_editor undo: a step whose point code has erased does nothing")
{
   editor_view v{undo_envelope()};
   recorder r;
   r.attach(*v.editor);
   v.drag({0.6f, 0.5f}, {0.6f, 0.8f});
   v.editor->erase(2);
   auto const now = state_of(*v.editor);

   // Stale: dropped, and nothing else to undo
   r.log.clear();
   CHECK(!v.view_.undo());
   CHECK(same(state_of(*v.editor), now));
   CHECK(r.log.empty());
}

TEST_CASE("curve_editor undo: a stale step is passed over for the one before")
{
   editor_view v{undo_envelope()};
   v.drag({0.6f, 0.5f}, {0.6f, 0.8f});          // point 2
   v.drag({0.9f, 0.0f}, {0.9f, 0.3f});          // point 3
   v.editor->erase(3);                          // by code: point 3's step is stale

   CHECK(v.view_.undo());                       // one press: point 2's drag
   CHECK(v.editor->y(2) == Approx(0.5f).margin(eps));
   CHECK(!v.view_.has_undo());
}

TEST_CASE("curve_editor undo: a drag on both axes reports x, then y, then on_change")
{
   editor_view v{undo_envelope()};
   recorder r;
   r.attach(*v.editor);

   v.drag({0.6f, 0.5f}, {0.65f, 0.7f});
   r.log.clear();
   CHECK(v.view_.undo());
   CHECK(r.log == log_type{"2 x(", "2 x", "2 x)", "2 y(", "2 y", "2 y)", "change 2"});
   r.log.clear();
   CHECK(v.view_.redo());
   CHECK(r.log == log_type{"2 x(", "2 x", "2 x)", "2 y(", "2 y", "2 y)", "change 2"});
}

TEST_CASE("curve_editor undo: an offset point's drag reports that point only")
{
   // The points after it move with it, as in the drag itself: their held
   // offsets are unchanged, and no callback of theirs is called
   editor_view v{undo_envelope()};
   recorder r;
   r.attach(*v.editor);

   r.log.clear();
   v.drag({0.3f, 1.0f}, {0.4f, 1.0f});
   auto const dragged = r.log;
   CHECK(dragged == log_type{"1 x(", "1 x", "change 1", "1 x)"});

   r.log.clear();
   CHECK(v.view_.undo());
   CHECK(r.log == log_type{"1 x(", "1 x", "1 x)", "change 1"});
}

TEST_CASE("curve_editor undo: refused on the way back or forward, a step changes nothing")
{
   editor_view v{undo_envelope()};
   recorder r;
   r.attach(*v.editor);

   // An undone insert, refused
   v.double_click({0.45f, 0.2f});
   auto const inserted = state_of(*v.editor);
   v.editor->on_erase = [](std::size_t) { return false; };
   CHECK(v.view_.undo());
   CHECK(same(state_of(*v.editor), inserted));

   // A redone erase, refused
   r.attach(*v.editor);
   v.double_click({0.6f, 0.5f});
   CHECK(v.view_.undo());
   auto const back = state_of(*v.editor);
   v.editor->on_erase = [](std::size_t) { return false; };
   CHECK(v.view_.redo());
   CHECK(same(state_of(*v.editor), back));
}

TEST_CASE("curve_editor undo: the first and the last point, erased and undone")
{
   editor_view v{undo_envelope()};
   recorder r;
   r.attach(*v.editor);
   v.editor->movable = {};
   auto const start = state_of(*v.editor);

   // The first: the offset point after it is first now, its x a place
   v.double_click({0.0f, 0.0f});
   REQUIRE(v.editor->size() == 3);
   CHECK(v.editor->position(0).x == Approx(0.3f).margin(eps));
   CHECK(v.editor->position(2).x == Approx(0.9f).margin(eps));
   CHECK(v.view_.undo());
   CHECK(same(state_of(*v.editor), start));

   // The last: nothing after it to report
   r.log.clear();
   v.double_click({0.9f, 0.0f});
   CHECK(r.log == log_type{"erase 3"});
   r.log.clear();
   CHECK(v.view_.undo());
   CHECK(r.log == log_type{"insert 3"});
   CHECK(same(state_of(*v.editor), start));
}

TEST_CASE("curve_editor undo: an insert at either end")
{
   // Free points, not offsets, with room before the first and after the
   // last
   editor_view v{{cp{{0.3f, 0.5f}}, cp{{0.6f, 0.5f}}}};
   recorder r;
   r.attach(*v.editor);
   auto const start = state_of(*v.editor);

   v.double_click({0.1f, 0.2f});
   REQUIRE(v.editor->size() == 3);
   CHECK(v.editor->position(0).x == Approx(0.1f).margin(eps));
   v.double_click({0.9f, 0.8f});
   REQUIRE(v.editor->size() == 4);
   CHECK(v.editor->position(3).x == Approx(0.9f).margin(eps));
   auto const end = state_of(*v.editor);

   CHECK(v.view_.undo());
   CHECK(v.view_.undo());
   CHECK(same(state_of(*v.editor), start));
   CHECK(v.view_.redo());
   CHECK(v.view_.redo());
   CHECK(same(state_of(*v.editor), end));
}

TEST_CASE("curve_editor undo: a new edit after an undo clears what could be redone")
{
   editor_view v{undo_envelope()};
   v.drag({0.6f, 0.5f}, {0.6f, 0.8f});
   CHECK(v.view_.undo());
   REQUIRE(v.view_.has_redo());
   v.drag({0.6f, 0.5f}, {0.6f, 0.2f});
   CHECK(!v.view_.has_redo());
   CHECK(v.view_.undo());
   CHECK(v.editor->y(2) == Approx(0.5f).margin(eps));
   CHECK(!v.view_.has_undo());
}

TEST_CASE("curve_editor undo: a point put back where the points before it now end")
{
   // The last point erased, then code erases another: the step puts it
   // back at the end, not past it
   editor_view v{undo_envelope()};
   recorder r;
   r.attach(*v.editor);
   auto const last = v.editor->node_at(3);
   v.double_click({0.9f, 0.0f});
   v.editor->erase(2);
   REQUIRE(v.editor->size() == 2);

   r.log.clear();
   CHECK(v.view_.undo());
   REQUIRE(v.editor->size() == 3);
   CHECK(v.editor->node_at(2) == last);
   CHECK(r.log == log_type{"insert 2"});
}

TEST_CASE("curve_editor undo: two editors in a view share its stack, in order")
{
   test_view tv{{432, 116}};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   auto a = share(curve_editor({cp{{0.5f, 0.5f}}}));
   auto b = share(curve_editor({cp{{0.5f, 0.5f}}}));
   tv.view_.content(htile(hold(a), hold(b)));
   tv.view_.layout();
   tv.draw();

   auto drag = [&](auto& e, point from, point to)
   {
      rect sq;
      tv.view_.in_context_do(*e, [&](context const& ctx) { sq = e->plot(ctx); });
      auto press = [](point p, bool down)
      {
         mouse_button btn{};
         btn.down = down;
         btn.state = mouse_button::left;
         btn.num_clicks = 1;
         btn.pos = p;
         return btn;
      };
      tv.view_.click(press(e->to_screen(from, sq), true));
      tv.view_.drag(press(e->to_screen(to, sq), true));
      tv.view_.click(press(e->to_screen(to, sq), false));
      tv.draw();
   };

   drag(a, {0.5f, 0.5f}, {0.5f, 0.8f});
   drag(b, {0.5f, 0.5f}, {0.5f, 0.2f});
   drag(a, {0.5f, 0.8f}, {0.5f, 0.9f});

   CHECK(tv.view_.undo());
   CHECK(a->y(0) == Approx(0.8f).margin(eps));
   CHECK(b->y(0) == Approx(0.2f).margin(eps));
   CHECK(tv.view_.undo());
   CHECK(b->y(0) == Approx(0.5f).margin(eps));
   CHECK(a->y(0) == Approx(0.8f).margin(eps));
   CHECK(tv.view_.undo());
   CHECK(a->y(0) == Approx(0.5f).margin(eps));
   CHECK(!tv.view_.has_undo());
}

TEST_CASE("curve_editor undo: a bound model follows undo and redo")
{
   editor_view v{undo_envelope()};
   value_model<float> sustain = 0.5f;
   value_model<float> decay = 0.3f;
   model_binder binder{[&](element& e) { v.view_.refresh(e); }};
   binder.bind(sustain, y_of(v.editor, 2));
   binder.bind(decay, x_of(v.editor, 2));

   v.drag({0.6f, 0.5f}, {0.6f, 0.75f});
   CHECK(sustain.get() == Approx(0.75f).margin(eps));
   CHECK(v.view_.undo());
   CHECK(sustain.get() == Approx(0.5f).margin(eps));
   CHECK(v.view_.redo());
   CHECK(sustain.get() == Approx(0.75f).margin(eps));

   // Point 1 erased and back: point 2's segment, its x, merges and splits
   v.editor->on_erase = [](std::size_t) { return true; };
   v.editor->on_insert = [](std::size_t, point) { return true; };
   v.double_click({0.3f, 1.0f});
   CHECK(decay.get() == Approx(0.6f).margin(eps));
   CHECK(v.view_.undo());
   CHECK(decay.get() == Approx(0.3f).margin(eps));
   CHECK(v.view_.redo());
   CHECK(decay.get() == Approx(0.6f).margin(eps));
}

TEST_CASE("curve_editor undo: an undo in the middle of a drag")
{
   editor_view v{undo_envelope()};
   auto press = [&](point u, bool down) { return editor_view::press(v.screen(u), down); };

   // An earlier drag of point 3, then point 2 taken hold of and moved
   v.drag({0.9f, 0.0f}, {0.9f, 0.2f});
   v.view_.click(press({0.6f, 0.5f}, true));
   v.view_.drag(press({0.6f, 0.6f}, true));

   // Undone while dragging: point 3 goes back, the drag goes on
   CHECK(v.view_.undo());
   CHECK(v.editor->y(3) == Approx(0.0f).margin(eps));
   v.view_.drag(press({0.6f, 0.7f}, true));
   v.view_.click(press({0.6f, 0.7f}, false));
   CHECK(v.editor->y(2) == Approx(0.7f).margin(eps));

   // The drag is a step of its own, from where it started
   CHECK(v.view_.undo());
   CHECK(v.editor->y(2) == Approx(0.5f).margin(eps));
   CHECK(!v.view_.has_undo());
}

TEST_CASE("curve_editor undo: undoing the insert of the point being dragged ends the drag")
{
   editor_view v{undo_envelope()};
   recorder r;
   r.attach(*v.editor);
   auto press = [&](point u, bool down) { return editor_view::press(v.screen(u), down); };

   v.double_click({0.45f, 0.2f});
   REQUIRE(v.editor->size() == 5);
   v.view_.click(press({0.45f, 0.2f}, true));
   v.view_.drag(press({0.45f, 0.4f}, true));

   CHECK(v.view_.undo());                       // the insert, while dragging
   CHECK(v.editor->size() == 4);
   v.view_.drag(press({0.45f, 0.6f}, true));    // nothing to drag
   v.view_.click(press({0.45f, 0.6f}, false));
   CHECK(v.editor->size() == 4);
   CHECK(!v.view_.has_undo());                  // and no step for it
}

TEST_CASE("curve_editor undo: a step changes only the editor that made it")
{
   editor_view v{undo_envelope()};
   recorder r;
   r.attach(*v.editor);
   auto copy = *v.editor;

   v.double_click({0.6f, 0.5f});                // erase point 2
   CHECK(v.editor->size() == 3);
   CHECK(copy.size() == 4);
   CHECK(v.view_.undo());
   CHECK(v.editor->size() == 4);
   CHECK(copy.size() == 4);
   CHECK(v.view_.redo());
   CHECK(v.editor->size() == 3);
   CHECK(copy.size() == 4);
}

TEST_CASE("curve_editor: a copy is an editor of its own")
{
   editor_view v{undo_envelope()};
   int called = 0;
   (*v.editor)[2].on_y_change = [&](float) { ++called; };
   auto copy = *v.editor;

   // New nodes, holding the same points, with the same callbacks
   CHECK(copy.node_at(2) != v.editor->node_at(2));
   for (std::size_t i = 0; i != copy.size(); ++i)
   {
      CHECK(copy.x(i) == v.editor->x(i));
      CHECK(copy.y(i) == v.editor->y(i));
      CHECK(copy[i].spec.offset_x == (*v.editor)[i].spec.offset_x);
   }
   REQUIRE(copy[2].on_y_change);
   copy[2].on_y_change(0.1f);
   CHECK(called == 1);

   // A drag in the original leaves the copy's point where it was
   v.drag({0.6f, 0.5f}, {0.6f, 0.8f});
   CHECK(v.editor->y(2) == Approx(0.8f).margin(eps));
   CHECK(copy.y(2) == Approx(0.5f).margin(eps));

   // and undoing it touches the original only
   copy.y(2, 0.3f);
   CHECK(v.view_.undo());
   CHECK(v.editor->y(2) == Approx(0.5f).margin(eps));
   CHECK(copy.y(2) == Approx(0.3f).margin(eps));
}

TEST_CASE("curve_editor: a moved editor keeps its points and its undo steps")
{
   editor_view v{undo_envelope()};
   v.drag({0.6f, 0.5f}, {0.6f, 0.8f});
   auto const node = v.editor->node_at(2);

   // Moved into a new editor, shared and placed in the view
   auto moved = std::make_shared<editor_view::editor_type>(std::move(*v.editor));
   CHECK(moved->node_at(2) == node);
   v.view_.content(hold(moved));
   v.view_.layout();
   v.draw();

   CHECK(v.view_.undo());
   CHECK(moved->y(2) == Approx(0.5f).margin(eps));
   CHECK(v.view_.redo());
   CHECK(moved->y(2) == Approx(0.8f).margin(eps));
}

TEST_CASE("curve_editor undo: a step outlives its editor harmlessly")
{
   editor_view v{undo_envelope()};
   v.drag({0.6f, 0.5f}, {0.6f, 0.8f});
   v.view_.content(element{});
   v.editor.reset();
   CHECK(!v.view_.undo());                      // stale: dropped
   CHECK(!v.view_.has_undo());
}

TEST_CASE("curve_editor: x_of and y_of bind one axis of one point")
{
   editor_view v{{cp{{0.25f, 0.5f}}, cp{{0.75f, 0.5f}}}};
   auto px = x_of(v.editor, 1);
   auto py = y_of(v.editor, 1);

   px->value(0.6f);
   py->value(0.1f);
   CHECK(v.editor->x(1) == Approx(0.6f).margin(eps));
   CHECK(v.editor->y(1) == Approx(0.1f).margin(eps));
   CHECK(px->refresh_target() == v.editor.get());

   float seen = -1;
   px->on_change = [&](float val) { seen = val; };
   v.drag({0.6f, 0.1f}, {0.7f, 0.1f});
   CHECK(seen == Approx(0.7f).margin(eps));

   // Its point erased, the proxy is harmless
   v.editor->erase(1);
   px->value(0.9f);
   CHECK(v.editor->size() == 1);
   CHECK(v.editor->x(0) == Approx(0.25f).margin(eps));
}

TEST_CASE("curve_editor: the editor keeps the axes it hands out")
{
   editor_view v{{cp{{0.25f, 0.5f}}, cp{{0.75f, 0.5f}}}};

   // Asked again, the same one
   CHECK(x_of(v.editor, 1) == x_of(v.editor, 1));
   CHECK(x_of(v.editor, 1) != y_of(v.editor, 1));

   // Bound and let go, it still sets the point: the editor keeps it
   std::weak_ptr<curve_value> weak = x_of(v.editor, 1);
   REQUIRE(!weak.expired());
   weak.lock()->value(0.6f);
   CHECK(v.editor->x(1) == Approx(0.6f).margin(eps));

   // Its point erased, the editor lets it go
   v.editor->erase(1);
   CHECK(weak.expired());
}

TEST_CASE("curve_editor: on_change reports the drag, not a callback's reset")
{
   editor_view v{{cp{{0.5f, 0.5f}}}};
   point seen{-1, -1};
   (*v.editor)[0].on_y_change = [&](float) { v.editor->y(0, 0.5f); };
   v.editor->on_change = [&](std::size_t, point pos) { seen = pos; };

   v.drag({0.5f, 0.5f}, {0.5f, 0.9f});
   CHECK(seen.y == Approx(0.9f).margin(eps));
   CHECK(v.editor->y(0) == Approx(0.5f).margin(eps));
}

TEST_CASE("curve_editor: on_change reports an offset point in the square")
{
   cp start{{0.0f, 0.0f}};
   cp a{{0.2f, 1.0f}, true};
   cp b{{0.3f, 0.5f}, true};
   editor_view v{{start, a, b}};
   point seen{-1, -1};
   v.editor->on_change = [&](std::size_t, point pos) { seen = pos; };

   v.drag({0.5f, 0.5f}, {0.6f, 0.5f});
   CHECK(seen.x == Approx(0.6f).margin(eps));             // a place
   CHECK(v.editor->x(2) == Approx(0.4f).margin(eps));     // a distance
}

TEST_CASE("curve_lines: a shape bends each segment, sampled along it")
{
   editor_view v{{cp{{0.0f, 0.0f}}, cp{{0.5f, 1.0f}}, cp{{1.0f, 0.5f}}}};
   auto& lines = v.editor->actual_subject();
   std::vector<std::pair<std::size_t, float>> asked;
   lines.steps = 4;
   lines.shape = [&](std::size_t segment, float t)
   {
      asked.push_back({segment, t});
      return t * t;
   };

   // Drawn once for the fill and once for the line
   v.draw();
   REQUIRE(asked.size() == 2 * 2 * 3);
   for (auto [segment, t] : asked)
   {
      CHECK(segment < 2);
      CHECK(t > 0.0f);
      CHECK(t < 1.0f);
   }

   // A transparent fill draws none, so the line alone asks
   asked.clear();
   lines.fill_color = lines.fill_color.opacity(0);
   v.draw();
   CHECK(asked.size() == 2 * 3);
}

TEST_CASE("curve_editor: fit_width spreads the points across the plot")
{
   editor_view v{{cp{{0.0f, 0.0f}}, cp{{0.25f, 1.0f}}, cp{{0.5f, 0.5f}}}};
   v.editor->fit_width = true;
   auto const right = v.editor->to_screen({0.5f, 0.5f}, v.square);
   CHECK(right.x == Approx(v.square.right).margin(eps));
   auto const back = v.editor->from_screen(right, v.square);
   CHECK(back.x == Approx(0.5f).margin(eps));
}
