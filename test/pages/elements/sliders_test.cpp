/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/elements/sliders.adoc and
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

   // A slider across a 220 by 60 view: the thumb is 20 wide, so its
   // centre travels from x = 10 at 0.0 to x = 210 at 1.0, 200 pixels.
   template <typename Slider>
   struct slider_view : test_view
   {
      explicit slider_view(Slider s)
       : test_view{{220, 60}}
       , sl{share(std::move(s))}
      {
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
         headless::open(view_);
#endif
         view_.content(align_middle(hold(sl)));
         view_.layout();
         draw();
      }

      void press(point p, int mods = 0)   { view_.click(left_button(p, true, mods)); }
      void release(point p, int mods = 0) { view_.click(left_button(p, false, mods)); }
      void drag(point p, int mods = 0)    { view_.drag(left_button(p, true, mods)); }

      std::shared_ptr<Slider> sl;
   };
}

TEST_CASE("sliders page: the value follows the pointer", "[sliders_page]")
{
   slider_view v{slider(basic_thumb<20>(), basic_track<5, false>(), 0.5)};
   std::vector<double> changes;
   v.sl->on_change = [&](double val) { changes.push_back(val); };

   // Grabbing the thumb at its centre, then dragging: the value is where
   // the thumb's centre is, over the 200 pixels it can travel.
   v.press({110, 30});
   v.drag({160, 30});
   CHECK(v.sl->value() == Approx(0.75));
   v.drag({10, 30});
   CHECK(v.sl->value() == Approx(0.0));
   v.drag({0, 30});                       // past the end: clamped
   CHECK(v.sl->value() == Approx(0.0));
   v.release({0, 30});
   CHECK(changes.size() == 2);            // the unchanged last step is not reported
   CHECK(changes.front() == Approx(0.75));

   // A press on the track, away from the thumb, jumps the thumb there on
   // the first movement.
   v.press({210, 30});
   v.drag({211, 30});
   CHECK(v.sl->value() == Approx(1.0));
   v.release({211, 30});
}

TEST_CASE("sliders page: grabbing the thumb off centre", "[sliders_page]")
{
   slider_view v{slider(basic_thumb<20>(), basic_track<5, false>(), 0.5)};

   // Grab 8 pixels right of the thumb's centre and drag 50: the thumb
   // moves 50, not 58.
   v.press({118, 30});
   v.drag({168, 30});
   CHECK(v.sl->value() == Approx(0.75));
   v.release({168, 30});
}

TEST_CASE("sliders page: vertical, with zero at the bottom", "[sliders_page]")
{
   test_view tv{extent{60, 220}, 1};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   auto sl = share(slider(basic_thumb<20>(), basic_track<5, true>(), 0.5));
   tv.view_.content(align_center(hold(sl)));
   tv.view_.layout();
   tv.draw();

   tv.view_.click(left_button({30, 110}, true));
   tv.view_.drag(left_button({30, 60}, true));         // up
   CHECK(sl->value() == Approx(0.75));
   tv.view_.drag(left_button({30, 210}, true));        // to the bottom
   CHECK(sl->value() == Approx(0.0));
   tv.view_.click(left_button({30, 210}, false));
}

TEST_CASE("sliders page: value, edit and the wheel", "[sliders_page]")
{
   slider_view v{slider(basic_thumb<20>(), basic_track<5, false>(), 0.5)};
   std::vector<double> changes;
   v.sl->on_change = [&](double val) { changes.push_back(val); };

   v.sl->value(0.25);                     // quiet
   CHECK(v.sl->value() == Approx(0.25));
   CHECK(changes.empty());
   v.sl->value(2.0);                      // clamped
   CHECK(v.sl->value() == Approx(1.0));

   v.sl->edit(v.view_, 0.5);              // as the user: on_change
   CHECK(changes == std::vector<double>{0.5});

   // The wheel moves it by 0.005 per unit, in the track's direction.
   auto before = v.sl->value();
   v.view_.scroll({1, 0}, {110, 30});
   CHECK(v.sl->value() != before);
   CHECK(changes.size() == 2);
}

TEST_CASE("sliders page: a selector snaps", "[sliders_page]")
{
   slider_view v{selector<5>(basic_thumb<20>(), basic_track<5, false>(), 0.0)};
   std::vector<std::size_t> picks;
   v.sl->on_change = [&](std::size_t i) { picks.push_back(i); };

   // Five states at 0, 0.25, 0.5, 0.75 and 1; a value snaps to the
   // nearest, and on_change gets the index.
   v.sl->value(0.3);
   CHECK(v.sl->value() == Approx(0.25));
   CHECK(picks == std::vector<std::size_t>{1});

   v.press({10, 30});
   v.drag({150, 30});                     // 0.7 of the travel
   CHECK(v.sl->value() == Approx(0.75));
   v.release({150, 30});
   CHECK(picks.back() == 3);

   // No wheel on a selector.
   auto before = v.sl->value();
   v.view_.scroll({1, 0}, {150, 30});
   CHECK(v.sl->value() == before);
}

TEST_CASE("sliders page: a range slider", "[sliders_page]")
{
   slider_view v{range_slider(basic_thumb<20>(), basic_thumb<20>(),
                              basic_track<5, false>(), {0.25, 0.75})};
   std::vector<double> firsts, seconds;
   v.sl->on_change.first = [&](double val) { firsts.push_back(val); };
   v.sl->on_change.second = [&](double val) { seconds.push_back(val); };

   CHECK(v.sl->value_first() == Approx(0.25));
   CHECK(v.sl->value_second() == Approx(0.75));

   // Drag the first thumb (centre at x = 60) to 0.5.
   v.press({60, 30});
   v.drag({110, 30});
   v.release({110, 30});
   CHECK(v.sl->value_first() == Approx(0.5));
   CHECK(v.sl->value_second() == Approx(0.75));
   CHECK(!firsts.empty());
   CHECK(seconds.empty());

   // The thumbs do not cross: dragging the first past the second stops
   // a thumb's width short of it.
   v.press({110, 30});
   v.drag({210, 30});
   v.release({210, 30});
   CHECK(v.sl->value_first() < v.sl->value_second());

   // Shift-drag moves both, keeping the distance.
   auto d = v.sl->value_second() - v.sl->value_first();
   auto x1 = 10 + 200 * v.sl->value_first();
   v.press({float(x1), 30}, mod_shift);
   v.drag({float(x1) - 40, 30}, mod_shift);
   v.release({float(x1) - 40, 30}, mod_shift);
   CHECK(v.sl->value_second() - v.sl->value_first() == Approx(d));

   // From code: value and the per-thumb setters.
   v.sl->value({0.1, 0.9});
   CHECK(v.sl->value() == double_range{0.1, 0.9});
   v.sl->value_first(0.2);
   CHECK(v.sl->value_first() == Approx(0.2));
}

TEST_CASE("sliders page: the wheel on two range sliders", "[sliders_page]")
{
   // Each range slider keeps its own wheel state: wheeling one, then
   // another at a point near the first thumb of each, moves the right
   // thumb of each.
   test_view tv{extent{220, 120}, 1};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   auto a = share(range_slider(basic_thumb<20>(), basic_thumb<20>(),
                               basic_track<5, false>(), {0.25, 0.75}));
   auto b = share(range_slider(basic_thumb<20>(), basic_thumb<20>(),
                               basic_track<5, false>(), {0.25, 0.75}));
   tv.view_.content(vtile(align_middle(hold(a)), align_middle(hold(b))));
   tv.view_.layout();
   tv.draw();

   tv.view_.scroll({1, 0}, {60, 30});           // a's first thumb
   tv.view_.scroll({1, 0}, {160, 90});          // b's second thumb
   CHECK(a->value_first() != Approx(0.25));
   CHECK(a->value_second() == Approx(0.75));
   CHECK(b->value_first() == Approx(0.25));
   CHECK(b->value_second() != Approx(0.75));
}

TEST_CASE("sliders page: the examples", "[sliders_page]")
{
   test_view tv{extent{220, 60}, 1};
   auto& view_ = tv.view_;

   value_model<double> volume = 0.5;
   auto vol = share(slider(basic_thumb<20>(), basic_track<5, false>()));
   auto readout = share(label("50%"));

   model_binder binder{[&view_](element& e) { view_.refresh(e); }};
   binder.bind(volume, vol);
   binder.observe(volume,
      [readout, &view_](double v)
      {
         readout->set_text(std::to_string(int(v * 100)) + "%");
         view_.refresh(*readout);
      });

   volume = 0.75;
   CHECK(vol->value() == Approx(0.75));
   CHECK(readout->get_text() == "75%");

   std::size_t mode_ = 0;
   auto mode = selector<4>(basic_tri_thumb<28, direction::down>(), basic_track<5, false>());
   mode.on_change = [&](std::size_t i) { mode_ = i; };
   mode.value(1.0);
   CHECK(mode_ == 3);

   double lo = 0, hi = 0;
   auto band = range_slider(
      basic_thumb<20>(), basic_thumb<20>(), basic_track<5, false>(), {0.2, 0.8});
   band.on_change.first = [&](double v) { lo = v; };
   band.on_change.second = [&](double v) { hi = v; };
   band.edit(view_, {0.3, 0.7});
   CHECK(lo == Approx(0.3));
   CHECK(hi == Approx(0.7));
}

TEST_CASE("sliders page: figure", "[sliders_page]")
{
   // The three forms and a vertical slider, with the marks and labels an
   // application gives them, and a caption each.
   auto caption = [](char const* text)
   {
      return align_center(margin_top(4,
         label(text).font_size(12).font_color(rgba(150, 150, 150, 255))));
   };
   auto marked = [](bool vertical)
   {
      return slider_labels<10>(
         slider_marks_lin<30>(basic_track<5, false>()), 0.8,
         "0", "25", "50", "75", "100");
   };

   auto hslider = slider(basic_thumb<20>(), marked(false), 0.6);
   auto sel = selector<4>(basic_tri_thumb<28, direction::down>(),
      slider_labels<10>(slider_marks_lin<30, 3, 1>(basic_track<5, false>()), 0.8,
         "Off", "Low", "Mid", "High"), 0.0);
   sel.value(0.34);
   auto rslider = range_slider(basic_thumb<20>(), basic_thumb<20>(),
      marked(false), {0.25, 0.7});
   auto vslider = slider(basic_thumb<20>(),
      slider_labels<10>(slider_marks_lin<30>(basic_track<5, true>()), 0.8,
         "0", "50", "100"), 0.35);

   test_view tv{extent{420, 260}, 2};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(
      margin({20, 16, 20, 16},
         htile(
            vtile(
               vtile(align_middle(hslider), caption("slider")),
               vtile(align_middle(sel), caption("selector<4>")),
               vtile(align_middle(rslider), caption("range_slider"))
            ),
            margin_left(40, vtile(align_center(vslider), caption("vertical")))
         )
      ),
      box(bkd_color)
   );
   tv.view_.layout();
   tv.draw();
   tv.img.save_png(std::string{RESULTS_PATH} + "elements_sliders.png");
}

TEST_CASE("sliders page: example figure", "[sliders_page]")
{
   // The page's first example rendered: the bound slider and its readout.
   test_view tv{extent{240, 70}, 2};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   auto& view_ = tv.view_;
   value_model<double> volume = 0.5;
   auto vol = share(slider(basic_thumb<20>(), basic_track<5, false>()));
   auto readout = share(label("50%"));
   model_binder binder{[&view_](element& e) { view_.refresh(e); }};
   binder.bind(volume, vol);
   binder.observe(volume,
      [readout, &view_](double v)
      {
         readout->set_text(std::to_string(int(v * 100)) + "%");
         view_.refresh(*readout);
      });
   tv.view_.content(
      margin({10, 10, 10, 10},
         htile(align_middle(hold(vol)), margin_left(12, hsize(40, align_middle(hold(readout)))))),
      box(bkd_color)
   );
   volume = 0.75;
   tv.view_.layout();
   tv.draw();
   tv.img.save_png(std::string{RESULTS_PATH} + "sliders_example.png");
}
