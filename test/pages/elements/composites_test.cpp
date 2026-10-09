/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/elements/composites.adoc.
=============================================================================*/
#include "test_support.hpp"
#include <string>
#include <vector>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   // An element that records what it was asked.
   struct probe : element
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

      bool wants_control() const override    { return true; }
      bool wants_focus() const override      { return focusable; }

      bool click(context const&, mouse_button btn) override
      {
         clicks.push_back(btn.down);
         return true;
      }

      void drag(context const&, mouse_button) override { ++drags; }

      bool cursor(context const&, point, cursor_tracking status) override
      {
         cursors.push_back(status);
         return false;
      }

      void begin_focus(focus_request) override  { ++focus_begins; }

      bool                          focusable = false;
      rect                          last_bounds = {};
      int                           draws = 0;
      std::vector<bool>             clicks;
      int                           drags = 0;
      std::vector<cursor_tracking>  cursors;
      int                           focus_begins = 0;
   };

   // The page's example: a composite that stacks its children in rows of
   // equal height. Everything else, the dispatch of drawing and events to
   // the children, is composite_base's.
   class rows_element : public composite_base
   {
   public:

      view_limits limits(basic_context const& ctx) const override
      {
         view_limits r{{0, 0}, {full_extent, 0}};
         for (std::size_t i = 0; i != size(); ++i)
         {
            auto l = at(i).limits(ctx);
            r.min.x = std::max(r.min.x, l.min.x);
            r.min.y += l.min.y;
            r.max.y += l.max.y;
         }
         r.max.y = std::min(r.max.y, full_extent);
         return r;
      }

      void layout(context const& ctx) override
      {
         for (std::size_t i = 0; i != size(); ++i)
         {
            context cctx{ctx, &at(i), bounds_of(ctx, i)};
            at(i).layout(cctx);
         }
      }

      rect bounds_of(context const& ctx, std::size_t index) const override
      {
         auto h = ctx.bounds.height() / size();
         auto top = ctx.bounds.top + h * index;
         return {ctx.bounds.left, top, ctx.bounds.right, top + h};
      }
   };

   using rows_composite = vector_composite<rows_element>;

   template <concepts::Element... E>
   auto rows(E&&... elements)
   {
      using composite = array_composite<sizeof...(elements), rows_element>;
      using container = typename composite::container_type;
      composite r{};
      r = container{{share(std::forward<E>(elements))...}};
      return r;
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
}

TEST_CASE("composites page: the example lays out and draws its children", "[composites_page]")
{
   auto a = share(probe{});
   auto b = share(probe{});
   auto c = share(probe{});

   test_view tv{extent{200, 120}, 1};
   show(tv, rows(hold(a), hold(b), hold(c)));

   // bounds_of places them; draw gives each its own context.
   CHECK(a->last_bounds == rect{0, 0, 200, 40});
   CHECK(b->last_bounds == rect{0, 40, 200, 80});
   CHECK(c->last_bounds == rect{0, 80, 200, 120});
   CHECK(a->draws == 1);
}

TEST_CASE("composites page: the factory and the vector composite compile", "[composites_page]")
{
   // The page's second example.
   auto three = rows(label("one"), label("two"), label("three"));
   rows_composite many;
   many.push_back(share(label("more")));
   CHECK(three.size() == 3);
   CHECK(many.size() == 1);
}

TEST_CASE("composites page: storage", "[composites_page]")
{
   auto a = share(probe{});
   auto b = share(probe{});

   // An array composite is fixed; a vector composite grows.
   auto fixed = rows(hold(a), hold(b));
   CHECK(fixed.size() == 2);
   CHECK(&fixed.at(1) == fixed[1].get());

   rows_composite grown;
   CHECK(grown.empty());
   grown.push_back(share(hold(a)));
   grown.push_back(share(hold(b)));
   grown.push_back(share(probe{}));
   CHECK(grown.size() == 3);

   // A range composite presents a slice of another storage.
   range_composite<rows_element> slice{grown, 1, 3};
   CHECK(slice.size() == 2);
   CHECK(&slice.at(0) == &grown.at(1));
}

TEST_CASE("composites page: a click goes to the child under it, and so do the drags and the release", "[composites_page]")
{
   auto a = share(probe{});
   auto b = share(probe{});

   test_view tv{extent{200, 120}, 1};
   show(tv, rows(hold(a), hold(b)));

   tv.view_.click(left_button({100, 30}, true));    // on a
   tv.view_.drag(left_button({100, 90}, true));     // moved over b
   tv.view_.click(left_button({100, 90}, false));   // released over b

   CHECK(a->clicks == std::vector<bool>{true, false});
   CHECK(a->drags == 1);
   CHECK(b->clicks.empty());
   CHECK(b->drags == 0);
}

TEST_CASE("composites page: the cursor enters and leaves each child", "[composites_page]")
{
   auto a = share(probe{});
   auto b = share(probe{});

   test_view tv{extent{200, 120}, 1};
   show(tv, rows(hold(a), hold(b)));

   tv.view_.cursor({100, 30}, cursor_tracking::hovering);
   tv.view_.cursor({100, 35}, cursor_tracking::hovering);
   tv.view_.cursor({100, 90}, cursor_tracking::hovering);

   using ct = cursor_tracking;
   CHECK(a->cursors == std::vector<ct>{ct::entering, ct::hovering, ct::leaving});
   CHECK(b->cursors == std::vector<ct>{ct::entering});
}

TEST_CASE("composites page: focus among the children", "[composites_page]")
{
   auto a = share(probe{});
   auto b = share(probe{});
   auto c = share(probe{});
   a->focusable = true;
   c->focusable = true;
   auto r = share(rows(hold(a), hold(b), hold(c)));

   test_view tv{extent{200, 120}, 1};
   show(tv, hold(r));

   // A click on a child that wants focus focuses it; focus() says which.
   tv.view_.click(left_button({100, 100}, true));   // on c
   tv.view_.click(left_button({100, 100}, false));
   CHECK(r->focus_index() == 2);
   CHECK(r->focus() == &r->at(2));
   CHECK(c->focus_begins == 1);

   // A click on one that does not takes the focus away.
   tv.view_.click(left_button({100, 60}, true));    // on b
   tv.view_.click(left_button({100, 60}, false));
   CHECK(r->focus_index() == -1);
   CHECK(r->focus() == nullptr);

   // Tab walks to the next child that wants focus, skipping b.
   tv.view_.key({key_code::tab, key_action::press, 0});
   CHECK(r->focus_index() == 0);
   tv.view_.key({key_code::tab, key_action::press, 0});
   CHECK(r->focus_index() == 2);
}

TEST_CASE("composites page: hit_element and for_each_visible", "[composites_page]")
{
   auto a = share(probe{});
   auto b = share(probe{});
   auto r = share(rows(hold(a), hold(b)));

   test_view tv{extent{200, 120}, 1};
   show(tv, hold(r));

   tv.view_.in_context_do(*r,
      [&](context const& ctx)
      {
         auto info = r->hit_element(ctx, {100, 90}, true);
         CHECK(info.index == 1);
         CHECK(info.element_ptr == &r->at(1));
         CHECK(info.leaf_element_ptr == b.get());    // through the hold
         CHECK(info.bounds == rect{0, 60, 200, 120});

         auto miss = r->hit_element(ctx, {300, 90}, true);
         CHECK(miss.index == -1);
         CHECK(miss.element_ptr == nullptr);

         std::vector<std::size_t> seen;
         r->for_each_visible(ctx,
            [&](element&, std::size_t ix, rect const&)
            {
               seen.push_back(ix);
               return false;                          // keep going
            }
         );
         CHECK(seen == std::vector<std::size_t>{0, 1});
      }
   );
}
