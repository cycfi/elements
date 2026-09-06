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
      // The taper, as decibels against travel in arbitrary units
      constexpr double db_pts[] =
         {-70, -60, -50, -40, -30, -20, -10, 0, 10};
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

   void vgrid_lines::draw(context const& ctx)
   {
      auto const&    theme_ = get_theme();
      auto&          canvas_ = ctx.canvas;
      auto const&    bounds = ctx.bounds;

      float pos = bounds.top;
      float incr = bounds.height() / _major_divisions;

      canvas_.stroke_style(theme_.major_grid_color);
      canvas_.line_width(theme_.major_grid_width);
      while (pos <= bounds.bottom+1)
      {
         canvas_.move_to({bounds.left, pos});
         canvas_.line_to({bounds.right, pos});
         canvas_.stroke();
         pos += incr;
      }

      pos = bounds.top;
      incr = bounds.height() / _minor_divisions;

      canvas_.stroke_style(theme_.minor_grid_color);
      canvas_.line_width(theme_.minor_grid_width);
      while (pos <= bounds.bottom+1)
      {
         canvas_.move_to({bounds.left, pos});
         canvas_.line_to({bounds.right, pos});
         canvas_.stroke();
         pos += incr;
      }
   }

   void hgrid_lines::draw(context const& ctx)
   {
      auto const& theme_ = get_theme();
      auto& canvas_ = ctx.canvas;
      auto const& bounds = ctx.bounds;

      float pos = bounds.left;
      float incr = bounds.width() / _major_divisions;

      canvas_.stroke_style(theme_.major_grid_color);
      canvas_.line_width(theme_.major_grid_width);
      while (pos <= bounds.right + 1)
      {
         canvas_.move_to({pos, bounds.top});
         canvas_.line_to({pos, bounds.bottom});
         canvas_.stroke();
         pos += incr;
      }

      pos = bounds.left;
      incr = bounds.width() / _minor_divisions;

      canvas_.stroke_style(theme_.minor_grid_color);
      canvas_.line_width(theme_.minor_grid_width);
      while (pos <= bounds.right + 1)
      {
         canvas_.move_to({pos, bounds.top});
         canvas_.line_to({pos, bounds.bottom});
         canvas_.stroke();
         pos += incr;
      }
   }
}

