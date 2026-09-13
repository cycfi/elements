/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/support/perf.hpp>
#include <chrono>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cstdio>
#include <cstdlib>
#include <string>

namespace cycfi { namespace elements { namespace perf
{
   namespace
   {
      char const* backend_name()
      {
#if defined(ARTIST_SKIA)
         return "skia";
#elif defined(ARTIST_CAIRO)
         return "cairo";
#elif defined(ARTIST_QUARTZ_2D)
         return "quartz2d";
#elif defined(ARTIST_DIRECT2D)
         return "direct2d";
#else
         return "unknown";
#endif
      }

      char const* host_name()
      {
#if defined(ELEMENTS_HOST_UI_LIBRARY_COCOA)
         return "cocoa";
#elif defined(ELEMENTS_HOST_UI_LIBRARY_GTK)
         return "gtk";
#elif defined(ELEMENTS_HOST_UI_LIBRARY_X11)
         return "x11";
#elif defined(ELEMENTS_HOST_UI_LIBRARY_WAYLAND)
         return "wayland";
#elif defined(ELEMENTS_HOST_UI_LIBRARY_WIN32)
         return "win32";
#elif defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
         return "headless";
#else
         return "unknown";
#endif
      }

      char const* platform_name()
      {
#if defined(__APPLE__)
         return "macos";
#elif defined(_WIN32)
         return "windows";
#elif defined(__linux__)
         return "linux";
#else
         return "unknown";
#endif
      }

      struct config
      {
         bool          on = false;
         int           warmup = 60;
         int           frames = 600;
         std::string   scene = "default";
      };

      config load()
      {
         config c;
         if (char const* e = std::getenv("ELEMENTS_PERF"))
         {
            c.on = true;
            int n = std::atoi(e);
            if (n > 0)
               c.frames = n;
         }
         if (char const* w = std::getenv("ELEMENTS_PERF_WARMUP"))
         {
            int n = std::atoi(w);
            if (n >= 0)
               c.warmup = n;
         }
         if (char const* s = std::getenv("ELEMENTS_PERF_SCENE"))
            c.scene = s;
         return c;
      }

      config& cfg()
      {
         static config c = load();
         return c;
      }

      using clock = std::chrono::steady_clock;

      std::vector<double>  draw_samples;   // draw+flush ms (from caller)
      std::vector<double>  frame_samples;  // full frame period ms (measured here)
      int                  seen = 0;
      int                  px_w = 0, px_h = 0;
      bool                 have_last = false;
      clock::time_point    last;

      double percentile(std::vector<double> v, double p)
      {
         if (v.empty())
            return 0;
         std::sort(v.begin(), v.end());
         return v[std::size_t(p * (v.size() - 1))];
      }

      double average(std::vector<double> const& v)
      {
         if (v.empty())
            return 0;
         return std::accumulate(v.begin(), v.end(), 0.0) / double(v.size());
      }

      double minimum(std::vector<double> const& v)
      {
         if (v.empty())
            return 0;
         return *std::min_element(v.begin(), v.end());
      }

      void summarize_and_exit()
      {
         double const d_med = percentile(draw_samples, 0.5);
         double const d_mean = average(draw_samples);
         double const d_p95 = percentile(draw_samples, 0.95);
         double const d_min = minimum(draw_samples);

         double const f_med = percentile(frame_samples, 0.5);
         double const f_mean = average(frame_samples);
         double const f_p95 = percentile(frame_samples, 0.95);
         double const f_min = minimum(frame_samples);

         double const fps_med = (f_med > 0)? 1000.0 / f_med : 0;
         double const fps_mean = (f_mean > 0)? 1000.0 / f_mean : 0;

         std::fprintf(stdout,
            "ELEMENTS_PERF platform=%s host=%s backend=%s scene=%s pixels=%dx%d "
            "samples=%zu warmup=%d "
            "draw_flush_ms{median=%.3f,mean=%.3f,p95=%.3f,min=%.3f} "
            "frame_ms{median=%.3f,mean=%.3f,p95=%.3f,min=%.3f} "
            "fps{median=%.1f,mean=%.1f}\n",
            platform_name(), host_name(), backend_name(), cfg().scene.c_str(),
            px_w, px_h, draw_samples.size(), cfg().warmup,
            d_med, d_mean, d_p95, d_min,
            f_med, f_mean, f_p95, f_min,
            fps_med, fps_mean);
         std::fflush(stdout);
         std::_Exit(0);
      }
   }

   void set_pixel_size(int w, int h)
   {
      px_w = w;
      px_h = h;
   }

   bool enabled()
   {
      return cfg().on;
   }

   void record(double draw_flush_ms)
   {
      if (!cfg().on)
         return;

      auto const now = clock::now();
      double frame_ms = 0;
      if (have_last)
         frame_ms = std::chrono::duration<double, std::milli>(now - last).count();
      last = now;
      have_last = true;

      ++seen;
      if (seen <= cfg().warmup)   // discard warmup frames
         return;

      draw_samples.push_back(draw_flush_ms);
      if (frame_ms > 0)
         frame_samples.push_back(frame_ms);

      if (int(draw_samples.size()) >= cfg().frames)
         summarize_and_exit();
   }
}}}
