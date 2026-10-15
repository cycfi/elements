/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/elements/tooltips.adoc and
   draws its figure. The figure lands in the build's results directory;
   copy it to docs/modules/ROOT/images/elements/ to update the page.
=============================================================================*/
#include "test_support.hpp"
#include <chrono>
#include <string>
#include <thread>

using namespace cycfi::elements;
using namespace cycfi::elements::test;
using namespace std::chrono_literals;

namespace
{
   auto constexpr bkd_color = rgba(35, 35, 37, 255);

   void open(test_view& tv)
   {
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
      headless::open(tv.view_);
#endif
   }

   void wait(view& v, std::chrono::milliseconds t)
   {
      std::this_thread::sleep_for(t);
      v.poll();
   }

   mouse_button left(point p, bool down)
   {
      mouse_button b{};
      b.state = mouse_button::left;
      b.down = down;
      b.num_clicks = 1;
      b.pos = p;
      return b;
   }

   void click(view& v, point p)
   {
      v.click(left(p, true));
      v.click(left(p, false));
   }

   // The page's example: examples/tooltip, verbatim.
   constexpr auto bred     = colors::red.opacity(0.4);
   constexpr auto bgreen   = colors::green.level(0.7).opacity(0.4);
   constexpr auto bblue    = colors::blue.opacity(0.4);
   constexpr auto pgold    = colors::gold.opacity(0.8);

   auto make_tip(std::string text)
   {
      return tooltip_bubble(std::move(text));
   }

   template <typename Label>
   auto make_button(std::string text, color c, Label label, view& view_)
   {
      static int i = 1;
      std::string orig{label->get_text().data(), label->get_text().size()};
      auto tt = tooltip(toggle_button("Option " + std::to_string(i++), 1.0f, c), make_tip(text));
      tt.on_hover =
         [label, text, &view_, orig](bool visible)
         {
            label->set_text(visible? text : orig);
            view_.refresh(*label.get(), 3);
         };
      return tt;
   }

   auto make_buttons(view& view_)
   {
      auto status = share(label("Have you found your vision quest?"));
      auto button1 = make_button("Eons from now", bred, status, view_);
      auto button2 = make_button("Take a stand against delusion", bgreen, status, view_);
      auto button3 = make_button("Totality is calling", bblue, status, view_);
      auto button4 = make_button("Four-dimensional superstructures", pgold, status, view_);

      return
         margin({50, 20, 50, 40},
            vtile(
               align_center(margin_top(20, hold(status))),
               margin_top(20, button1),
               margin_top(20, button2),
               margin_top(20, button3),
               margin_top(20, button4)
            )
         );
   }
}

TEST_CASE("tooltips page: tooltips", "[tooltips_page]")
{
   test_view tv{extent{300, 100}};
   open(tv);
   bool shown = false;
   auto t = share(tooltip(fixed_size({60, 30}, element{}),
      fixed_size({80, 20}, element{}), 5ms));
   t->on_hover = [&](bool visible) { shown = visible; };
   tv.view_.content(align_center_middle(hold(t)));
   tv.draw();

   // Rest on it: after the delay, the tip, above.
   context ctx{tv.view_, tv.cnv, t.get(), rect{120, 35, 180, 65}};
   t->cursor(ctx, {150, 50}, cursor_tracking::entering);
   wait(tv.view_, 20ms);
   wait(tv.view_, 1ms);
   REQUIRE(shown);
   auto tip = std::dynamic_pointer_cast<basic_popup_element>(
      tv.view_.layers().back());
   REQUIRE(tip);
   CHECK(tip->bounds() == rect{110, 15, 190, 35});

   // Escape closes it.
   t->key(ctx, key_info{key_code::escape, key_action::press, 0});
   tv.view_.poll();
   CHECK(!shown);
}

#if !defined(ARTIST_RECORDING)
TEST_CASE("tooltips page: example", "[tooltips_page]")
{
   test_view tv{extent{480, 340}, 2};
   open(tv);
   auto& view_ = tv.view_;
   view_.content(make_buttons(view_), box(bkd_color));
   view_.layout();
   tv.draw();

   // Rest on the second button: its tip, and the label shows its text.
   auto at = point{240, 157};
   view_.cursor(at, cursor_tracking::entering);
   view_.cursor(at, cursor_tracking::hovering);
   wait(view_, 600ms);
   wait(view_, 1ms);
   view_.layout();
   tv.draw();
   tv.img.save_png(std::string{RESULTS_PATH} + "tooltips_example.png");
   CHECK(std::dynamic_pointer_cast<basic_popup_element>(
      view_.layers().back()));
}
#endif
