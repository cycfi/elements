/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/support/context.adoc.
=============================================================================*/
#include "test_support.hpp"

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   // An element that can be disabled: the base element is always enabled.
   struct switchable : element
   {
      void enable(bool state) override { _enabled = state; }
      bool is_enabled() const override { return _enabled; }
      bool _enabled = true;
   };

   // The page's example, verbatim apart from recording what the child got.
   class right_half : public element
   {
   public:

      explicit right_half(element& child)
       : _child{child}
      {}

      void draw(context const& ctx) override
      {
         auto r = ctx.bounds;
         r.left = (r.left + r.right) / 2;
         _child.draw(context{ctx, &_child, r});
      }

   private:

      element& _child;
   };

   struct recorder : switchable
   {
      void draw(context const& ctx) override
      {
         bounds = ctx.bounds;
         parent_bounds = ctx.parent? ctx.parent->bounds : rect{};
         enabled = ctx.enabled;
      }

      rect bounds = {};
      rect parent_bounds = {};
      bool enabled = false;
   };
}

TEST_CASE("context page: the constructors", "[context_page]")
{
   test_view tv{extent{200, 100}};
   switchable top, child;

   context root{tv.view_, tv.cnv, &top, rect{0, 0, 200, 100}};
   CHECK(root.parent == nullptr);
   CHECK(root.element == &top);
   CHECK(root.enabled);

   // A child's context points at its parent's.
   context c{root, &child, rect{10, 10, 50, 50}};
   CHECK(c.parent == &root);
   CHECK(c.element == &child);
   CHECK(c.bounds == rect{10, 10, 50, 50});
   CHECK(c.enabled);

   // Enabled only if both the parent context and the element are.
   child.enable(false);
   CHECK_FALSE((context{root, &child, rect{}}.enabled));
   child.enable(true);
   top.enable(false);
   context disabled_root{tv.view_, tv.cnv, &top, rect{}};
   CHECK_FALSE(disabled_root.enabled);
   CHECK_FALSE((context{disabled_root, &child, rect{}}.enabled));

   // A null element is not enabled.
   CHECK_FALSE((context{tv.view_, tv.cnv, nullptr, rect{}}.enabled));

   // Moving the bounds keeps everything else.
   context moved{c, rect{1, 2, 3, 4}};
   CHECK(moved.bounds == rect{1, 2, 3, 4});
   CHECK(moved.element == c.element);
   CHECK(moved.parent == c.parent);
   CHECK(moved.enabled == c.enabled);

   // sub_context: one level down, same bounds.
   auto sub = c.sub_context();
   CHECK(sub.parent == &c);
   CHECK(sub.element == c.element);
   CHECK(sub.bounds == c.bounds);
}

TEST_CASE("context page: view and visible bounds", "[context_page]")
{
   test_view tv{extent{200, 100}};
   basic_context bctx{tv.view_, tv.cnv};

   CHECK(bctx.view_bounds() == rect{0, 0, 200, 100});

   // In the canvas's current user coordinates.
   {
      auto st = tv.cnv.new_state();
      tv.cnv.translate(20, 10);
      CHECK(bctx.view_bounds() == rect{-20, -10, 180, 90});
   }

   // Narrowed by the clip.
   {
      auto st = tv.cnv.new_state();
      tv.cnv.add_rect(rect{50, 20, 120, 60});
      tv.cnv.clip();
      CHECK(bctx.visible_bounds() == rect{50, 20, 120, 60});
   }
}

#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
TEST_CASE("context page: the cursor", "[context_page]")
{
   test_view tv{extent{200, 100}};
   headless::open(tv.view_);
   headless::cursor_pos(tv.view_, point{40, 30});
   basic_context bctx{tv.view_, tv.cnv};
   CHECK(bctx.cursor_pos() == point{40, 30});

   auto st = tv.cnv.new_state();
   tv.cnv.translate(10, 10);
   CHECK(bctx.cursor_pos() == point{30, 20});
}
#endif

TEST_CASE("context page: the example", "[context_page]")
{
   test_view tv{extent{200, 100}};
   recorder child;
   right_half half{child};

   context ctx{tv.view_, tv.cnv, &half, rect{0, 0, 200, 100}};
   half.draw(ctx);
   CHECK(child.bounds == rect{100, 0, 200, 100});
   CHECK(child.parent_bounds == rect{0, 0, 200, 100});
   CHECK(child.enabled);
}
