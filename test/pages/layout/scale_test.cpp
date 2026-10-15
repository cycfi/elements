/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/layout/scale.adoc.
=============================================================================*/
#include "test_support.hpp"
#include <limits>
#include <type_traits>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   // A 40 by 20 element that records the bounds it is drawn with, in its
   // own units and on the canvas, and the last point it was hit tested with.
   struct probe : element
   {
      view_limits limits(basic_context const&) const override
      {
         return {{40, 20}, {40, 20}};
      }

      void draw(context const& ctx) override
      {
         drawn = ctx.bounds;
         on_canvas = {
            ctx.canvas.user_to_device(ctx.bounds.top_left())
          , ctx.canvas.user_to_device(ctx.bounds.bottom_right())
         };
      }

      element* hit_test(context const& ctx, point p, bool leaf, bool control) override
      {
         hit = p;
         return element::hit_test(ctx, p, leaf, control);
      }

      rect drawn = {};
      rect on_canvas = {};
      point hit = {};
   };

   template <typename E>
   void lay_out(test_view& tv, E& e, rect bounds)
   {
      context ctx{tv.view_, tv.cnv, &e, bounds};
      e.limits(ctx);
      e.layout(ctx);
      e.draw(ctx);
   }

   template <typename E>
   view_limits limits_of(test_view& tv, E& e)
   {
      basic_context ctx{tv.view_, tv.cnv};
      return e.limits(ctx);
   }
}

TEST_CASE("scale page: the default scale is 1.0", "[scale_page]")
{
   test_view tv{extent{100, 100}};
   CHECK(tv.view_.scale() == Approx(1.0));
}

TEST_CASE("scale page: the factory sets the scale", "[scale_page]")
{
   auto e = scale(2, probe{});
   CHECK(e.scale() == Approx(2));
   CHECK(std::is_same_v<decltype(e), scale_element<probe>>);
}

TEST_CASE("scale page: a value > 1.0 scales the element up", "[scale_page]")
{
   test_view tv{extent{400, 400}};
   probe p;
   auto e = scale(2, link(p));

   auto l = limits_of(tv, e);
   CHECK(l.min == point{80, 40});
   CHECK(l.max == point{80, 40});

   // The subject keeps its own units; the canvas doubles them.
   lay_out(tv, e, rect{10, 10, 90, 50});
   CHECK(p.drawn.width() == Approx(40));
   CHECK(p.drawn.height() == Approx(20));
   CHECK(p.on_canvas.left == Approx(10));
   CHECK(p.on_canvas.top == Approx(10));
   CHECK(p.on_canvas.width() == Approx(80));
   CHECK(p.on_canvas.height() == Approx(40));
}

TEST_CASE("scale page: a value < 1.0 scales the element down", "[scale_page]")
{
   test_view tv{extent{400, 400}};
   probe p;
   auto e = scale(0.5, link(p));

   auto l = limits_of(tv, e);
   CHECK(l.min == point{20, 10});
   CHECK(l.max == point{20, 10});

   lay_out(tv, e, rect{10, 10, 30, 20});
   CHECK(p.drawn.width() == Approx(40));
   CHECK(p.drawn.height() == Approx(20));
   CHECK(p.on_canvas.left == Approx(10));
   CHECK(p.on_canvas.top == Approx(10));
   CHECK(p.on_canvas.width() == Approx(20));
   CHECK(p.on_canvas.height() == Approx(10));
}

TEST_CASE("scale page: the mutator sets the scale", "[scale_page]")
{
   test_view tv{extent{400, 400}};
   probe p;
   auto e = scale(1, link(p));
   e.scale(3);
   CHECK(e.scale() == Approx(3));
   CHECK(limits_of(tv, e).min == point{120, 60});
}

TEST_CASE("scale page: a scale within a scale", "[scale_page]")
{
   test_view tv{extent{400, 400}};

   probe p;
   auto e = scale(2, scale(2, link(p)));
   CHECK(limits_of(tv, e).min == point{160, 80});

   lay_out(tv, e, rect{0, 0, 160, 80});
   CHECK(p.drawn == rect{0, 0, 40, 20});
   CHECK(p.on_canvas.width() == Approx(160));
   CHECK(p.on_canvas.height() == Approx(80));

   // Offset bounds: the subject is drawn where its scaled bounds are.
   lay_out(tv, e, rect{20, 20, 180, 100});
   CHECK(p.drawn == rect{5, 5, 45, 25});
   CHECK(p.on_canvas.left == Approx(20));
   CHECK(p.on_canvas.top == Approx(20));
   CHECK(p.on_canvas.width() == Approx(160));
   CHECK(p.on_canvas.height() == Approx(80));

   // A hit point maps into the subject's units.
   context ctx{tv.view_, tv.cnv, &e, rect{20, 20, 180, 100}};
   CHECK(e.hit_test(ctx, point{100, 60}, true, false) == &p);
   CHECK(p.hit == point{25, 15});
   CHECK(e.hit_test(ctx, point{170, 90}, true, false) == &p);
   CHECK(p.hit == point{42.5, 22.5});
   CHECK(e.hit_test(ctx, point{190, 60}, true, false) == nullptr);
}

TEST_CASE("scale page: a scale of 0 or less is taken as epsilon", "[scale_page]")
{
   float const eps = std::numeric_limits<float>::epsilon();
   CHECK(scale(0, probe{}).scale() == eps);
   CHECK(scale(-1, probe{}).scale() == eps);

   auto e = scale(1, probe{});
   e.scale(0);
   CHECK(e.scale() == eps);
   e.scale(-2);
   CHECK(e.scale() == eps);

   // The limits stay positive.
   test_view tv{extent{400, 400}};
   auto l = limits_of(tv, e);
   CHECK(l.min.x > 0);
   CHECK(l.min.y > 0);
}
