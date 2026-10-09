/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/elements/proxies.adoc.
=============================================================================*/
#include "test_support.hpp"
#include <string>
#include <vector>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   // An element that records the bounds it was drawn with.
   struct bounds_probe : element
   {
      view_limits limits(basic_context const&) const override
      {
         return {{20, 20}, {full_extent, full_extent}};
      }

      void draw(context const& ctx) override
      {
         last_bounds = ctx.bounds;
         ++draws;
      }

      bool wants_control() const override { return true; }

      rect  last_bounds = {};
      int   draws = 0;
   };

   // The page's example: a proxy that insets its subject by a fixed
   // amount, and counts the hooks.
   template <typename Subject>
   struct inset_by : proxy<Subject>
   {
      using base_type = proxy<Subject>;

      inset_by(Subject subject, float amount)
       : base_type{std::move(subject)}, _amount{amount}
      {}

      view_limits limits(basic_context const& ctx) const override
      {
         auto l = this->subject().limits(ctx);
         l.min.x += 2 * _amount; l.min.y += 2 * _amount;
         return l;
      }

      void prepare_subject(context& ctx) override
      {
         ctx.bounds = ctx.bounds.inset(_amount, _amount);
         ++prepares;
      }

      void restore_subject(context& /*ctx*/) override
      {
         ++restores;
      }

      float _amount;
      int   prepares = 0;
      int   restores = 0;
   };

   template <typename Content>
   void show(test_view& tv, Content&& content)
   {
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
      headless::open(tv.view_);
#endif
      tv.view_.content(std::forward<Content>(content));
      tv.view_.layout();
      tv.draw();
   }

   mouse_button left_button(point p, bool down)
   {
      mouse_button btn{};
      btn.down = down;
      btn.state = mouse_button::left;
      btn.num_clicks = 1;
      btn.pos = p;
      return btn;
   }
}

TEST_CASE("proxies page: a proxy forwards to its subject", "[proxies_page]")
{
   // The page's example: the subject is drawn inset, and each forwarded
   // call is bracketed by prepare_subject and restore_subject.
   auto p = share(inset_by<bounds_probe>{bounds_probe{}, 10});

   test_view tv{extent{200, 100}, 1};
   show(tv, hold(p));

   auto& probe = p->actual_subject();
   CHECK(probe.draws == 1);
   CHECK(probe.last_bounds == rect{10, 10, 190, 90});
   CHECK(p->prepares >= 1);
   CHECK(p->restores == p->prepares);

   // limits, stretch and span come from the subject, through the proxy.
   basic_context bctx{tv.view_, tv.cnv};
   CHECK(p->limits(bctx).min == point{40, 40});
   CHECK(p->stretch().x == probe.stretch().x);
   CHECK(p->span() == probe.span());

   // Events too: the click lands on the subject with the inset bounds.
   auto before = p->prepares;
   tv.view_.click(left_button({100, 50}, true));
   tv.view_.click(left_button({100, 50}, false));
   CHECK(p->prepares > before);
   CHECK(p->restores == p->prepares);
}

TEST_CASE("proxies page: margin is a proxy", "[proxies_page]")
{
   // The library's own proxies do the same: margin insets the bounds.
   auto probe = share(bounds_probe{});
   test_view tv{extent{200, 100}, 1};
   show(tv, margin({10, 20, 30, 40}, hold(probe)));
   CHECK(probe->last_bounds == rect{10, 20, 170, 60});
}
