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
#include <string>
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
      view_limits limits(basic_context const&) const override
      {
         return {{0, 0}, {full_extent, full_extent}};
      }

      void draw(context const& ctx) override
      {
         // A moving marker so successive frames differ (nothing is trivially
         // cached). The continuous redraw is driven by the host loop in
         // benchmark mode, not from here (refresh() renders synchronously on
         // these hosts, so requesting it from draw would recurse).
         auto& cnv = ctx.canvas;
         auto  b = ctx.bounds;

         // The host has already applied the device scale to the canvas, so the
         // transform tells us how many real pixels this logical area covers.
         auto const xf = cnv.transform();
         perf::set_pixel_size(
            int(std::lround(b.width() * xf.a)),
            int(std::lround(b.height() * xf.d)));

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

      unsigned _frame = 0;
   };

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
      return margin({10, 10, 10, 10}, vscroller(hold(rows)));
   }

   auto make_scene()
   {
      // Two side-by-side panels of widget rows, over a background, with the
      // animation driver on top.
      return layer(
         perf_driver{},
         margin({16, 16, 16, 16},
            htile(
               panel{}, // shadow/gradient panel behind the left column
               make_column(),
               make_column()
            )
         ),
         box(bkd_color)
      );
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

   _app.run();
   return 0;
}
