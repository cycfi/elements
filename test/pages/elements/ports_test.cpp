/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/elements/ports.adoc and
   draws its figures. The figures land in the build's results directory;
   copy them to docs/modules/ROOT/images/elements/ to update the page.
=============================================================================*/
#include "test_support.hpp"
#include <string>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   auto constexpr bkd_color = rgba(35, 35, 37, 255);

   // Content of a fixed size that records where it is laid out.
   struct content : element
   {
      content(point s) : size{s} {}
      view_limits limits(basic_context const&) const override
      {
         return {size, size};
      }
      void layout(context const& ctx) override { bounds = ctx.bounds; }
      void draw(context const& ctx) override
      {
         bounds = ctx.bounds;
         port_bounds = get_port_bounds(ctx);
      }
      point size;
      rect bounds;
      rect port_bounds;
   };

   template <typename P>
   void show(test_view& tv, std::shared_ptr<P> p)
   {
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
      headless::open(tv.view_);
#endif
      tv.view_.content(hold(p));
      tv.view_.layout();
      tv.draw();
   }
}

TEST_CASE("ports page: a port's limits", "[ports_page]")
{
   test_view tv{extent{100, 100}, 1};
   basic_context bctx{tv.view_, tv.cnv};
   auto p = port(content{{400, 300}});
   auto v = vport(content{{400, 300}});
   auto h = hport(content{{400, 300}});
   CHECK(p.limits(bctx).min == point{32, 32});
   CHECK(v.limits(bctx).min == point{400, 32});
   CHECK(h.limits(bctx).min == point{32, 300});
   CHECK(p.limits(bctx).max == point{400, 300});
}

TEST_CASE("ports page: alignment moves the subject", "[ports_page]")
{
   // 400 by 300 in a 100 by 100 port: alignment 0 shows the top left, 1 the
   // bottom right, and between, a part as far along.
   auto p = share(port(content{{400, 300}}));
   test_view tv{extent{100, 100}, 1};
   show(tv, p);
   auto& c = p->actual_subject();
   CHECK(c.bounds == rect{0, 0, 400, 300});

   p->halign(1.0);
   p->valign(0.5);
   tv.draw();
   CHECK(c.bounds.left == Approx(-300));
   CHECK(c.bounds.top == Approx(-100));
   CHECK(c.bounds.width() == 400);              // at its own size
}

TEST_CASE("ports page: get_port_bounds", "[ports_page]")
{
   // From inside a port: the port's bounds, not the subject's.
   auto c = share(content{{400, 300}});
   auto p = share(margin({10, 10, 10, 10}, port(hold(c))));
   test_view tv{extent{100, 100}, 1};
   show(tv, p);
   CHECK(c->port_bounds == rect{10, 10, 90, 90});
}

TEST_CASE("ports page: scroller", "[ports_page]")
{
   test_view tv{extent{100, 100}, 1};
   basic_context bctx{tv.view_, tv.cnv};
   auto s = scroller(content{{400, 300}});
   auto v = vscroller(content{{400, 300}});
   CHECK(s.limits(bctx).min == point{32, 32});
   CHECK(v.limits(bctx).min == point{400, 32});  // no horizontal scroll

   // From code: no callback.
   auto sc = share(scroller(content{{400, 300}}));
   int calls = 0;
   sc->on_scroll = [&](point) { ++calls; };
   show(tv, sc);
   sc->set_alignment({0.5, 0.5});
   CHECK(sc->halign() == 0.5);
   CHECK(calls == 0);

   // From the wheel: the callback, with the alignment.
   context ctx{tv.view_, tv.cnv, sc.get(), rect{0, 0, 100, 100}};
   point got;
   sc->on_scroll = [&](point p) { ++calls; got = p; };
   sc->scroll(ctx, {0, -10}, {50, 50});
   CHECK(calls == 1);
   CHECK(got.y > 0.5);
}

TEST_CASE("ports page: scroll into view", "[ports_page]")
{
   auto sc = share(vscroller(content{{80, 400}}));
   test_view tv{extent{100, 100}, 1};
   show(tv, sc);
   context ctx{tv.view_, tv.cnv, sc.get(), rect{0, 0, 100, 100}};
   CHECK(sc->scroll_into_view(ctx, rect{10, 300, 20, 320}));
   CHECK(sc->valign() > 0);

   // From a descendant: find the scrollable around it.
   auto s = scrollable::find(ctx);
   CHECK(s.scrollable_ptr == sc.get());
}

TEST_CASE("ports page: example", "[ports_page]")
{
   // Two lists of cells, the second scrolled with the first.
   test_view tv{extent{320, 160}, 2};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   auto& view_ = tv.view_;

   auto make_cell = [](std::size_t index)
   {
      auto text = "Item " + std::to_string(index + 1);
      return share(margin({10, 2, 10, 2}, align_left(label(text))));
   };

   auto cells = [&] { return list(basic_cell_composer(1000, make_cell)); };
   auto first = share(vscroller(cells()));
   auto second = share(vscroller(cells()));

   first->on_scroll = [second, &view_](point p)
   {
      second->set_alignment(p);
      view_.refresh(*second);
   };

   view_.content(
      margin({20, 15, 20, 15},
         htile_spaced(20,
            layer(hold(first), frame{}),
            layer(hold(second), frame{})
         )),
      box(bkd_color)
   );
   view_.layout();
   tv.draw();

   // The user scrolls the first; the second follows.
   first->set_alignment({0, 0.25});
   first->on_scroll({0, 0.25});
   CHECK(second->valign() == Approx(0.25));
   view_.layout();
   tv.draw();
   tv.img.save_png(std::string{RESULTS_PATH} + "ports_example.png");
}
