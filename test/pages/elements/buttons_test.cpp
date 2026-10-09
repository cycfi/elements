/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/elements/buttons.adoc and
   draws its figure. The figure lands in the build's results directory;
   copy it to docs/modules/ROOT/images/elements/ to update the page.
=============================================================================*/
#include "test_support.hpp"
#include <string>
#include <vector>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   auto constexpr bkd_color = rgba(35, 35, 37, 255);

   // The page's example: a styler drawn from the button's state.
   struct plain_styler : button_styler_base
   {
      view_limits limits(basic_context const&) const override
      {
         return {{80, 24}, {full_extent, 24}};
      }

      void draw(context const& ctx) override
      {
         auto* btn = find_parent<basic_button*>(ctx);
         if (!btn)
            return;
         auto& cnv = ctx.canvas;
         auto body = btn->value() ? colors::gold : colors::dim_gray;
         if (btn->hilite())
            body = body.level(1.2);
         if (!ctx.enabled)
            body = body.opacity(0.4);
         cnv.fill_style(body);
         cnv.add_round_rect(ctx.bounds, 6);
         cnv.fill();
      }
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

   // A button filling a view, with its clicks and the view's tracking
   // reports logged.
   template <typename Button>
   struct button_view : test_view
   {
      explicit button_view(Button b)
       : test_view{{200, 100}}
       , button{share(std::move(b))}
      {
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
         headless::open(view_);
#endif
         view_.content(hold(button));
         view_.layout();
         draw();
         button->on_click = [this](bool v) { clicks.push_back(v); };
         view_.on_tracking = [this](element&, element::tracking s)
         {
            tracking.push_back(s == element::begin_tracking ? "begin"
                             : s == element::end_tracking ? "end" : "while");
         };
      }

      // The styler is 24 tall and sits at the top of the view.
      point inside() const  { return {100, 12}; }
      point outside() const { return {300, 300}; }

      void press(point p)   { view_.click(left_button(p, true)); }
      void release(point p) { view_.click(left_button(p, false)); }
      void drag(point p)    { view_.drag(left_button(p, true)); }

      std::shared_ptr<Button>    button;
      std::vector<bool>          clicks;
      std::vector<std::string>   tracking;
   };
}

TEST_CASE("buttons page: a momentary button", "[buttons_page]")
{
   button_view v{momentary_button(plain_styler{})};
   CHECK(!v.button->value());

   v.press(v.inside());
   CHECK(v.button->value());               // pressed while the mouse is down
   CHECK(v.button->tracking());
   v.release(v.inside());
   CHECK(!v.button->value());              // and only then
   CHECK(v.clicks == std::vector<bool>{true});
   CHECK(v.tracking == std::vector<std::string>{"begin", "end"});

   // Dragging off and back changes the value with the pointer; releasing
   // outside cancels the click.
   v.press(v.inside());
   v.drag(v.outside());
   CHECK(!v.button->value());
   v.drag(v.inside());
   CHECK(v.button->value());
   v.drag(v.outside());
   v.release(v.outside());
   CHECK(v.clicks == std::vector<bool>{true});
   CHECK(!v.button->tracking());
}

TEST_CASE("buttons page: a toggle button", "[buttons_page]")
{
   button_view v{toggle_button(plain_styler{})};

   v.press(v.inside());
   CHECK(v.button->value());               // flips on the press
   v.release(v.inside());
   CHECK(v.button->value());               // and stays
   CHECK(v.clicks == std::vector<bool>{true});

   v.press(v.inside());
   v.release(v.inside());
   CHECK(!v.button->value());
   CHECK(v.clicks == std::vector<bool>{true, false});

   // Dragging off shows the old state again; releasing outside cancels.
   v.press(v.inside());
   CHECK(v.button->value());
   v.drag(v.outside());
   CHECK(!v.button->value());
   v.release(v.outside());
   CHECK(!v.button->value());
   CHECK(v.clicks == std::vector<bool>{true, false});
}

TEST_CASE("buttons page: a latching button", "[buttons_page]")
{
   button_view v{latching_button(plain_styler{})};

   v.press(v.inside());
   v.release(v.inside());
   CHECK(v.button->value());
   CHECK(v.clicks == std::vector<bool>{true});

   // Latched, it takes no further clicks.
   v.press(v.inside());
   v.release(v.inside());
   CHECK(v.button->value());
   CHECK(v.clicks == std::vector<bool>{true});

   // Code releases the latch.
   v.button->value(false);
   v.press(v.inside());
   v.release(v.inside());
   CHECK(v.clicks == std::vector<bool>{true, true});
}

TEST_CASE("buttons page: choices in a group", "[buttons_page]")
{
   auto a = share(choice(plain_styler{}));
   auto b = share(choice(plain_styler{}));
   auto c = share(choice(plain_styler{}));
   a->select(true);

   test_view tv{extent{200, 100}, 1};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(vtile(hold(a), hold(b), hold(c)));
   tv.view_.layout();
   tv.draw();

   // Clicking b (the second 24 pixel row) selects it and deselects a; the
   // composite they share is the group.
   tv.view_.click(left_button({100, 36}, true));
   tv.view_.click(left_button({100, 36}, false));
   CHECK(!a->is_selected());
   CHECK(b->is_selected());
   CHECK(!c->is_selected());

   // Clicking the selected one again changes nothing.
   tv.view_.click(left_button({100, 36}, true));
   tv.view_.click(left_button({100, 36}, false));
   CHECK(b->is_selected());
}

TEST_CASE("buttons page: hilite, enable, edit", "[buttons_page]")
{
   button_view v{toggle_button(plain_styler{})};

   // The cursor over the button highlights it.
   v.view_.cursor(v.inside(), cursor_tracking::hovering);
   CHECK(v.button->hilite());
   v.view_.cursor(v.outside(), cursor_tracking::hovering);
   CHECK(!v.button->hilite());

   // Disabled, it ignores the mouse.
   v.button->enable(false);
   CHECK(!v.button->is_enabled());
   v.press(v.inside());
   v.release(v.inside());
   CHECK(!v.button->value());
   CHECK(v.clicks.empty());
   v.button->enable(true);

   // edit sets the value as the user would, and calls on_click.
   v.button->edit(v.view_, true);
   CHECK(v.button->value());
   CHECK(v.clicks == std::vector<bool>{true});

   // value sets it quietly.
   v.button->value(false);
   CHECK(!v.button->value());
   CHECK(v.clicks == std::vector<bool>{true});
}

TEST_CASE("buttons page: figure", "[buttons_page]")
{
   // The four kinds, in the library's default style.
   auto momentary = button("Momentary");
   auto toggle = toggle_button("Toggle", 1.0, colors::green.level(0.7).opacity(0.4));
   auto latching = latching_button("Latching", 1.0, colors::royal_blue.opacity(0.4));
   auto choice_a = radio_button("Choice A");
   auto choice_b = radio_button("Choice B");
   toggle.value(true);
   latching.value(true);
   choice_a.select(true);

   test_view tv{extent{200, 200}, 2};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(
      margin({20, 16, 20, 16},
         vtile_spaced(10,
            momentary, toggle, latching,
            align_left(choice_a), align_left(choice_b)
         )
      ),
      box(bkd_color)
   );
   tv.view_.layout();
   tv.draw();
   tv.img.save_png(std::string{RESULTS_PATH} + "elements_buttons.png");
}

TEST_CASE("buttons page: example figure", "[buttons_page]")
{
   // The page's example rendered: the plain styler as a toggle, off and
   // on, the on one highlighted as if under the cursor.
   auto off = share(toggle_button(plain_styler{}));
   auto on = share(toggle_button(plain_styler{}));
   on->value(true);
   test_view tv{extent{220, 60}, 2};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(
      margin({10, 10, 10, 10},
         htile(margin_right(10, align_middle(hold(off))), align_middle(hold(on)))),
      box(bkd_color)
   );
   tv.view_.layout();
   tv.view_.cursor({165, 30}, cursor_tracking::hovering);
   tv.draw();
   tv.img.save_png(std::string{RESULTS_PATH} + "buttons_example.png");
}
