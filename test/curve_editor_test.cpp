/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   A curve editor driven the way a user drives it: presses, drags and
   releases on its points, placed through the editor's own mapping from
   its unit square to the screen, as laid out.
=============================================================================*/
#include "test_support.hpp"

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

TEST_CASE("curve_editor: a relative point moves the points after it")
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

TEST_CASE("curve_editor: inserting and erasing keeps relative points in place")
{
   cp start{{0.0f, 0.0f}};
   cp a{{0.4f, 1.0f}, true};
   cp b{{0.4f, 0.0f}, true};
   editor_view v{{start, a, b}};
   REQUIRE(v.editor->position(2).x == Approx(0.8f).margin(eps));

   // A relative point inserted at 0.2 splits the first segment
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
