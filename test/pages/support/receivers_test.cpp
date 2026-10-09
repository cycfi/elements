/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/support/receivers.adoc:
   the calls in its two sequence diagrams, in order.
=============================================================================*/
#include "test_support.hpp"
#include <chrono>
#include <string>
#include <thread>
#include <vector>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   // One log for both channels, so their order shows: "change" for
   // on_change, the tracking states by name.
   struct call_log
   {
      void tracking(element::tracking state)
      {
         switch (state)
         {
            case element::begin_tracking: calls.push_back("begin"); break;
            case element::while_tracking: calls.push_back("while"); break;
            case element::end_tracking:   calls.push_back("end");   break;
            default:                      calls.push_back("?");     break;
         }
      }

      std::vector<std::string> calls;
   };

   // A slider across the view, at mid height.
   struct slider_view : test_view
   {
      slider_view()
       : test_view{{200, 100}}
       , vol{share(slider(basic_thumb<20>(), basic_track<5, false>(), 0.5))}
      {
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
         headless::open(view_);
#endif
         view_.content(align_middle(hold(vol)));
         view_.layout();
         draw();
         view_.on_tracking =
            [this](element&, element::tracking state) { log.tracking(state); };
      }

      mouse_button press(point p, bool down) const
      {
         mouse_button btn{};
         btn.down = down;
         btn.state = mouse_button::left;
         btn.num_clicks = 1;
         btn.pos = p;
         return btn;
      }

      std::shared_ptr<basic_slider_base> vol;
      call_log log;
   };
}

TEST_CASE("receivers page: basic_receiver", "[receivers_page]")
{
   basic_receiver<double> d;
   CHECK(d.value() == 0.0);         // value initialized
   d.value(0.5);
   CHECK(d.value() == 0.5);

   basic_receiver<std::string> s;
   CHECK(s.value().empty());
   s.value("abc");                  // takes a string_view
   std::string const& ref = s.value();   // returns a reference
   CHECK(ref == "abc");
}

TEST_CASE("receivers page: GUI interactions", "[receivers_page]")
{
   slider_view sv;
   sv.vol->on_change = [&](double) { sv.log.calls.push_back("change"); };

   sv.view_.click(sv.press({100, 50}, true));       // mouse down, on the thumb
   sv.view_.drag(sv.press({150, 50}, true));        // drag
   sv.view_.click(sv.press({150, 50}, false));      // mouse up

   CHECK(sv.log.calls
      == std::vector<std::string>{"begin", "change", "while", "end"});
   CHECK(sv.vol->value() > 0.5);
}

TEST_CASE("receivers page: scroll wheel", "[receivers_page]")
{
   slider_view sv;
   sv.vol->on_change = [&](double) { sv.log.calls.push_back("change"); };

   sv.view_.scroll({0, 1}, {100, 50});              // one wheel step

   // No press or release: the view supplies begin, and poll ends it.
   CHECK(sv.log.calls
      == std::vector<std::string>{"begin", "while", "change"});
   CHECK(sv.vol->value() != 0.5);
}

namespace
{
   // A button across the view, at mid height.
   struct button_view : test_view
   {
      button_view()
       : test_view{{200, 100}}
       , btn{share(button("OK"))}
      {
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
         headless::open(view_);
#endif
         view_.content(align_middle(hold(btn)));
         view_.layout();
         draw();
         view_.on_tracking =
            [this](element&, element::tracking state) { log.tracking(state); };
         btn->on_click = [this](bool) { log.calls.push_back("click"); };
      }

      mouse_button press(point p, bool down) const
      {
         mouse_button b{};
         b.down = down;
         b.state = mouse_button::left;
         b.num_clicks = 1;
         b.pos = p;
         return b;
      }

      std::shared_ptr<basic_button> btn;
      call_log log;
   };
}

TEST_CASE("receivers page: button click", "[receivers_page]")
{
   button_view bv;
   bv.view_.click(bv.press({100, 50}, true));       // mouse down
   bv.view_.click(bv.press({100, 50}, false));      // mouse up, inside

   // No while_tracking; the value goes out once, on release.
   CHECK(bv.log.calls == std::vector<std::string>{"begin", "click", "end"});
}

TEST_CASE("receivers page: button released outside", "[receivers_page]")
{
   button_view bv;
   bv.view_.click(bv.press({100, 50}, true));
   bv.view_.drag(bv.press({300, 50}, true));
   bv.view_.click(bv.press({300, 50}, false));      // mouse up, outside

   CHECK(bv.log.calls == std::vector<std::string>{"begin", "end"});
}

TEST_CASE("receivers page: button edit", "[receivers_page]")
{
   button_view bv;
   bv.btn->edit(bv.view_, true);
   CHECK(bv.log.calls == std::vector<std::string>{"click", "begin", "while"});
   CHECK(!bv.btn->value());         // momentary: edit leaves "pressed" alone
}

TEST_CASE("receivers page: toggle edit sets its own value", "[receivers_page]")
{
   test_view tv{{200, 100}};
   auto tgl = share(toggle_button("On"));
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(align_middle(hold(tgl)));
   tv.view_.layout();
   tv.draw();

   std::vector<bool> clicks;
   tgl->on_click = [&](bool v) { clicks.push_back(v); };
   tgl->edit(tv.view_, true);         // unbound: the value is set
   CHECK(tgl->value());
   CHECK(clicks == std::vector<bool>{true});
   tgl->edit(tv.view_, false);
   CHECK(!tgl->value());

   // Bound to a model, on_click updates the model, and the model calls
   // value back on the toggle, which already holds it.
   value_model<bool> on = false;
   model_binder binder{[&tv](element& e) { tv.view_.refresh(e); }};
   binder.bind(on, tgl);
   tgl->edit(tv.view_, true);
   CHECK(on == true);
   CHECK(tgl->value());
   on = false;                        // and the model reaches the toggle
   CHECK(!tgl->value());
}

TEST_CASE("receivers page: programmatic interactions", "[receivers_page]")
{
   slider_view sv;

   // The page's example.
   value_model<double> volume = 0.5;
   model_binder binder{[&sv](element& e) { sv.view_.refresh(e); }};
   binder.bind(volume, sv.vol);

   // value: the model reaches the slider, and nothing is called.
   volume = 0.3;
   CHECK(sv.vol->value() == Approx(0.3));
   CHECK(sv.log.calls.empty());

   // edit: on_change (here, the binding updating the model), then the
   // gesture, its begin supplied by the view.
   sv.vol->edit(sv.view_, 0.75);
   CHECK(volume == Approx(0.75));
   CHECK(sv.log.calls == std::vector<std::string>{"begin", "while"});

   // A further edit within the second continues the same gesture.
   sv.vol->edit(sv.view_, 0.8);
   CHECK(sv.log.calls == std::vector<std::string>{"begin", "while", "while"});

   // poll before the second is up ends nothing; after it, the gesture ends.
   sv.view_.poll();
   CHECK(sv.log.calls.size() == 3);
   std::this_thread::sleep_for(std::chrono::milliseconds{1100});
   sv.view_.poll();
   CHECK(sv.log.calls
      == std::vector<std::string>{"begin", "while", "while", "end"});
}

TEST_CASE("receivers page: edit calls on_change before the gesture", "[receivers_page]")
{
   slider_view sv;
   sv.vol->on_change = [&](double) { sv.log.calls.push_back("change"); };
   sv.vol->edit(sv.view_, 0.75);
   CHECK(sv.log.calls == std::vector<std::string>{"change", "begin", "while"});
}
