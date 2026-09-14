/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Render-speed benchmark. Builds a fixed, render-heavy Elements scene (text,
   panels, gradients, sliders, dials, an image) and forces a full redraw every
   frame. With ELEMENTS_PERF set, the host render path samples per-frame
   draw+flush and frame timings and prints a summary line, then exits. Without
   it, this is just a normal window you can look at.
=============================================================================*/
#include <elements.hpp>
#include <elements/support/perf.hpp>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <chrono>
#include <string>
#include <memory>
#include <vector>

using namespace cycfi::elements;
using namespace cycfi::artist;

namespace
{
   auto constexpr bkd_color = rgba(35, 35, 37, 255);

   // A full-window overlay that keeps the view redrawing every frame and draws
   // a moving marker, so successive frames differ (nothing is trivially cached)
   // and the benchmark measures a continuous render stream.
   struct perf_driver : element
   {
      // Full-window scenes redraw everything every frame, driven from here.
      // The dirty and scroll scenes invalidate only a rect, from a timer, so
      // there the driver must neither paint nor ask for a redraw.
      explicit perf_driver(bool free_run) : _free_run(free_run) {}

      view_limits limits(basic_context const&) const override
      {
         return {{0, 0}, {full_extent, full_extent}};
      }

      void draw(context const& ctx) override
      {
         auto& cnv = ctx.canvas;
         auto  b = ctx.bounds;

         // The host has already applied the device scale to the canvas, so the
         // transform tells us how many real pixels this logical area covers.
         auto const xf = cnv.transform();
         perf::set_pixel_size(
            int(std::lround(b.width() * xf.a)),
            int(std::lround(b.height() * xf.d)));

         if (!_free_run)
            return;

         // A moving marker so successive frames differ (nothing is trivially
         // cached).
         float t = float(_frame % 180) / 180.0f;
         float x = b.left + t * b.width();
         cnv.fill_style(rgba(255, 255, 255, 30));
         cnv.fill_rect({x, b.top, x + 50, b.bottom});
         ++_frame;

#if !defined(ELEMENTS_HOST_UI_LIBRARY_X11) && !defined(ELEMENTS_HOST_UI_LIBRARY_WAYLAND)
         // On these hosts refresh() is an async invalidate (setNeedsDisplay /
         // InvalidateRect / queue_draw), so requesting it from draw safely
         // drives the next frame. The x11/wayland hosts render synchronously,
         // so there the benchmark free-runs from the host poll loop instead.
         ctx.view.refresh();
#endif
      }

      bool     _free_run;
      unsigned _frame = 0;
   };

   // Handles the timer-driven scenes animate: one dial, one scroller.
   std::function<void(double)>   set_dial;        // value in [0, 1]
   std::function<void(double)>   set_scroll;      // alignment in [0, 1]
   std::shared_ptr<element>      dial_element;
   std::shared_ptr<element>      scroll_element;

   // One row: a label, a button, a horizontal slider and a dial. Text shaping
   // and widget drawing are the dominant per-frame costs, so we stack many.
   auto make_row(int i)
   {
      auto lbl = label("Parameter " + std::to_string(i)).font_size(14);
      auto sld = share(slider(
         basic_thumb<12>(),
         slider_marks_lin<20>(basic_track<5, false>()),
         (i % 7) * 0.14f));
      auto dl = share(dial(
         radial_marks<15>(basic_knob<28>()),
         (i % 5) * 0.2f));
      if (!dial_element)
      {
         dial_element = dl;
         set_dial = [dl](double v){ dl->value(v); };
      }

      return margin({8, 4, 8, 4},
         htile(
            hsize(140, align_left(lbl)),
            hsize(90, align_middle(button("Go"))),
            hmargin({12, 12}, align_middle(hold(sld))),
            hsize(70, align_middle(hold(dl)))
         )
      );
   }

   auto make_column()
   {
      auto rows = share(vtile_composite{});
      for (int i = 0; i < 14; ++i)
         rows->push_back(share(make_row(i)));
      auto sc = share(vscroller(hold(rows)));
      if (!scroll_element)
      {
         scroll_element = sc;
         set_scroll = [sc](double v){ sc->valign(v); };
      }
      return margin({10, 10, 10, 10}, hold(sc));
   }

   // The widget scene: two side-by-side panels of rows. Text shaping and
   // widget drawing dominate, and the sliders and dials contribute hundreds of
   // small marks, so this is the vector-and-text workload.
   auto make_widgets()
   {
      return margin({16, 16, 16, 16},
         htile(
            panel{}, // shadow/gradient panel behind the left column
            make_column(),
            make_column()
         )
      );
   }

   // ELEMENTS_PERF_SCENE picks what is measured. Each scene answers a
   // different question, and the label is already reported on the perf line:
   //
   //   minimal   background and the driver only: the fixed per-frame cost of
   //             the host (clear, present, vsync), with no content to speak
   //             of. This is the floor every other scene builds on.
   //   default   the widget scene: vector shapes and text at volume.
   //   image     a photo redrawn every frame: bitmap upload, scaling and
   //             filtering, which the other two never touch.
   //   dirty     the widget scene, but only one dial changes per frame and
   //             only its rect is invalidated: what an interaction costs.
   //   scroll    the widget scene with one column scrolling continuously:
   //             the whole viewport invalidated, every frame.
   //
   // Note that `minimal` is usually vsync-bound, so read draw_flush_ms there
   // and treat its fps as a property of the display.
   std::string scene_name()
   {
      if (char const* s = std::getenv("ELEMENTS_PERF_SCENE"))
         return s;
      return "default";
   }

   auto make_scene()
   {
      auto scene = scene_name();
      bool timed = scene == "dirty" || scene == "scroll";

      std::shared_ptr<element> content;
      if (scene == "minimal")
         content = share(element{});
      else if (scene == "image")
         // In a scroller, as hello_universe has it: a bare image reports its
         // full size as a minimum and the window grows to the photo, which
         // would measure a different number of pixels than the other scenes.
         // Qualified because artist has an `image` too and this file uses both.
         content = share(scroller(cycfi::elements::image{"space.jpg"}));
      else
         content = share(make_widgets());

      return layer(
         perf_driver{!timed},
         hold(content),
         box(bkd_color)
      );
   }

   // Drive the dirty and scroll scenes from a timer: change one thing, ask for
   // a redraw of just that element, and let the host paint. Re-posted from
   // its own callback so it runs as fast as the host paints.
   void start_animation(view& v)
   {
      auto scene = scene_name();
      if (scene != "dirty" && scene != "scroll")
         return;

      auto tick = std::make_shared<std::function<void()>>();
      auto frame = std::make_shared<unsigned>(0);
      *tick = [&v, tick, frame, scene]()
      {
         double t = double(++*frame % 240) / 240.0;
         if (scene == "dirty" && set_dial && dial_element)
         {
            set_dial(t);
            v.refresh(*dial_element);
         }
         else if (scene == "scroll" && set_scroll && scroll_element)
         {
            set_scroll(t);
            v.refresh(*scroll_element);
         }
         v.post(std::chrono::milliseconds(1), *tick);
      };
      v.post(std::chrono::milliseconds(1), *tick);
   }
}

int main(int argc, char* argv[])
{
   app _app("Elements Render Benchmark");
   init_resources();

   // Render the same logical area everywhere, so platforms differ only by
   // display scale and the numbers can be compared. ELEMENTS_PERF_SIZE=WxH
   // overrides it, which is also how the fixed and per-pixel parts of the
   // frame cost get separated.
   float win_w = 1000, win_h = 700;
   if (char const* s = std::getenv("ELEMENTS_PERF_SIZE"))
   {
      int w = 0, h = 0;
      if (std::sscanf(s, "%dx%d", &w, &h) == 2 && w > 0 && h > 0)
      {
         win_w = float(w);
         win_h = float(h);
      }
   }
   window _win(_app.name(), window::standard, {50, 50, 50 + win_w, 50 + win_h});
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);
   view_.content(make_scene());
   start_animation(view_);

   _app.run();
   return 0;
}
