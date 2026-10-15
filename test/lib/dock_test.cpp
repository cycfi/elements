/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   The dock: panes added to a side share it, beside the content; a window
   whose title is dragged near the dock's edge snaps to it and shows where
   it would dock, and docks on release; its title dragged out of the dock
   undocks it into its window.
=============================================================================*/
#include "test_support.hpp"
#include <string>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   // Records its bounds; at least 40 by 40.
   struct slot : element
   {
      view_limits limits(basic_context const&) const override
      {
         return {{40, 40}, {full_extent, full_extent}};
      }
      void layout(context const& ctx) override { bounds = ctx.bounds; }
      void draw(context const& ctx) override { bounds = ctx.bounds; }
      rect bounds;
   };

   mouse_button left(point p, bool down)
   {
      mouse_button b{};
      b.state = mouse_button::left;
      b.down = down;
      b.num_clicks = 1;
      b.pos = p;
      return b;
   }

   // A dockable pane: a title, which drags it, over its content.
   auto make_pane(std::shared_ptr<dock_element> d, std::shared_ptr<slot> s)
   {
      return share(dockable(d,
         vtile(
            movable(vsize(20, box(colors::gray[40]))),
            hold(s)
         )));
   }
}

TEST_CASE("dock: panes share a side, beside the content", "[dock]")
{
   auto content = share(slot{});
   auto a = share(slot{});
   auto b = share(slot{});
   auto d = share(dock(hold(content)));
   test_view tv{extent{400, 300}};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(d);
   tv.draw();
   CHECK(content->bounds == rect{0, 0, 400, 300});

   d->add(a, dock_side::left);
   tv.view_.layout();
   tv.draw();
   CHECK(d->side_of(a) == dock_side::left);
   CHECK(a->bounds.left == 0);
   CHECK(a->bounds.height() == 300);
   CHECK(content->bounds.left == a->bounds.right);

   d->add(b, dock_side::left);
   tv.view_.layout();
   tv.draw();
   CHECK(a->bounds.height() == Approx(150));     // shared evenly
   CHECK(b->bounds.top == Approx(150));

   CHECK(d->remove(a));
   tv.view_.layout();
   tv.draw();
   CHECK(!d->side_of(a));
   CHECK(b->bounds.height() == 300);
}

TEST_CASE("dock: a window docks and undocks", "[dock]")
{
   auto content = share(slot{});
   auto s = share(slot{});
   auto d = share(dock(hold(content)));
   auto pane = make_pane(d, s);
   auto window = share(child_window({150, 100, 250, 200}, hold(pane)));

   test_view tv{extent{400, 300}};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(window, d);
   tv.draw();
   auto& v = tv.view_;

   // Drag the title to within gravity of the left edge: it snaps there,
   // and the dock shows where it would go.
   v.click(left({200, 110}, true));
   v.drag(left({63, 110}, true));
   CHECK(window->bounds().left == 0);
   CHECK(d->preview() == dock_side::left);

   // Release: docked.
   v.click(left({63, 110}, false));
   v.poll();
   v.layout();
   tv.draw();
   CHECK(!v.is_open(window));
   CHECK(d->side_of(pane) == dock_side::left);
   CHECK(!d->preview());
   CHECK(s->bounds.left == 0);
   CHECK(content->bounds.left == Approx(s->bounds.right));

   // Drag the docked title out: undocked, into its window, which opens
   // where the pointer is.
   auto title = point{s->bounds.left + 10, s->bounds.top - 10};
   v.click(left(title, true));
   v.drag(left({title.x + 200, title.y + 50}, true));
   v.poll();
   CHECK(!d->side_of(pane));
   CHECK(v.is_open(window));
   CHECK(window->bounds().left == Approx(200));

   // The same drag goes on, moving the window.
   v.drag(left({title.x + 230, title.y + 60}, true));
   v.click(left({title.x + 230, title.y + 60}, false));
   CHECK(window->bounds().left == Approx(230));
}

TEST_CASE("dock: the pointer near an edge docks too", "[dock]")
{
   auto d = share(dock(slot{}));
   auto pane = make_pane(d, share(slot{}));
   auto window = share(child_window({150, 100, 250, 200}, hold(pane)));
   test_view tv{extent{400, 300}};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(window, d);
   tv.draw();

   // The pointer at the bottom edge; the window is still far from it.
   tv.view_.click(left({200, 110}, true));
   tv.view_.drag(left({200, 290}, true));
   CHECK(d->preview() == dock_side::bottom);
   CHECK(window->bounds().bottom == 300);
   tv.view_.click(left({200, 290}, false));
   tv.view_.poll();
   CHECK(d->side_of(pane) == dock_side::bottom);
}

TEST_CASE("dock: a pane docked from the start undocks", "[dock]")
{
   auto s = share(slot{});
   auto d = share(dock(slot{}));
   auto pane = make_pane(d, s);
   d->add(pane, dock_side::right);
   test_view tv{extent{400, 300}};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(d);
   tv.draw();
   auto docked = s->bounds;
   REQUIRE(docked.left > 0);

   // Into a window of its own, the size it had in the dock.
   auto title = point{docked.left + 10, docked.top - 10};
   tv.view_.click(left(title, true));
   tv.view_.drag(left({title.x - 150, title.y + 40}, true));
   tv.view_.click(left({title.x - 150, title.y + 40}, false));
   tv.view_.poll();
   CHECK(!d->side_of(pane));
   REQUIRE(tv.view_.layers().size() == 2);
   auto w = std::dynamic_pointer_cast<floating_element>(
      tv.view_.layers().back());
   REQUIRE(w);
   CHECK(w->bounds().width() == Approx(docked.width()));
}
