/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/layout/flow.adoc.
=============================================================================*/
#include "test_support.hpp"
#include <type_traits>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   // An element of the given limits that records the bounds it is drawn
   // with.
   struct probe : element
   {
      probe(view_limits l = full_limits) : lim(l) {}

      view_limits limits(basic_context const&) const override { return lim; }
      void draw(context const& ctx) override { drawn = ctx.bounds; }

      view_limits lim;
      rect drawn = {};
   };

   template <typename E>
   void lay_out(test_view& tv, E& e, rect bounds)
   {
      context ctx{tv.view_, tv.cnv, &e, bounds};
      e.layout(ctx);
      e.draw(ctx);
   }

   template <typename E>
   view_limits limits_of(test_view& tv, E& e)
   {
      basic_context ctx{tv.view_, tv.cnv};
      return e.limits(ctx);
   }

   view_limits const fixed_40x20 = {{40, 20}, {40, 20}};
}

TEST_CASE("flow page: the factory, the element and the composite", "[flow_page]")
{
   flow_composite c;
   auto f = flow(c);
   CHECK(std::is_same_v<decltype(f), flow_element>);
   CHECK(std::is_base_of_v<vector_composite<vtile_element>, flow_element>);
   CHECK(std::is_base_of_v<composite_base, flow_element>);
   CHECK(std::is_same_v<flow_composite, vector_composite<flowable_container>>);
   CHECK(std::is_base_of_v<storage, flow_composite>);
   CHECK(std::is_base_of_v<std::vector<element_ptr>, flow_composite>);

   // Used just as a std::vector of element_ptr.
   probe a{fixed_40x20};
   c.push_back(share(a));
   CHECK(c.size() == 1);
   c.push_back(share(link(a)));
   CHECK(c.size() == 2);
   c.pop_back();
   CHECK(c.size() == 1);
}

TEST_CASE("flow page: left to right, then the next row", "[flow_page]")
{
   test_view tv{extent{400, 400}};
   probe a{fixed_40x20}, b{fixed_40x20}, c{fixed_40x20}
     , d{fixed_40x20}, e{fixed_40x20};

   flow_composite items;
   for (auto p : {&a, &b, &c, &d, &e})
      items.push_back(share(link(*p)));
   auto f = flow(items);

   // Two in a row of 100: a and b, then c and d, then e.
   lay_out(tv, f, rect{10, 20, 110, 80});
   CHECK(f.size() == 3);
   CHECK(a.drawn == rect{10, 20, 50, 40});
   CHECK(b.drawn == rect{50, 20, 90, 40});
   CHECK(c.drawn == rect{10, 40, 50, 60});
   CHECK(d.drawn == rect{50, 40, 90, 60});
   CHECK(e.drawn == rect{10, 60, 50, 80});
}

TEST_CASE("flow page: re-flowed when the size changes", "[flow_page]")
{
   test_view tv{extent{400, 400}};
   probe a{fixed_40x20}, b{fixed_40x20}, c{fixed_40x20};

   flow_composite items;
   for (auto p : {&a, &b, &c})
      items.push_back(share(link(*p)));
   auto f = flow(items);

   lay_out(tv, f, rect{0, 0, 120, 60});
   CHECK(f.size() == 1);
   CHECK(c.drawn == rect{80, 0, 120, 20});

   lay_out(tv, f, rect{0, 0, 80, 60});
   CHECK(f.size() == 2);
   CHECK(c.drawn == rect{0, 20, 40, 40});
}

TEST_CASE("flow page: reflow, needs_reflow and reflow_done", "[flow_page]")
{
   test_view tv{extent{400, 400}};
   probe a{fixed_40x20}, b{{{60, 30}, {60, 30}}};

   flow_composite items;
   items.push_back(share(link(a)));
   items.push_back(share(link(b)));
   auto f = flow(items);

   // Marked initially; open limits until laid out.
   CHECK(items.needs_reflow());
   auto open = limits_of(tv, f);
   CHECK(open.min.x == 0);
   CHECK(open.min.y == 0);
   CHECK(open.max.x == full_extent);
   CHECK(open.max.y == full_extent);

   // Laid out in 80: a, then b, in two rows. The layout clears the mark.
   lay_out(tv, f, rect{0, 0, 80, 100});
   CHECK_FALSE(items.needs_reflow());
   auto l = limits_of(tv, f);
   CHECK(l.min.x == 60);                     // The widest element.
   CHECK(l.min.y == 50);                     // The rows: 20 + 30.
   CHECK(l.max.x == full_extent);
   CHECK(l.max.y == full_extent);

   items.reflow();
   CHECK(items.needs_reflow());
   CHECK(limits_of(tv, f).min.y == 0);

   items.reflow_done();
   CHECK_FALSE(items.needs_reflow());
   CHECK(limits_of(tv, f).min.y == 50);
}

TEST_CASE("flow page: an element wider than the flow", "[flow_page]")
{
   test_view tv{extent{400, 400}};
   probe a{{{150, 20}, {150, 20}}}, b{fixed_40x20};

   flow_composite items;
   items.push_back(share(link(a)));
   items.push_back(share(link(b)));
   auto f = flow(items);

   // a takes a row of its own, at the top, past the flow's end; b is next.
   lay_out(tv, f, rect{0, 0, 100, 100});
   CHECK(f.size() == 2);
   CHECK(a.drawn == rect{0, 0, 150, 20});
   CHECK(b.drawn == rect{0, 20, 40, 40});
}

TEST_CASE("flow page: elements removed or added without reflow", "[flow_page]")
{
   test_view tv{extent{400, 400}};
   probe a{fixed_40x20}, b{fixed_40x20}, c{fixed_40x20}
     , d{fixed_40x20}, e{fixed_40x20};

   flow_composite items;
   for (auto p : {&a, &b, &c, &d})
      items.push_back(share(link(*p)));
   auto f = flow(items);
   lay_out(tv, f, rect{0, 0, 100, 100});
   CHECK(f.size() == 2);
   CHECK(limits_of(tv, f).min.y == 40);

   SECTION("removed")
   {
      items.pop_back();
      items.pop_back();
      CHECK_FALSE(items.needs_reflow());

      // The rows are stale: no rows, open limits, nothing drawn.
      CHECK(f.size() == 0);
      CHECK(limits_of(tv, f).min.y == 0);
      a.drawn = {};
      {
         context ctx{tv.view_, tv.cnv, &f, rect{0, 0, 100, 100}};
         f.draw(ctx);
      }
      CHECK(a.drawn == rect{});

      // The next layout builds the rows anew.
      lay_out(tv, f, rect{0, 0, 100, 100});
      CHECK(f.size() == 1);
      CHECK(a.drawn == rect{0, 0, 40, 20});
      CHECK(b.drawn == rect{40, 0, 80, 20});
      CHECK(limits_of(tv, f).min.y == 20);
   }

   SECTION("added")
   {
      items.push_back(share(link(e)));
      CHECK_FALSE(items.needs_reflow());
      CHECK(f.size() == 0);
      CHECK(limits_of(tv, f).min.y == 0);

      lay_out(tv, f, rect{0, 0, 100, 100});
      CHECK(f.size() == 3);
      CHECK(e.drawn == rect{0, 40, 40, 60});
      CHECK(limits_of(tv, f).min.y == 60);
   }
}
