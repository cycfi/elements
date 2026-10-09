/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/elements/dials.adoc and
   draws its figures. The figures land in the build's results directory;
   copy them to docs/modules/ROOT/images/elements/ to update the page.
=============================================================================*/
#include "test_support.hpp"
#include <cstdio>
#include <string>
#include <vector>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   auto constexpr bkd_color = rgba(35, 35, 37, 255);

   mouse_button left_button(point p, bool down, int modifiers = 0)
   {
      mouse_button btn{};
      btn.down = down;
      btn.state = mouse_button::left;
      btn.num_clicks = 1;
      btn.pos = p;
      btn.modifiers = modifiers;
      return btn;
   }

   // Every case leaves the theme as it found it.
   struct restore_theme
   {
      ~restore_theme() { set_theme(theme{}); }
   };

   // A 50 pixel knob in the middle of a 100 by 100 view.
   struct dial_view : test_view
   {
      dial_view(double init = 0.5)
       : test_view{{100, 100}}
       , d{share(dial(basic_knob<50>(), init))}
      {
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
         headless::open(view_);
#endif
         view_.content(align_center_middle(hold(d)));
         view_.layout();
         draw();
         d->on_change = [this](double v) { changes.push_back(v); };
      }

      void press(point p, int mods = 0)   { view_.click(left_button(p, true, mods)); }
      void release(point p, int mods = 0) { view_.click(left_button(p, false, mods)); }
      void drag(point p, int mods = 0)    { view_.drag(left_button(p, true, mods)); }

      std::shared_ptr<basic_dial> d;
      std::vector<double>         changes;
   };
}

TEST_CASE("dials page: linear mode", "[dials_page]")
{
   restore_theme guard;
   CHECK(get_theme().dial_mode == dial_mode_enum::linear);
   CHECK(get_theme().dial_linear_range == 200.0f);

   dial_view v;
   // Linear: the value changes by (dx - dy) / dial_linear_range per
   // movement, so up or right turns it up.
   v.press({50, 50});
   v.drag({70, 50});                        // right 20
   CHECK(v.d->value() == Approx(0.6));
   v.drag({70, 30});                        // up 20
   CHECK(v.d->value() == Approx(0.7));
   v.drag({30, 70});                        // left 40, down 40
   CHECK(v.d->value() == Approx(0.3));
   v.release({30, 70});
   CHECK(v.changes.size() == 3);

   // Shift makes it five times finer.
   v.press({50, 50}, mod_shift);
   v.drag({70, 50}, mod_shift);
   CHECK(v.d->value() == Approx(0.32));
   v.release({70, 50}, mod_shift);

   // Clamped at the ends.
   v.press({50, 50});
   v.drag({50, 400});
   CHECK(v.d->value() == Approx(0.0));
   v.release({50, 400});
}

TEST_CASE("dials page: radial mode", "[dials_page]")
{
   restore_theme guard;
   theme t;
   t.dial_mode = dial_mode_enum::radial;
   set_theme(t);

   dial_view v{0.5};
   // Radial: the value is the pointer's angle around the knob's centre,
   // along the dial's travel from the bottom left round to the bottom
   // right. Straight up is the middle; a movement is taken only when it
   // lands within 0.6 of the current value, so the pointer cannot jump
   // across the gap at the bottom.
   v.press({50, 50});
   v.drag({50, 20});                        // straight up
   CHECK(v.d->value() == Approx(0.5).margin(0.01));
   v.drag({80, 50});                        // right
   CHECK(v.d->value() == Approx(0.5 + 0.25 / 0.83).margin(0.01));
   v.drag({20, 50});                        // left: too far from where it was
   CHECK(v.d->value() == Approx(0.5 + 0.25 / 0.83).margin(0.01));
   v.release({20, 50});
}

TEST_CASE("dials page: value, edit, wheel", "[dials_page]")
{
   dial_view v;
   v.d->value(0.25);
   CHECK(v.d->value() == Approx(0.25));
   CHECK(v.changes.empty());
   v.d->value(3.0);
   CHECK(v.d->value() == Approx(1.0));

   v.d->edit(v.view_, 0.5);
   CHECK(v.changes == std::vector<double>{0.5});

   v.view_.scroll({0, 1}, {50, 50});        // 0.005 per unit
   CHECK(v.d->value() != Approx(0.5));
   CHECK(v.changes.size() == 2);
}

TEST_CASE("dials page: the example", "[dials_page]")
{
   test_view tv{extent{200, 160}, 1};
   auto& view_ = tv.view_;

   value_model<double> cutoff = 0.5;
   auto knob = share(dial(radial_marks<20>(basic_knob<50>())));
   auto cutoff_dial = radial_labels<15>(hold(knob), 0.7,
      "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10");
   auto readout = share(label("5.0"));

   model_binder binder{[&view_](element& e) { view_.refresh(e); }};
   binder.bind(cutoff, knob);
   binder.observe(cutoff,
      [readout, &view_](double v)
      {
         char buf[8];
         std::snprintf(buf, sizeof buf, "%.1f", v * 10);
         readout->set_text(buf);
         view_.refresh(*readout);
      });

   cutoff = 0.7;
   CHECK(knob->value() == Approx(0.7));
   CHECK(readout->get_text() == "7.0");

   knob->edit(view_, 0.25);                  // as the user would
   CHECK(double(cutoff) == Approx(0.25));
   CHECK(readout->get_text() == "2.5");
}

TEST_CASE("dials page: figure", "[dials_page]")
{
   // A dial with the marks and labels an application gives it.
   auto d = share(dial(radial_marks<20>(basic_knob<50>()), 0.6));
   auto marked = radial_labels<15>(hold(d), 0.7,
      "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10");

   test_view tv{extent{200, 150}, 2};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(
      margin({10, 10, 10, 10}, align_center_middle(marked)),
      box(bkd_color)
   );
   tv.view_.layout();
   tv.draw();
   tv.img.save_png(std::string{RESULTS_PATH} + "elements_dials.png");
}

TEST_CASE("dials page: example figure", "[dials_page]")
{
   // The page's example rendered: the bound dial and its readout.
   test_view tv{extent{200, 145}, 2};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   auto& view_ = tv.view_;
   value_model<double> cutoff = 0.5;
   auto knob = share(dial(radial_marks<20>(basic_knob<50>())));
   auto cutoff_dial = radial_labels<15>(hold(knob), 0.7,
      "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10");
   auto readout = share(label("5.0"));
   model_binder binder{[&view_](element& e) { view_.refresh(e); }};
   binder.bind(cutoff, knob);
   binder.observe(cutoff,
      [readout, &view_](double v)
      {
         char buf[8];
         std::snprintf(buf, sizeof buf, "%.1f", v * 10);
         readout->set_text(buf);
         view_.refresh(*readout);
      });
   tv.view_.content(
      margin({10, 10, 10, 10},
         align_center_middle(vtile(
            align_center(cutoff_dial),
            margin_top(4, align_center(hold(readout)))
         ))),
      box(bkd_color)
   );
   cutoff = 0.7;
   tv.view_.layout();
   tv.draw();
   tv.img.save_png(std::string{RESULTS_PATH} + "dials_example.png");
}
