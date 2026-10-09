/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/elements/element.adoc and
   draws its figure. The figure case builds the page's Example, clicks it
   the way a user would and saves what it drew, so the picture on the page
   and the code on the page cannot drift apart.

   Figures land in the build's results directory. Copy them to
   docs/modules/ROOT/images/elements/ to update the page.
=============================================================================*/
#include "test_support.hpp"
#include <string>
#include <vector>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   // The page's Example, verbatim.
   class swatch : public element
   {
   public:

      view_limits limits(basic_context const& /* ctx */) const override
      {
         return {{60, 60}, {60, 60}};
      }

      void draw(context const& ctx) override
      {
         auto& cnv = ctx.canvas;
         cnv.fill_style(_on? colors::gold : colors::slate_gray);
         cnv.add_round_rect(ctx.bounds.inset(4), 8);
         cnv.fill();
      }

      bool wants_control() const override
      {
         return true;
      }

      bool click(context const& ctx, mouse_button btn) override
      {
         if (btn.down)
         {
            _on = !_on;
            refresh(ctx);
         }
         return true;
      }

   private:

      bool _on = false;
   };

   // An element that records the events it is given.
   struct event_probe : element
   {
      view_limits limits(basic_context const&) const override
      {
         return {{50, 50}, {50, 50}};
      }

      void draw(context const& ctx) override
      {
         last_bounds = ctx.bounds;
      }

      bool wants_control() const override    { return control; }
      bool wants_focus() const override      { return focusable; }
      bool is_enabled() const override       { return enabled; }

      bool click(context const& ctx, mouse_button btn) override
      {
         clicks.push_back(btn.down);
         ctx_enabled = ctx.enabled;
         return accept;
      }

      void drag(context const&, mouse_button) override
      {
         ++drags;
      }

      bool key(context const&, key_info) override
      {
         ++keys;
         return false;
      }

      bool text(context const&, text_info) override
      {
         ++texts;
         return true;
      }

      void begin_focus(focus_request) override
      {
         ++focus_begins;
      }

      bool end_focus() override
      {
         ++focus_ends;
         return true;
      }

      bool                 control = true;
      bool                 accept = true;
      bool                 focusable = false;
      bool                 enabled = true;

      rect                 last_bounds = {};
      std::vector<bool>    clicks;
      bool                 ctx_enabled = true;
      int                  drags = 0;
      int                  keys = 0;
      int                  texts = 0;
      int                  focus_begins = 0;
      int                  focus_ends = 0;
   };

   mouse_button left_button(point p, bool down)
   {
      mouse_button btn{};
      btn.down = down;
      btn.state = mouse_button::left;
      btn.num_clicks = 1;
      btn.pos = p;
      return btn;
   }

   key_info tab_press()
   {
      return {key_code::tab, key_action::press, 0};
   }

   // Put `content` in a view, draw it once so everything has bounds, and
   // hand the view back.
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

// Expressions: the base class's defaults.
TEST_CASE("element page: the defaults", "[element_page]")
{
   test_view tv;
   element e;
   basic_context bctx{tv.view_, tv.cnv};
   context ctx{tv.view_, tv.cnv, &e, rect{0, 0, 10, 10}};

   auto lim = e.limits(bctx);
   CHECK(lim.min == full_limits.min);
   CHECK(lim.max == full_limits.max);
   CHECK(e.stretch().x == 1.0f);
   CHECK(e.stretch().y == 1.0f);
   CHECK(e.span() == 1);

   CHECK(e.hit_test(ctx, {5, 5}, false, false) == &e);
   CHECK(e.hit_test(ctx, {20, 20}, false, false) == nullptr);

   CHECK_FALSE(e.wants_control());
   CHECK_FALSE(e.click(ctx, left_button({5, 5}, true)));
   CHECK_FALSE(e.key(ctx, tab_press()));
   CHECK_FALSE(e.text(ctx, {'a', 0}));
   CHECK_FALSE(e.cursor(ctx, {5, 5}, cursor_tracking::hovering));
   CHECK_FALSE(e.scroll(ctx, {0, 1}, {5, 5}));
   CHECK_FALSE(e.drop(ctx, drop_info{}));

   // The base keeps no enabled state.
   e.enable(false);
   CHECK(e.is_enabled());

   CHECK_FALSE(e.wants_focus());
   CHECK(e.end_focus());
   CHECK(e.focus() == &e);
   CHECK(std::as_const(e).focus() == &e);

   CHECK(e.class_name().find("element") != std::string::npos);

   // empty() is a default element: no size of its own, no events.
   auto nothing = empty();
   CHECK(nothing.limits(bctx).max == full_limits.max);
   CHECK_FALSE(nothing.wants_control());
}

// Expressions, Control: a composite hands a click only to an element that
// wants control.
TEST_CASE("element page: a click reaches only a control", "[element_page]")
{
   auto a = share(event_probe{});
   auto b = share(event_probe{});
   a->control = false;

   test_view tv;
   show(tv, htile(hold(a), hold(b)));

   tv.view_.click(left_button(center_point(a->last_bounds), true));
   tv.view_.click(left_button(center_point(a->last_bounds), false));
   CHECK(a->clicks.empty());

   tv.view_.click(left_button(center_point(b->last_bounds), true));
   tv.view_.click(left_button(center_point(b->last_bounds), false));
   CHECK(b->clicks == std::vector<bool>{true, false});
}

// Expressions, Control: the element that takes the press gets the drags and
// the release, wherever the cursor has gone.
TEST_CASE("element page: the press decides who gets drag and release", "[element_page]")
{
   auto a = share(event_probe{});
   auto b = share(event_probe{});

   test_view tv;
   show(tv, htile(hold(a), hold(b)));

   auto on_a = center_point(a->last_bounds);
   auto on_b = center_point(b->last_bounds);

   tv.view_.click(left_button(on_a, true));
   tv.view_.drag(left_button(on_b, true));
   tv.view_.click(left_button(on_b, false));

   CHECK(a->clicks == std::vector<bool>{true, false});
   CHECK(a->drags == 1);
   CHECK(b->clicks.empty());
   CHECK(b->drags == 0);

   // Decline the press and nothing follows it.
   a->clicks.clear();
   a->drags = 0;
   a->accept = false;
   tv.view_.click(left_button(on_a, true));
   tv.view_.drag(left_button(on_a, true));
   tv.view_.click(left_button(on_a, false));
   CHECK(a->clicks == std::vector<bool>{true});
   CHECK(a->drags == 0);
}

// Expressions, Focus: key and text go to a composite's focus, and tab moves
// it to the next child that wants it.
TEST_CASE("element page: tab moves the focus, text goes to it", "[element_page]")
{
   auto a = share(event_probe{});
   auto b = share(event_probe{});
   a->focusable = true;
   b->focusable = true;
   auto h = share(htile(hold(a), hold(b)));

   test_view tv;
   show(tv, hold(h));

   // The events go straight to the tile, in the tile's own context.
   rect bounds = {
      a->last_bounds.left, a->last_bounds.top
    , b->last_bounds.right, b->last_bounds.bottom
   };
   context ctx{tv.view_, tv.cnv, h.get(), bounds};

   CHECK(h->key(ctx, tab_press()));
   CHECK(a->focus_begins == 1);
   CHECK(h->focus() != nullptr);

   CHECK(h->text(ctx, {'x', 0}));
   CHECK(a->texts == 1);
   CHECK(b->texts == 0);

   CHECK(h->key(ctx, tab_press()));
   CHECK(a->keys == 1);             // the focus saw the tab first
   CHECK(a->focus_ends == 1);
   CHECK(b->focus_begins == 1);

   CHECK(h->text(ctx, {'y', 0}));
   CHECK(b->texts == 1);
}

#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)

// Expressions, Focus: tab from the view's top focuses the first child that
// wants focus, then the next; shift-tab goes back.
TEST_CASE("element page: tab through the view", "[element_page]")
{
   auto a = share(event_probe{});
   auto b = share(event_probe{});
   a->focusable = true;
   b->focusable = true;

   test_view tv;
   show(tv, htile(hold(a), hold(b)));

   tv.view_.key(tab_press());
   CHECK(a->focus_begins == 1);
   CHECK(b->focus_begins == 0);

   tv.view_.key(tab_press());
   CHECK(a->focus_ends == 1);
   CHECK(b->focus_begins == 1);

   tv.view_.key({key_code::tab, key_action::press, mod_shift});
   CHECK(b->focus_ends == 1);
   CHECK(a->focus_begins == 2);
}

#endif

// Expressions, Enabling: a disabled element is still sent events; the
// context says it is disabled and the element decides.
TEST_CASE("element page: disabling is read from the context", "[element_page]")
{
   auto a = share(event_probe{});
   a->enabled = false;

   test_view tv;
   show(tv, htile(hold(a), hold(share(event_probe{}))));

   tv.view_.click(left_button(center_point(a->last_bounds), true));
   CHECK(a->clicks == std::vector<bool>{true});
   CHECK_FALSE(a->ctx_enabled);
}

#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)

// Expressions, Drawing and Refresh: outward counts enclosing elements.
TEST_CASE("element page: refresh outward reaches the enclosing element", "[element_page]")
{
   auto a = share(event_probe{});

   test_view tv;
   show(tv, margin({10, 10, 10, 10}, hold(a)));
   headless::take_invalidated(tv.view_);

   tv.view_.in_context_do(*a,
      [&](context const& ctx)
      {
         a->refresh(ctx);
         a->refresh(ctx, 1);
      }
   );
   tv.view_.poll();
   auto areas = headless::take_invalidated(tv.view_);

   REQUIRE(areas.size() == 2);
   CHECK(areas[0] == a->last_bounds);
   CHECK(areas[1] == a->last_bounds.inset(-10));
}

// Expressions, Drawing and Refresh: in_context_do hands f the context of
// the element sought, a composite's direct child included.
TEST_CASE("element page: in_context_do on a composite's child", "[element_page]")
{
   auto h = share(htile(event_probe{}, event_probe{}));
   auto& b = h->at(1);

   test_view tv;
   show(tv, hold(h));
   rect b_bounds = static_cast<event_probe&>(b).last_bounds;

   element* given = nullptr;
   rect bounds = {};
   tv.view_.in_context_do(b,
      [&](context const& ctx)
      {
         given = ctx.element;
         bounds = ctx.bounds;
      }
   );
   CHECK(given == &b);
   CHECK(bounds == b_bounds);
}

#endif

// Example: three swatches, the middle one clicked.
TEST_CASE("element page: example figure", "[element_page]")
{
   auto constexpr bkd_color = rgba(35, 35, 37, 255);

   test_view tv{extent{280, 100}, 4};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(
      align_center_middle(htile(swatch{}, swatch{}, swatch{})),
      box(bkd_color)
   );
   tv.view_.layout();
   tv.draw();

   tv.view_.click(left_button({140, 50}, true));
   tv.view_.click(left_button({140, 50}, false));
   tv.draw();
   tv.img.save_png(std::string{RESULTS_PATH} + "swatches.png");
}
