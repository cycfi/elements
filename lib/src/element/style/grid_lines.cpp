/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/style/grid_lines.hpp>
#include <elements/support/theme.hpp>
#include <algorithm>
#include <cstdio>
#include <cmath>

namespace cycfi::elements
{
   namespace
   {
      // The taper, as decibels against travel in arbitrary units. The
      // bottom of the fader is silence, the 24 bit floor; everything below
      // the last mark shares the sliver of travel under it, as on a
      // console. A scale that stops higher just starts further up.
      constexpr double db_pts[] =
         {-144, -60, -50, -40, -30, -20, -10, 0, 10};
      constexpr double travel_pts[] =
         {0, 6.5, 13, 19.5, 32.5, 45.5, 58.5, 84.5, 110.5};
      constexpr std::size_t num_pts = 9;
      constexpr double top_slope = 2.6;   // units per dB above 10

      double taper(double db)
      {
         if (db <= db_pts[0])
            return travel_pts[0];
         if (db >= db_pts[num_pts-1])
            return travel_pts[num_pts-1]
               + (db - db_pts[num_pts-1]) * top_slope;
         std::size_t i = 1;
         while (i < num_pts - 1 && db > db_pts[i])
            ++i;
         auto t = (db - db_pts[i-1]) / (db_pts[i] - db_pts[i-1]);
         return travel_pts[i-1] + t * (travel_pts[i] - travel_pts[i-1]);
      }

      double untaper(double travel)
      {
         if (travel <= travel_pts[0])
            return db_pts[0];
         if (travel >= travel_pts[num_pts-1])
            return db_pts[num_pts-1]
               + (travel - travel_pts[num_pts-1]) / top_slope;
         std::size_t i = 1;
         while (i < num_pts - 1 && travel > travel_pts[i])
            ++i;
         auto t = (travel - travel_pts[i-1])
            / (travel_pts[i] - travel_pts[i-1]);
         return db_pts[i-1] + t * (db_pts[i] - db_pts[i-1]);
      }
   }

   double db_scale::position(double db) const
   {
      auto lo = taper(_min);
      auto hi = taper(_max);
      auto p = (taper(std::clamp(db, _min, _max)) - lo) / (hi - lo);
      return std::clamp(p, 0.0, 1.0);
   }

   double db_scale::value(double position_) const
   {
      auto lo = taper(_min);
      auto hi = taper(_max);
      auto travel = lo + std::clamp(position_, 0.0, 1.0) * (hi - lo);
      return std::clamp(untaper(travel), _min, _max);
   }

   int db_mark_label(double db, char* text, std::size_t size)
   {
      if (db <= -1000)
         return std::snprintf(text, size, "\xe2\x88\x9e");   // infinity
      return std::snprintf(text, size, "%.0f", std::abs(db));
   }

   namespace
   {
      // Every line of a decibel grid: the majors, and a minor halfway
      // between each pair. Each is handed to f as (position, major).
      template <typename F>
      void for_each_db_line(db_scale const& scale, F&& f)
      {
         double prev = 0;
         bool have_prev = false;
         for (auto m = db_marks; *m > -1000; ++m)
         {
            if (*m < scale.min() || *m > scale.max())
               continue;
            f(scale.position(*m), true);
            if (have_prev)
               f(scale.position((prev + *m) / 2), false);
            prev = *m;
            have_prev = true;
         }
      }
   }

   void vgrid_lines_db::draw(context const& ctx)
   {
      auto const&    theme_ = get_theme();
      auto&          canvas_ = ctx.canvas;
      auto const&    bounds = ctx.bounds;

      for_each_db_line(_scale,
         [&](double position, bool major)
         {
            canvas_.stroke_style(
               major ? theme_.major_grid_color : theme_.minor_grid_color);
            canvas_.line_width(
               major ? theme_.major_grid_width : theme_.minor_grid_width);
            auto y = bounds.bottom - float(position) * bounds.height();
            canvas_.move_to({bounds.left, y});
            canvas_.line_to({bounds.right, y});
            canvas_.stroke();
         });
   }

   void hgrid_lines_db::draw(context const& ctx)
   {
      auto const&    theme_ = get_theme();
      auto&          canvas_ = ctx.canvas;
      auto const&    bounds = ctx.bounds;

      for_each_db_line(_scale,
         [&](double position, bool major)
         {
            canvas_.stroke_style(
               major ? theme_.major_grid_color : theme_.minor_grid_color);
            canvas_.line_width(
               major ? theme_.major_grid_width : theme_.minor_grid_width);
            auto x = bounds.left + float(position) * bounds.width();
            canvas_.move_to({x, bounds.top});
            canvas_.line_to({x, bounds.bottom});
            canvas_.stroke();
         });
   }

   namespace
   {
      // Step from `start` to `end` in `divisions` equal parts, handing
      // every boundary to f, both edges included.
      template <typename F>
      void for_each_division(float start, float end, float divisions, F&& f)
      {
         float incr = (end - start) / divisions;
         for (float pos = start; pos <= end + 1; pos += incr)
            f(pos);
      }

      // Whether a minor rule at pos falls on a major division, within half
      // a pixel. One that does is left undrawn: the major rule is what the
      // reader is meant to see there, and both grid colors are
      // translucent, so drawing one over the other would blend them into a
      // third.
      bool on_a_major(float pos, float start, float end, float major_divisions)
      {
         float incr = (end - start) / major_divisions;
         if (!(incr > 0))
            return false;
         float n = (pos - start) / incr;
         return std::abs(n - std::round(n)) * incr < 0.5f;
      }
   }

   void vgrid_lines::draw(context const& ctx)
   {
      auto const&    theme_ = get_theme();
      auto&          canvas_ = ctx.canvas;
      auto const&    bounds = ctx.bounds;

      auto rule =
         [&](float y)
         {
            canvas_.move_to({bounds.left, y});
            canvas_.line_to({bounds.right, y});
            canvas_.stroke();
         };

      canvas_.stroke_style(theme_.minor_grid_color);
      canvas_.line_width(theme_.minor_grid_width);
      for_each_division(bounds.top, bounds.bottom, _minor_divisions,
         [&](float y)
         {
            if (!on_a_major(y, bounds.top, bounds.bottom, _major_divisions))
               rule(y);
         });

      canvas_.stroke_style(theme_.major_grid_color);
      canvas_.line_width(theme_.major_grid_width);
      for_each_division(bounds.top, bounds.bottom, _major_divisions, rule);
   }

   void hgrid_lines::draw(context const& ctx)
   {
      auto const& theme_ = get_theme();
      auto& canvas_ = ctx.canvas;
      auto const& bounds = ctx.bounds;

      auto rule =
         [&](float x)
         {
            canvas_.move_to({x, bounds.top});
            canvas_.line_to({x, bounds.bottom});
            canvas_.stroke();
         };

      canvas_.stroke_style(theme_.minor_grid_color);
      canvas_.line_width(theme_.minor_grid_width);
      for_each_division(bounds.left, bounds.right, _minor_divisions,
         [&](float x)
         {
            if (!on_a_major(x, bounds.left, bounds.right, _major_divisions))
               rule(x);
         });

      canvas_.stroke_style(theme_.major_grid_color);
      canvas_.line_width(theme_.major_grid_width);
      for_each_division(bounds.left, bounds.right, _major_divisions, rule);
   }
}

