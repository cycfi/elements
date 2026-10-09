/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/elements/trackers.adoc.
=============================================================================*/
#include "test_support.hpp"
#include <string>
#include <vector>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   // The page's example: a box the user drags around, built on tracker.
   // It keeps where it is, and moves by the mouse's movement.
   struct draggable_box : tracker<>
   {
      view_limits limits(basic_context const&) const override
      {
         return {{0, 0}, {full_extent, full_extent}};
      }

      void draw(context const& ctx) override
      {
         ctx.canvas.fill_style(colors::gold);
         ctx.canvas.add_rect(box.move(ctx.bounds.left, ctx.bounds.top));
         ctx.canvas.fill();
      }

      void begin_tracking(context const& ctx, tracker_info& info) override
      {
         log.push_back("begin");
         // Take hold only on the box; otherwise this is not our drag.
         info.processed = box.move(ctx.bounds.left, ctx.bounds.top)
            .includes(info.current);
      }

      void keep_tracking(context const& ctx, tracker_info& info) override
      {
         log.push_back("keep");
         auto m = info.movement();
         box = box.move(m.x, m.y);
         ctx.view.refresh(ctx);
      }

      void end_tracking(context const&, tracker_info& info) override
      {
         log.push_back("end");
         total = info.distance();
      }

      rect                       box = {10, 10, 50, 50};
      point                      total = {};
      std::vector<std::string>   log;
   };

   mouse_button left_button(point p, bool down, int modifiers = 0)
   {
      mouse_button btn{};
      btn.down = down;
      btn.state = mouse_button::left;
      btn.num_clicks = 1;
      btn.pos = p;
      btn.modifiers = modifiers;
      return btn;
   }

   struct box_view : test_view
   {
      box_view()
       : test_view{{200, 200}}
       , b{share(draggable_box{})}
      {
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
         headless::open(view_);
#endif
         view_.content(hold(b));
         view_.layout();
         draw();
         view_.on_tracking = [this](element&, element::tracking state)
         {
            switch (state)
            {
               case element::begin_tracking: reports.push_back("begin"); break;
               case element::while_tracking: reports.push_back("while"); break;
               case element::end_tracking:   reports.push_back("end");   break;
               default: break;
            }
         };
      }

      std::shared_ptr<draggable_box> b;
      std::vector<std::string>       reports;
   };
}

TEST_CASE("trackers page: a drag is begin, keep, end", "[trackers_page]")
{
   box_view bv;
   CHECK(!bv.b->is_tracking());

   bv.view_.click(left_button({20, 20}, true));
   CHECK(bv.b->is_tracking());
   CHECK(bv.b->log == std::vector<std::string>{"begin"});

   bv.view_.drag(left_button({30, 25}, true));
   bv.view_.drag(left_button({45, 40}, true));
   CHECK(bv.b->log == std::vector<std::string>{"begin", "keep", "keep"});
   CHECK(bv.b->box == rect{35, 30, 75, 70});       // moved by (25, 20)

   bv.view_.click(left_button({45, 40}, false));
   CHECK(!bv.b->is_tracking());
   CHECK(bv.b->log == std::vector<std::string>{"begin", "keep", "keep", "end"});
   CHECK(bv.b->total == point{25, 20});            // distance from the start

   // The view's on_tracking saw the same gesture.
   CHECK(bv.reports == std::vector<std::string>{"begin", "while", "while", "end"});
}

TEST_CASE("trackers page: tracker_info", "[trackers_page]")
{
   tracker_info info{{10, 10}, mod_shift};
   CHECK(info.start == point{10, 10});
   CHECK(info.current == point{10, 10});
   CHECK(info.previous == point{10, 10});
   CHECK(info.modifiers == mod_shift);
   CHECK(info.processed);

   info.previous = info.current;
   info.current = {15, 12};
   CHECK(info.movement() == point{5, 2});
   CHECK(info.distance() == point{5, 2});
   info.previous = info.current;
   info.current = {30, 30};
   CHECK(info.movement() == point{15, 18});
   CHECK(info.distance() == point{20, 20});
}

TEST_CASE("trackers page: offset shifts the current point", "[trackers_page]")
{
   // A tracker that grabs its subject off-centre sets offset in
   // begin_tracking, and every current point after is shifted by it, so
   // the gesture reads as if it had begun at the grab point's origin.
   struct offset_box : draggable_box
   {
      void begin_tracking(context const& ctx, tracker_info& info) override
      {
         draggable_box::begin_tracking(ctx, info);
         info.offset = {5, 5};
      }
      void keep_tracking(context const&, tracker_info& info) override
      {
         currents.push_back(info.current);
      }
      std::vector<point> currents;
   };

   test_view tv{extent{200, 200}, 1};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   auto b = share(offset_box{});
   tv.view_.content(hold(b));
   tv.view_.layout();
   tv.draw();

   tv.view_.click(left_button({20, 20}, true));
   tv.view_.drag(left_button({30, 30}, true));
   CHECK(b->currents == std::vector<point>{{25, 25}});
}

TEST_CASE("trackers page: processed declines the press", "[trackers_page]")
{
   // A press off the box: begin_tracking says it was not processed, so
   // click returns false, and the composite sends no drags.
   box_view bv;
   bv.view_.click(left_button({150, 150}, true));
   CHECK(!bv.b->is_tracking());                 // the state is dropped at once
   bv.view_.drag(left_button({160, 160}, true));
   CHECK(bv.b->log == std::vector<std::string>{"begin"});
   bv.view_.click(left_button({160, 160}, false));
   CHECK(bv.b->log == std::vector<std::string>{"begin"});

   // The view's on_tracking saw a begin and its end, nothing between.
   CHECK(bv.reports == std::vector<std::string>{"begin", "end"});
}

TEST_CASE("trackers page: escape_tracking ends it early", "[trackers_page]")
{
   box_view bv;
   bv.view_.click(left_button({20, 20}, true));
   bv.view_.in_context_do(*bv.b,
      [&](context const& ctx) { bv.b->escape_tracking(ctx); });
   CHECK(!bv.b->is_tracking());
   CHECK(bv.b->log == std::vector<std::string>{"begin", "end"});
   CHECK(bv.reports == std::vector<std::string>{"begin", "end"});
}

TEST_CASE("trackers page: the slider is a tracker", "[trackers_page]")
{
   // The library's sliders, dials and thumbwheels are trackers over a
   // receiver; the gesture reaches on_tracking as the page says.
   auto s = share(slider(basic_thumb<20>(), basic_track<5, false>(), 0.5));
   test_view tv{extent{200, 100}, 1};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(align_middle(hold(s)));
   tv.view_.layout();
   tv.draw();
   CHECK(!s->is_tracking());
   tv.view_.click(left_button({100, 50}, true));
   CHECK(s->is_tracking());
   tv.view_.click(left_button({100, 50}, false));
   CHECK(!s->is_tracking());
}
