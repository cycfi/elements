/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/layout/collapsible.adoc
   and draws its figure. The figure lands in the build's results
   directory; copy it to docs/modules/ROOT/images/layout/ to update the
   page.
=============================================================================*/
#include "test_support.hpp"
#include <string>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   auto constexpr bkd_color = rgba(35, 35, 37, 255);

   // An element of a fixed size that records the bounds it was drawn with.
   struct block : element
   {
      view_limits limits(basic_context const&) const override
      {
         return {{60, 30}, {full_extent, 30}};
      }

      void draw(context const& ctx) override
      {
         last_bounds = ctx.bounds;
         ctx.canvas.fill_style(colors::gold);
         ctx.canvas.add_rect(ctx.bounds);
         ctx.canvas.fill();
      }

      rect last_bounds = {};
   };

   // The page's example: a header over a body that folds away.
   struct section
   {
      bool open = true;

      auto make()
      {
         auto body = vcollapsible(
            layer(
               margin({10, 10, 10, 10}, align_left(label("Collapsible section"))),
               rbox(rgba(50, 70, 100, 255))
            ));
         body.is_collapsed = [this]{ return !open; };
         return vtile(
            align_left(label("A section").font_size(16)),
            margin_top(6, std::move(body)),
            margin_top(8, align_left(label("What follows the section")))
         );
      }
   };
}

TEST_CASE("collapsible page: vcollapsible collapses the height", "[collapsible_page]")
{
   bool collapsed = false;
   auto top = share(block{});
   auto middle = share(block{});
   auto bottom = share(block{});
   auto folding = vcollapsible(hold(middle));
   folding.is_collapsed = [&collapsed]{ return collapsed; };

   test_view tv{extent{200, 200}, 1};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(align_top(vtile(hold(top), std::move(folding), hold(bottom))));
   tv.view_.layout();
   tv.draw();

   // Open: three blocks of 30.
   CHECK(top->last_bounds == rect{0, 0, 200, 30});
   CHECK(middle->last_bounds == rect{0, 30, 200, 60});
   CHECK(bottom->last_bounds == rect{0, 60, 200, 90});

   // Collapsed: the middle one takes no height, is not drawn, and what
   // follows moves up. The predicate is read when the tile lays out, so
   // lay out again.
   collapsed = true;
   middle->last_bounds = {};
   tv.view_.layout();
   tv.draw();
   CHECK(top->last_bounds == rect{0, 0, 200, 30});
   CHECK(middle->last_bounds == rect{});           // draw was not called
   CHECK(bottom->last_bounds == rect{0, 30, 200, 60});

   // And back.
   collapsed = false;
   tv.view_.layout();
   tv.draw();
   CHECK(bottom->last_bounds == rect{0, 60, 200, 90});
}

TEST_CASE("collapsible page: the limits", "[collapsible_page]")
{
   test_view tv{extent{200, 200}, 1};
   basic_context bctx{tv.view_, tv.cnv};
   bool collapsed = false;

   auto v = vcollapsible(block{});
   v.is_collapsed = [&collapsed]{ return collapsed; };
   CHECK(v.limits(bctx).min == point{60, 30});
   CHECK(v.limits(bctx).max.y == 30);
   collapsed = true;
   CHECK(v.limits(bctx).min == point{60, 0});     // the width is untouched
   CHECK(v.limits(bctx).max.y == 0);
   CHECK(v.limits(bctx).max.x == full_extent);

   collapsed = false;
   auto h = hcollapsible(block{});
   h.is_collapsed = [&collapsed]{ return collapsed; };
   CHECK(h.limits(bctx).min == point{60, 30});
   collapsed = true;
   CHECK(h.limits(bctx).min == point{0, 30});     // the height is untouched
   CHECK(h.limits(bctx).max.x == 0);
   CHECK(h.limits(bctx).max.y == 30);

   // The default predicate: never collapsed.
   auto d = vcollapsible(block{});
   CHECK(d.limits(bctx).min == point{60, 30});

   // Collapsed, the subject takes no mouse and no keyboard.
   auto c = vcollapsible(slider(basic_thumb<20>(), basic_track<5, false>(), 0.5));
   CHECK(c.wants_control());
   c.is_collapsed = []{ return true; };
   CHECK(!c.wants_control());
   CHECK(!c.wants_focus());
}

TEST_CASE("collapsible page: figure", "[collapsible_page]")
{
   // The example, open and collapsed, side by side.
   section open_, closed_;
   closed_.open = false;

   test_view tv{extent{440, 120}, 2};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(
      htile(
         margin({10, 10, 10, 10}, align_top(open_.make())),
         margin({10, 10, 10, 10}, align_top(closed_.make()))
      ),
      box(bkd_color)
   );
   tv.view_.layout();
   tv.draw();
   tv.img.save_png(std::string{RESULTS_PATH} + "layout_collapsible.png");
}
