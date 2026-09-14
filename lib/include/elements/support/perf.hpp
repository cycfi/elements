/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_PERF_SEPTEMBER_12_2026)
#define ELEMENTS_PERF_SEPTEMBER_12_2026

namespace cycfi { namespace elements { namespace perf
{
   // Frame-render benchmarking, gated by the ELEMENTS_PERF environment
   // variable. A host render path calls record() once per presented frame with
   // the draw+flush time in milliseconds (the CPU draw plus the backend's GPU
   // submit, excluding the vsync-blocked present). perf measures the full
   // frame-to-frame period itself from the interval between record() calls, so
   // a caller passes only the one number it can measure. After a warmup the
   // configured number of samples is gathered, a machine-readable summary line
   // is printed to stdout, and the process exits.
   //
   // Config (environment):
   //   ELEMENTS_PERF         set to enable; a positive value sets sample count
   //   ELEMENTS_PERF_WARMUP  frames discarded before sampling (default 60)
   //   ELEMENTS_PERF_SCENE   free-form scene label for the report line

   bool enabled();
   void record(double draw_flush_ms);

   // The size actually rasterized, in device pixels. Reported alongside the
   // timings: platforms differ in display scaling, so a frame time only means
   // something next to the number of pixels it covered.
   void set_pixel_size(int w, int h);
}}}

#endif
