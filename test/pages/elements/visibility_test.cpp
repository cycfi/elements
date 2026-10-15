/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/elements/visibility.adoc.
=============================================================================*/
#include "test_support.hpp"
#include <string>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   void open(test_view& tv)
   {
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
      headless::open(tv.view_);
#endif
   }

   // Records the composite it finds from the context of a key.
   struct finder : element
   {
      bool wants_focus() const override { return true; }
      bool wants_control() const override { return true; }
      bool click(context const&, mouse_button) override { return true; }
      bool key(context const& ctx, key_info) override
      {
         ++keys;
         found = find_composite(ctx).first;
         return true;
      }
      composite_base* found = nullptr;
      int keys = 0;
   };
}

TEST_CASE("visibility page: intercepts", "[visibility_page]")
{
   test_view tv{extent{200, 200}};
   open(tv);
   auto f = share(finder{});
   auto tile = share(vtile(hold(f)));
   auto ki = share(key_intercept(hold(tile)));
   tv.view_.content(hold(ki));
   tv.view_.layout();
   tv.draw();

   // Click it for the focus, then a key: from the key's context, it finds
   // its tile.
   mouse_button btn{};
   btn.state = mouse_button::left;
   btn.down = true;
   btn.num_clicks = 1;
   btn.pos = {100, 100};
   tv.view_.click(btn);
   btn.down = false;
   tv.view_.click(btn);
   tv.view_.key(key_info{key_code::a, key_action::press, 0});
   CHECK(f->keys == 1);
   CHECK(f->found == tile.get());
}

TEST_CASE("visibility page: hidable", "[visibility_page]")
{
   test_view tv{extent{200, 100}};
   open(tv);
   int clicks = 0;
   auto b = share(momentary_button(box(colors::red)));
   b->on_click = [&](bool) { ++clicks; };
   auto h = share(hidable(hold(b)));
   auto other = share(box(colors::blue));
   tv.view_.content(htile(hsize(100, hold(h)), hold(other)));
   tv.view_.layout();
   tv.draw();
   basic_context bctx{tv.view_, tv.cnv};

   mouse_button btn{};
   btn.state = mouse_button::left;
   btn.num_clicks = 1;
   btn.pos = {50, 50};
   auto click = [&]
   {
      btn.down = true;
      tv.view_.click(btn);
      btn.down = false;
      tv.view_.click(btn);
   };

   click();
   CHECK(clicks == 1);

   // Hidden: the same limits, so the same space, and no clicks.
   auto lim = h->limits(bctx);
   h->is_hidden = true;
   CHECK(h->limits(bctx).min == lim.min);
   CHECK(!h->wants_control());
   CHECK(!h->wants_focus());
   click();
   CHECK(clicks == 1);

   // Shown again.
   h->is_hidden = false;
   click();
   CHECK(clicks == 2);
}

TEST_CASE("visibility page: text intercept", "[visibility_page]")
{
   test_view tv{extent{200, 100}};
   open(tv);

   // The page's snippet: a text box that takes digits only.
   auto [box_, text] = input_box("");
   auto digits_only = text_intercept(box_);
   digits_only.on_text = [](text_info info)
   {
      return info.codepoint < '0' || info.codepoint > '9';   // take, and drop, the rest
   };

   tv.view_.content(digits_only);
   tv.view_.layout();
   tv.draw();
   mouse_button btn{};
   btn.state = mouse_button::left;
   btn.num_clicks = 1;
   btn.pos = {50, 10};
   btn.down = true;
   tv.view_.click(btn);
   btn.down = false;
   tv.view_.click(btn);
   for (char c : std::string{"a1b2"})
      tv.view_.text(text_info{std::uint32_t(c), 0});
   CHECK(text->get_text() == U"12");
}

TEST_CASE("visibility page: click intercept", "[visibility_page]")
{
   test_view tv{extent{200, 100}};
   open(tv);
   int clicks = 0, seen = 0;
   auto b = share(momentary_button(box(colors::red)));
   b->on_click = [&](bool) { ++clicks; };
   auto ci = share(click_intercept(hold(b)));
   bool take = true;
   ci->on_click = [&](mouse_button) { ++seen; return take; };
   tv.view_.content(hold(ci));
   tv.view_.layout();
   tv.draw();
   mouse_button btn{};
   btn.state = mouse_button::left;
   btn.num_clicks = 1;
   btn.pos = {50, 50};
   auto click = [&]
   {
      btn.down = true;
      tv.view_.click(btn);
      btn.down = false;
      tv.view_.click(btn);
   };

   // Taken: the button sees nothing.
   click();
   CHECK(seen >= 1);
   CHECK(clicks == 0);

   // Passed on.
   take = false;
   click();
   CHECK(clicks == 1);

   // A focus stop only if its subject is.
   CHECK(!click_intercept(box(colors::red)).wants_focus());
}
