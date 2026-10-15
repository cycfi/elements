/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/elements/docking.adoc and
   draws its figure. The figure lands in the build's results directory;
   copy it to docs/modules/ROOT/images/elements/ to update the page.
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

   // A dockable pane, as the page builds one: a title over its content.
   auto make_pane(std::shared_ptr<dock_element> d, std::shared_ptr<slot> s)
   {
      return share(dockable(d,
         vtile(
            movable(vsize(20, box(colors::gray[40]))),
            hold(s)
         )));
   }

   void open(test_view& tv)
   {
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
      headless::open(tv.view_);
#endif
   }
}

TEST_CASE("docking page: the dock", "[docking_page]")
{
   // A docked side takes a quarter; panes on a side share it evenly.
   auto content = share(slot{});
   auto a = share(slot{});
   auto b = share(slot{});
   auto d = share(dock(hold(content)));
   test_view tv{extent{400, 400}};
   open(tv);
   tv.view_.content(d);
   tv.draw();

   d->add(a, dock_side::right);
   d->add(b, dock_side::right);
   tv.view_.layout();
   tv.draw();
   CHECK(a->bounds.width() == Approx(100));
   CHECK(a->bounds.height() == Approx(200));
   CHECK(b->bounds.top == Approx(200));
   CHECK(content->bounds.right == Approx(300));
   CHECK(d->side_of(b) == dock_side::right);

   d->preview(dock_side::top);
   CHECK(d->preview() == dock_side::top);
   d->preview(std::nullopt);
   CHECK(!d->preview());

   CHECK(d->remove(a));
   CHECK(!d->remove(a));
}

TEST_CASE("docking page: docking and undocking a window", "[docking_page]")
{
   auto s = share(slot{});
   auto d = share(dock(slot{}));
   auto pane = make_pane(d, s);
   auto window = share(child_window({150, 100, 250, 200}, hold(pane)));
   test_view tv{extent{400, 300}};
   open(tv);
   tv.view_.content(window, d);
   tv.draw();
   auto& v = tv.view_;

   // An edge within gravity of the dock's: it snaps, and shows.
   v.click(left({200, 110}, true));
   v.drag(left({200, 2}, true));
   CHECK(window->bounds().top == 0);
   CHECK(d->preview() == dock_side::top);

   // Released: docked, and out of the view's layers.
   v.click(left({200, 2}, false));
   v.poll();
   CHECK(d->side_of(pane) == dock_side::top);
   CHECK(!v.is_open(window));
   CHECK(pane->is_docked());

   // Out by more than gravity: undocked, into its window; the drag goes
   // on.
   v.layout();
   tv.draw();
   auto title = point{s->bounds.left + 30, s->bounds.top - 10};
   v.click(left(title, true));
   v.drag(left({title.x, title.y + 100}, true));
   v.poll();
   CHECK(!pane->is_docked());
   CHECK(v.is_open(window));
   auto top = window->bounds().top;
   v.drag(left({title.x, title.y + 120}, true));
   v.click(left({title.x, title.y + 120}, false));
   CHECK(window->bounds().top == Approx(top + 20));
}

#if !defined(ARTIST_RECORDING)
TEST_CASE("docking page: example", "[docking_page]")
{
   auto main_content = layer(
      align_center_middle(label("Main content").font_size(24)),
      box(rgba(35, 35, 37, 255))
   );
   auto d = share(dock(std::move(main_content)));
   auto make = [&](char const* title, char const* text)
   {
      return share(dockable(d,
         pane_ex(
            movable(title_bar{}),
            label(title),
            margin({10, 10, 10, 10}, align_left_top(label(text))),
            get_theme().child_window_opacity
         )));
   };
   d->add(make("Tools", "Tools go here."), dock_side::left);
   d->add(make("Layers", "Layers go here."), dock_side::left);
   d->add(make("Output", "Output goes here."), dock_side::bottom);
   auto floating_ = share(child_window({440, 180, 620, 300},
      resizable(hold(make("Floating", "Drag my title to an edge.")))));

   test_view tv{extent{640, 420}, 2};
   open(tv);
   tv.view_.content(floating_, d);
   tv.draw();
   tv.img.save_png(std::string{RESULTS_PATH} + "docking_example.png");
}
#endif
