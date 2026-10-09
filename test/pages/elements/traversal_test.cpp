/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/elements/traversal.adoc.
=============================================================================*/
#include "test_support.hpp"
#include <string>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   struct bounds_probe : element
   {
      view_limits limits(basic_context const&) const override
      {
         return {{20, 20}, {full_extent, full_extent}};
      }

      void draw(context const& ctx) override   { last_bounds = ctx.bounds; }
      bool wants_control() const override      { return true; }

      rect last_bounds = {};
   };

   // The page's first example: a control that closes the popup it sits
   // in, found by walking up from its own context.
   struct closer : element
   {
      bool wants_control() const override { return true; }

      bool click(context const& ctx, mouse_button btn) override
      {
         if (btn.down)
            if (auto* popup = find_parent<basic_popup_element*>(ctx))
               popup->close(ctx.view);
         return true;
      }
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
}

TEST_CASE("traversal page: down, through subjects", "[traversal_page]")
{
   auto probe = share(bounds_probe{});
   auto outer = share(margin({5, 5, 5, 5}, align_center(hold(probe))));

   // find_subject looks down a proxy chain, through indirects; find_element
   // tries the element itself first.
   CHECK(find_subject<bounds_probe*>(outer.get()) == probe.get());
   CHECK(find_element<bounds_probe*>(outer.get()) == probe.get());
   CHECK(find_subject<bounds_probe*>(probe.get()) == nullptr);   // not a proxy
   CHECK(find_element<bounds_probe*>(probe.get()) == probe.get());
   CHECK(find_subject<basic_button*>(outer.get()) == nullptr);

   // A composite's children are not searched.
   auto tile = share(htile(hold(probe)));
   CHECK(find_subject<bounds_probe*>(tile.get()) == nullptr);

   // The page's second example.
   auto lbl = share(margin({10, 10, 10, 10}, label("Before")));
   if (auto* w = find_subject<text_writer_u8*>(lbl.get()))
      w->set_text("After");
   CHECK(find_subject<text_reader_u8*>(lbl.get())->get_text() == "After");
}

TEST_CASE("traversal page: up, through parent contexts", "[traversal_page]")
{
   auto probe = share(bounds_probe{});
   auto outer = share(margin({5, 5, 5, 5}, align_center(hold(probe))));
   auto tile = share(htile(hold(outer)));

   test_view tv{extent{200, 100}, 1};
   show(tv, hold(tile));

   // The contexts live only for the walk, so the checks happen inside it.
   bool ran = false;
   tv.view_.in_context_do(*probe,
      [&](context const& ctx)
      {
         ran = true;
         auto [comp, comp_ctx] = find_composite(ctx);
         CHECK(comp == tile.get());
         REQUIRE(comp_ctx);
         // The context is the one the composite was found in: here that
         // of the indirect holding the tile.
         CHECK(find_element<composite_base*>(comp_ctx->element) == tile.get());

         auto* nearest = find_parent<proxy_base*>(ctx);
         REQUIRE(nearest);
         CHECK(find_subject<bounds_probe*>(nearest) == probe.get());

         auto* nearest_ctx = find_parent_context<proxy_base*>(ctx);
         REQUIRE(nearest_ctx);
         CHECK(nearest_ctx->element == nearest);
      }
   );
   CHECK(ran);
}

TEST_CASE("traversal page: closing the popup from inside", "[traversal_page]")
{
   // The first example, run: the closer's click reaches the popup.
   auto c = share(closer{});
   auto pop = share(basic_popup(hold(c), rect{0, 0, 200, 100}));

   test_view tv{extent{200, 100}, 1};
   show(tv, box(colors::black));
   pop->open(tv.view_);                // a popup is a layer of the view,
   tv.view_.poll();                    // added when the view next polls
   tv.view_.layout();
   tv.draw();
   CHECK(pop->is_open(tv.view_));

   mouse_button btn{};
   btn.down = true;
   btn.state = mouse_button::left;
   btn.num_clicks = 1;
   btn.pos = {100, 50};
   tv.view_.click(btn);
   tv.view_.poll();                    // and removed the same way
   CHECK(!pop->is_open(tv.view_));
}
