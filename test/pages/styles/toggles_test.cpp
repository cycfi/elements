/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/styles/toggles.adoc and
   draws its figure. The figure lands in the build's results directory;
   copy it to docs/modules/ROOT/images/styles/ to update the page.
=============================================================================*/
#include "test_support.hpp"
#include <elements/element/style/check_box.hpp>
#include <elements/element/style/radio_button.hpp>
#include <elements/element/style/slide_switch.hpp>
#include <string>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   auto constexpr bkd_color = rgba(35, 35, 37, 255);

   // The page's toggle selector example.
   struct star_styler : toggle_selector
   {
      using toggle_selector::toggle_selector;

      void draw(context const& ctx) override
      {
         auto btn = find_parent<basic_button*>(ctx);
         if (!btn)
            return;

         auto& cnv = ctx.canvas;
         auto const& thm = get_theme();
         rect box = ctx.bounds.move(15, 0);
         box.width(box.height());

         auto on = btn->value();
         draw_icon(cnv, box, on? icons::star : icons::star_empty, 16,
            on? colors::gold : thm.label_font_color);

         cnv.fill_style(thm.label_font_color);
         cnv.font(thm.label_font);
         cnv.text_align(cnv.left | cnv.middle);
         cnv.fill_text(_text, point{box.right + 10, center_point(ctx.bounds).y});
      }
   };

   void open(test_view& tv)
   {
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
      headless::open(tv.view_);
#endif
   }

   void click(view& v, point p)
   {
      mouse_button b{};
      b.state = mouse_button::left;
      b.num_clicks = 1;
      b.pos = p;
      b.down = true;
      v.click(b);
      b.down = false;
      v.click(b);
   }
}

TEST_CASE("toggles page: behavior", "[toggles_page]")
{
   test_view tv{extent{300, 300}};
   open(tv);
   auto cb = share(check_box("Check"));
   auto r1 = share(radio_button("One"));
   auto r2 = share(radio_button("Two"));
   auto sw = share(slide_switch());
   r1->select(true);
   tv.view_.content(
      align_left_top(
         vtile(
            hold(cb),
            align_left(hold(r1)),
            align_left(hold(r2)),
            align_left(hold(sw))
         )
      )
   );
   tv.view_.layout();
   tv.draw();
   basic_context bctx{tv.view_, tv.cnv};
   auto h = [&](auto const& e) { return e->limits(bctx).min.y; };

   // A check box is a toggle: each click flips it.
   click(tv.view_, {20, h(cb) / 2});
   CHECK(cb->value());
   click(tv.view_, {20, h(cb) / 2});
   CHECK(!cb->value());

   // Radio buttons: selecting one deselects the others.
   auto y2 = h(cb) + h(r1) + h(r2) / 2;
   click(tv.view_, {20, y2});
   CHECK(r2->is_selected());
   CHECK(!r1->is_selected());

   // A radio button stays on when clicked again.
   click(tv.view_, {20, y2});
   CHECK(r2->is_selected());

   // The selectors are as large as their text, and no larger.
   auto l = cb->limits(bctx);
   CHECK(l.min == l.max);

   // A slide switch is the theme's slide_button_size.
   CHECK(sw->limits(bctx).min == get_theme().slide_button_size);
}

TEST_CASE("toggles page: radio groups", "[toggles_page]")
{
   // The nearest composite is the group: a radio button in a nested tile
   // is not in the outer tile's group.
   test_view tv{extent{300, 300}};
   open(tv);
   auto r1 = share(radio_button("One"));
   auto r2 = share(radio_button("Two"));
   auto r3 = share(radio_button("Three"));
   r1->select(true);
   r3->select(true);
   tv.view_.content(
      align_left_top(
         vtile(
            align_left(hold(r1)),
            align_left(hold(r2)),
            htile(hold(r3), label("Info"))
         )
      )
   );
   tv.view_.layout();
   tv.draw();
   basic_context bctx{tv.view_, tv.cnv};
   auto h = r1->limits(bctx).min.y;
   click(tv.view_, {20, h + h / 2});      // Two
   CHECK(r2->is_selected());
   CHECK(!r1->is_selected());
   CHECK(r3->is_selected());              // its own group
}

TEST_CASE("toggles page: toggle selector example", "[toggles_page]")
{
   test_view tv{extent{300, 100}, 2};
   open(tv);
   auto favorite = toggle_button(star_styler{"Favorite"});
   auto starred = toggle_button(star_styler{"Starred"});
   starred.value(true);

   basic_context bctx{tv.view_, tv.cnv};
   auto l = favorite.limits(bctx);
   CHECK(l.min == l.max);

#if !defined(ARTIST_RECORDING)
   tv.view_.content(
      margin({20, 20, 20, 20},
         vtile_spaced(12.0, align_left(favorite), align_left(starred))),
      box(bkd_color)
   );
   tv.view_.layout();
   tv.draw();
   tv.img.save_png(std::string{RESULTS_PATH} + "toggles_selector.png");
#endif
}

TEST_CASE("toggles page: example", "[toggles_page]")
{
   // The page's example, from examples/buttons.
   auto  radio_button1 = radio_button("Eons from now");
   auto  radio_button2 = radio_button("Ultra-sentient particles");
   auto  radio_button3 = radio_button("The stratosphere is electrified");
   auto  radio_button4 = radio_button("No, no, not me");

   radio_button1.select(true);
   radio_button4.enable(false); // Disable this one

   auto  radio_buttons =
         vtile_spaced(10.0, // space in between
            align_left(radio_button1),
            align_left(radio_button2),
            align_left(radio_button3),
            align_left(radio_button4)
         );

   CHECK(radio_buttons.size() == 4);
   CHECK(!radio_button4.is_enabled());
}

#if !defined(ARTIST_RECORDING)
TEST_CASE("toggles page: figure", "[toggles_page]")
{
   test_view tv{extent{520, 150}, 2};
   open(tv);

   auto checked = check_box("Checked");
   checked.value(true);
   auto disabled = check_box("Disabled");
   disabled.enable(false);

   auto one = radio_button("Selected");
   one.select(true);
   auto two = radio_button("Not selected");
   auto three = radio_button("Disabled");
   three.enable(false);

   auto on = slide_switch();
   on.value(true);
   auto off = slide_switch();
   auto sw_disabled = slide_switch();
   sw_disabled.enable(false);

   tv.view_.content(
      margin({20, 20, 20, 20},
         htile(
            vtile_spaced(12.0,
               align_left(checked),
               align_left(check_box("Not checked")),
               align_left(disabled)
            ),
            vtile_spaced(12.0,
               align_left(one),
               align_left(two),
               align_left(three)
            ),
            vtile_spaced(14.0,
               align_left(htile(on, margin_left(10, label("On")))),
               align_left(htile(off, margin_left(10, label("Off")))),
               align_left(htile(sw_disabled, margin_left(10, label("Disabled"))))
            )
         )
      ),
      box(bkd_color)
   );
   tv.view_.layout();

   // A slide switch slides its thumb over a few frames.
   for (int i = 0; i != 20; ++i)
      tv.draw();
   tv.img.save_png(std::string{RESULTS_PATH} + "toggles.png");
}
#endif
