/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_STYLE_TRACK_OCTOBER_14_2026)
#define ELEMENTS_STYLE_TRACK_OCTOBER_14_2026

#include <elements/element/element.hpp>
#include <elements/element/slider.hpp>
#include <elements/element/range_slider.hpp>
#include <elements/element/status_bar.hpp>
#include <elements/element/style/dial.hpp>
#include <elements/element/traversal.hpp>
#include <elements/support/draw_utils.hpp>
#include <elements/support/theme.hpp>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>

// MSVC lays out only the first of several empty bases at no cost unless
// told to lay out all of them so: a styler's colors are empty bases.
#if defined(_MSC_VER)
# define ELEMENTS_EMPTY_BASES __declspec(empty_bases)
#else
# define ELEMENTS_EMPTY_BASES
#endif

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Colors, as a styler takes them: a default, which is empty and so costs
   // nothing to hold as a base, or one given.
   ////////////////////////////////////////////////////////////////////////////
   struct fixed_color
   {
      color c;
      color operator()() const { return c; }
   };

   struct default_track_color
   {
      color operator()() const { return colors::black; }
   };

   struct default_fill_color
   {
      color operator()() const { return get_theme().indicator_color; }
   };

   enum class track_shape
   {
      round
    , square
   };

   // Where a radial fill starts: at the start of the dial's travel, or at
   // its center, 12 o'clock, filling to either side, as for a pan control
   enum class track_origin
   {
      start
    , center
   };

   namespace detail
   {
      // A track with square ends: draw_track's look without the rounding.
      inline void draw_square_track(canvas& cnv, rect bounds, color c)
      {
         auto state = cnv.new_state();
         auto r = std::min(bounds.width(), bounds.height()) / 2;
         cnv.begin_path();
         cnv.add_rect(bounds);
         cnv.clip();

         cnv.fill_style(c);
         cnv.fill_rect(bounds);

         auto lwidth = r/4;
         cnv.stroke_style(colors::white.opacity(0.3));
         cnv.add_rect(bounds.move(-lwidth, -lwidth));
         cnv.line_width(lwidth*1.5);
         cnv.stroke();
      }

      inline void draw_track(canvas& cnv, rect bounds, color c, track_shape shape)
      {
         if (shape == track_shape::square)
            draw_square_track(cnv, bounds, c);
         else
            elements::draw_track(cnv, bounds, c);
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   // Basic Track (You can use this as the slider's track)
   ////////////////////////////////////////////////////////////////////////////
   template <
      unsigned _size
    , bool _vertical = false
    , typename Color = default_track_color
    , track_shape _shape = track_shape::round
   >
   class ELEMENTS_EMPTY_BASES basic_track_styler : Color, public element
   {
   public:

      static unsigned const size = _size;
      static bool const vertical = _vertical;
      static unsigned const min_length = 64;

                              basic_track_styler() = default;

                              basic_track_styler(Color c)
                               : Color(c)
                              {}

      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;
      std::string             class_name() const override { return "track"; }

      basic_track_styler<_size, _vertical, fixed_color, _shape>
                              track_color(color c) const
                              { return {fixed_color{c}}; }
   };

   template <unsigned size, bool vertical, typename Color, track_shape _shape>
   inline view_limits basic_track_styler<size, vertical, Color, _shape>
      ::limits(basic_context const& /* ctx */) const
   {
	  auto sz = float(size);
	  auto min_length_ = float(min_length);
	  auto p1 = vertical ? point{sz, min_length} : point{ min_length_, sz};
	  auto p2 = vertical ? point{sz, full_extent} : point{ full_extent, sz};
	  return view_limits{p1, p2};
   }

   template <unsigned size, bool vertical, typename Color, track_shape _shape>
   inline void basic_track_styler<size, vertical, Color, _shape>
      ::draw(context const& ctx)
   {
      detail::draw_track(ctx.canvas, ctx.bounds, static_cast<Color const&>(*this)(), _shape);
   }

   template <
      unsigned size
    , bool vertical = false
    , track_shape shape = track_shape::round
   >
   inline basic_track_styler<size, vertical, default_track_color, shape>
   basic_track()
   {
      return {};
   }

   ////////////////////////////////////////////////////////////////////////////
   // Fill Track: a track filled up to a value. In a slider, up to the
   // slider's value; in a range slider, between its two values; anywhere
   // else, all of it, as a status bar's foreground, which the bar sizes to
   // its value.
   ////////////////////////////////////////////////////////////////////////////
   template <typename C>
   struct track_part : C
   {
      color track() const { return C::operator()(); }
   };

   template <typename C>
   struct fill_part : C
   {
      color fill() const { return C::operator()(); }
   };

   template <typename Track, typename Fill>
   struct ELEMENTS_EMPTY_BASES fill_track_colors : track_part<Track>, fill_part<Fill>
   {
      using track_type = Track;
      using fill_type = Fill;
   };

   // Bands of color along the track, as a VU meter has: each band goes up
   // to `to`, from 0.0 to 1.0 along the whole track, or the whole bar of a
   // status bar.
   struct fill_band
   {
      float    to;
      color    c;
   };

   template <std::size_t N>
   struct band_fill
   {
      std::array<fill_band, N> bands;
      color operator()() const { return bands[N-1].c; }
   };

   // A smooth gradient along the track, through color stops from 0.0 to
   // 1.0 along the whole track, or the whole bar of a status bar.
   template <std::size_t N>
   struct gradient_fill
   {
      std::array<canvas::color_stop, N> stops;
      color operator()() const { return stops[N-1].color; }

      // The color at pos, from 0.0 to 1.0
      color at(float pos) const
      {
         if (pos <= stops[0].offset)
            return stops[0].color;
         for (std::size_t i = 1; i != N; ++i)
         {
            if (pos <= stops[i].offset)
            {
               auto const& a = stops[i-1];
               auto const& b = stops[i];
               auto t = (pos - a.offset) / std::max(b.offset - a.offset, 1e-6f);
               return color{
                  a.color.red + (b.color.red - a.color.red) * t
                , a.color.green + (b.color.green - a.color.green) * t
                , a.color.blue + (b.color.blue - a.color.blue) * t
                , a.color.alpha + (b.color.alpha - a.color.alpha) * t
               };
            }
         }
         return stops[N-1].color;
      }
   };

   namespace detail
   {
      template <typename T>
      struct is_gradient_fill : std::false_type {};

      template <std::size_t N>
      struct is_gradient_fill<gradient_fill<N>> : std::true_type {};

      template <typename T>
      struct is_band_fill : std::false_type {};

      template <std::size_t N>
      struct is_band_fill<band_fill<N>> : std::true_type {};
   }

   template <
      unsigned _size
    , bool _vertical = false
    , typename Colors = fill_track_colors<default_track_color, default_fill_color>
    , track_shape _shape = track_shape::round
    , bool _glow = false
   >
   class ELEMENTS_EMPTY_BASES basic_fill_track_styler : Colors, public element
   {
   public:

      static unsigned const size = _size;
      static bool const vertical = _vertical;
      static unsigned const min_length = 64;
      static unsigned const glow_size = _glow? _size : 0;

                              basic_fill_track_styler() = default;

                              basic_fill_track_styler(Colors c)
                               : Colors(c)
                              {}

      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;
      std::string             class_name() const override { return "fill_track"; }

      using track_type = typename Colors::track_type;
      using fill_type = typename Colors::fill_type;

      color                   track_color() const { return this->track(); }
      color                   fill_color() const { return this->fill(); }

      basic_fill_track_styler<_size, _vertical, fill_track_colors<fixed_color, fill_type>, _shape, _glow>
                              track_color(color c) const
                              {
                                 return {{{fixed_color{c}}, static_cast<fill_part<fill_type> const&>(*this)}};
                              }

      basic_fill_track_styler<_size, _vertical, fill_track_colors<track_type, fixed_color>, _shape, _glow>
                              fill_color(color c) const
                              {
                                 return {{static_cast<track_part<track_type> const&>(*this), {fixed_color{c}}}};
                              }

                              template <std::size_t N>
      basic_fill_track_styler<_size, _vertical, fill_track_colors<track_type, band_fill<N>>, _shape, _glow>
                              fill_colors(fill_band const (&bands)[N]) const
                              {
                                 band_fill<N> b;
                                 for (std::size_t i = 0; i != N; ++i)
                                    b.bands[i] = bands[i];
                                 return {{static_cast<track_part<track_type> const&>(*this), {b}}};
                              }

                              template <std::size_t N>
      basic_fill_track_styler<_size, _vertical, fill_track_colors<track_type, gradient_fill<N>>, _shape, _glow>
                              fill_gradient(canvas::color_stop const (&stops)[N]) const
                              {
                                 gradient_fill<N> g;
                                 for (std::size_t i = 0; i != N; ++i)
                                    g.stops[i] = stops[i];
                                 return {{static_cast<track_part<track_type> const&>(*this), {g}}};
                              }

      basic_fill_track_styler<_size, _vertical, Colors, _shape, true>
                              glow() const
                              { return {static_cast<Colors const&>(*this)}; }
   };

   template <unsigned size, bool vertical, typename Colors, track_shape _shape, bool _glow>
   inline view_limits basic_fill_track_styler<size, vertical, Colors, _shape, _glow>
      ::limits(basic_context const& /* ctx */) const
   {
      // A glow takes room at both sides of the track
      auto sz = float(size + 2*glow_size);
      auto min_length_ = float(min_length);
      auto p1 = vertical ? point{sz, min_length_} : point{min_length_, sz};
      auto p2 = vertical ? point{sz, full_extent} : point{full_extent, sz};
      return view_limits{p1, p2};
   }

   template <unsigned size, bool vertical, typename Colors, track_shape _shape, bool _glow>
   inline void basic_fill_track_styler<size, vertical, Colors, _shape, _glow>
      ::draw(context const& ctx)
   {
      auto& cnv = ctx.canvas;
      auto const g = float(glow_size);
      auto const b = vertical? ctx.bounds.inset(g, 0) : ctx.bounds.inset(0, g);
      detail::draw_track(cnv, b, this->track(), _shape);

      // The part to fill, from 0.0 to 1.0 along the track: from the start,
      // the bottom of a vertical track
      double from = 0.0, to = 1.0;
      if (auto rs = find_parent<range_slider_base*>(ctx))
      {
         from = rs->value_first();
         to = rs->value_second();
      }
      else if (auto s = find_parent<slider_base*>(ctx))
      {
         to = s->value();
      }
      if (to <= from)
         return;

      auto fill = b;
      if (vertical)
      {
         fill.top = b.bottom - float(to * b.height());
         fill.bottom = b.bottom - float(from * b.height());
      }
      else
      {
         fill.left = b.left + float(from * b.width());
         fill.right = b.left + float(to * b.width());
      }
      auto r = (vertical? fill.width() : fill.height()) / 2;
      auto shape = (_shape == track_shape::square)? fill : (vertical? fill.inset(0, -r) : fill.inset(-r, 0));

      // Bands and gradients are placed along the whole bar of a status
      // bar, or this track
      auto ref = b;
      if (auto pctx = find_parent_context<status_bar_base*>(ctx))
         ref = pctx->bounds;
      auto const& fill_colors = static_cast<fill_part<fill_type> const&>(*this);

      // Call f(rect, color) for each part of the fill in one color: the
      // bands, or slices of a gradient, `size` long
      auto for_each_part = [&](auto f)
      {
         if constexpr (detail::is_gradient_fill<fill_type>::value)
         {
            auto const len = vertical? shape.height() : shape.width();
            auto const step = float(size);
            for (float p = 0; p < len; p += step)
            {
               auto part = shape;
               auto mid = 0.0f;
               if (vertical)
               {
                  part.bottom = shape.bottom - p;
                  part.top = std::max(shape.top, part.bottom - step);
                  mid = (ref.bottom - (part.top + part.bottom) / 2) / ref.height();
               }
               else
               {
                  part.left = shape.left + p;
                  part.right = std::min(shape.right, part.left + step);
                  mid = ((part.left + part.right) / 2 - ref.left) / ref.width();
               }
               f(part, fill_colors.at(mid));
            }
         }
         else if constexpr (!detail::is_band_fill<fill_type>::value)
         {
            f(shape, this->fill());
         }
         else
         {
            auto const& bands = fill_colors.bands;
            float prev = 0;
            for (std::size_t i = 0; i != bands.size(); ++i)
            {
               auto band = shape;
               auto from = (i == 0)? -full_extent : prev;
               auto to = (i == bands.size()-1)? full_extent : bands[i].to;
               if (vertical)
               {
                  band.top = std::max(shape.top, ref.bottom - to * ref.height());
                  band.bottom = std::min(shape.bottom, ref.bottom - from * ref.height());
               }
               else
               {
                  band.left = std::max(shape.left, ref.left + from * ref.width());
                  band.right = std::min(shape.right, ref.left + to * ref.width());
               }
               prev = bands[i].to;
               if (band.width() > 0 && band.height() > 0)
                  f(band, bands[i].c);
            }
         }
      };

      // A gradient along the whole bar of a status bar, or this track
      auto along = [&]
      {
         return vertical?
            canvas::linear_gradient{point{0, ref.bottom}, point{0, ref.top}} :
            canvas::linear_gradient{point{ref.left, 0}, point{ref.right, 0}};
      };

      if constexpr (_glow)
      {
         // The glow: the fill, a quarter of the glow's room wider at each
         // side and blurred by a third of it, so it has all but faded where
         // the room ends. It is kept within the bounds of a status bar, or
         // this track. Bands are a gradient with hard stops, so the glow is
         // one shape.
         auto clip = ctx.bounds;
         if (auto pctx = find_parent_context<status_bar_base*>(ctx))
            clip = pctx->bounds;
         auto state = cnv.new_state();
         cnv.begin_path();
         cnv.add_rect(clip);
         cnv.clip();
         cnv.blur_style(g / 3);

         if constexpr (detail::is_gradient_fill<fill_type>::value)
         {
            auto gr = along();
            for (auto const& cs : fill_colors.stops)
               gr.add_color_stop(cs);
            cnv.fill_style(gr);
         }
         else if constexpr (detail::is_band_fill<fill_type>::value)
         {
            auto gr = along();
            float prev = 0;
            for (std::size_t i = 0; i != fill_colors.bands.size(); ++i)
            {
               auto const& band = fill_colors.bands[i];
               auto to = (i == fill_colors.bands.size()-1)? 1.0f : band.to;
               gr.add_color_stop(prev, band.c);
               gr.add_color_stop(to, band.c);
               prev = to;
            }
            cnv.fill_style(gr);
         }
         else
         {
            cnv.fill_style(this->fill());
         }

         auto const grow = g / 4;
         cnv.begin_path();
         if (_shape == track_shape::square)
            cnv.add_rect(shape.inset(-grow));
         else
            cnv.add_round_rect(shape.inset(-grow), r + grow);
         cnv.fill();
      }

      if constexpr (
         !detail::is_band_fill<fill_type>::value &&
         !detail::is_gradient_fill<fill_type>::value)
      {
         detail::draw_track(cnv, fill, this->fill(), _shape);
      }
      else
      {
         // The bands or the gradient, clipped to the fill's shape
         auto state = cnv.new_state();
         cnv.begin_path();
         if (_shape == track_shape::square)
            cnv.add_rect(shape);
         else
            cnv.add_round_rect(shape, r);
         cnv.clip();

         if constexpr (detail::is_gradient_fill<fill_type>::value)
         {
            auto gr = along();
            for (auto const& cs : fill_colors.stops)
               gr.add_color_stop(cs);
            cnv.fill_style(gr);
            cnv.fill_rect(shape);
         }
         else
         {
            for_each_part(
               [&](rect part, color c)
               {
                  cnv.fill_style(c);
                  cnv.fill_rect(part);
               }
            );
         }

         // The highlight, as draw_track draws it
         auto lwidth = r/4;
         cnv.stroke_style(colors::white.opacity(0.3));
         if (_shape == track_shape::square)
            cnv.add_rect(shape.move(-lwidth, -lwidth));
         else
            cnv.add_round_rect(shape.move(-lwidth, -lwidth), r*0.6);
         cnv.line_width(lwidth*1.5);
         cnv.stroke();
      }
   }

   template <
      unsigned size
    , bool vertical = false
    , track_shape shape = track_shape::round
   >
   inline basic_fill_track_styler<
      size, vertical
    , fill_track_colors<default_track_color, default_fill_color>
    , shape
   >
   basic_fill_track()
   {
      return {};
   }

   ////////////////////////////////////////////////////////////////////////////
   // Radial Fill Track (You can use this to place a track around a dial)
   ////////////////////////////////////////////////////////////////////////////
   template <
      std::size_t _size
    , concepts::Element Subject
    , typename Colors = fill_track_colors<default_track_color, default_fill_color>
    , track_shape _shape = track_shape::round
    , bool _glow = false
    , track_origin _origin = track_origin::start
   >
   class ELEMENTS_EMPTY_BASES radial_fill_track_styler : Colors, public radial_styler_base<_size, Subject>
   {
   public:

      static std::size_t const size = _size;
      using base_type = radial_styler_base<_size, Subject>;
      using track_type = typename Colors::track_type;
      using fill_type = typename Colors::fill_type;

                              radial_fill_track_styler(Subject subject, Colors c = {})
                               : Colors(c)
                               , base_type(std::move(subject))
                              {}

      void                    draw(context const& ctx) override;

      color                   track_color() const { return this->track(); }
      color                   fill_color() const { return this->fill(); }

      radial_fill_track_styler<_size, Subject, fill_track_colors<fixed_color, fill_type>, _shape, _glow, _origin>
                              track_color(color c) const
                              {
                                 return {this->actual_subject(), {{fixed_color{c}}, static_cast<fill_part<fill_type> const&>(*this)}};
                              }

      radial_fill_track_styler<_size, Subject, fill_track_colors<track_type, fixed_color>, _shape, _glow, _origin>
                              fill_color(color c) const
                              {
                                 return {this->actual_subject(), {static_cast<track_part<track_type> const&>(*this), {fixed_color{c}}}};
                              }

                              template <std::size_t N>
      radial_fill_track_styler<_size, Subject, fill_track_colors<track_type, band_fill<N>>, _shape, _glow, _origin>
                              fill_colors(fill_band const (&bands)[N]) const
                              {
                                 band_fill<N> b;
                                 for (std::size_t i = 0; i != N; ++i)
                                    b.bands[i] = bands[i];
                                 return {this->actual_subject(), {static_cast<track_part<track_type> const&>(*this), {b}}};
                              }

                              template <std::size_t N>
      radial_fill_track_styler<_size, Subject, fill_track_colors<track_type, gradient_fill<N>>, _shape, _glow, _origin>
                              fill_gradient(canvas::color_stop const (&stops)[N]) const
                              {
                                 gradient_fill<N> g;
                                 for (std::size_t i = 0; i != N; ++i)
                                    g.stops[i] = stops[i];
                                 return {this->actual_subject(), {static_cast<track_part<track_type> const&>(*this), {g}}};
                              }

      radial_fill_track_styler<_size, Subject, Colors, _shape, true, _origin>
                              glow() const
                              { return {this->actual_subject(), static_cast<Colors const&>(*this)}; }
   };

   // The track is a quarter of `size` thick, in the middle of the ring
   // around the subject. The fill goes from the origin up to the dial's
   // value.
   template <
      std::size_t size, concepts::Element Subject
    , typename Colors, track_shape _shape, bool _glow, track_origin _origin
   >
   inline void radial_fill_track_styler<size, Subject, Colors, _shape, _glow, _origin>
      ::draw(context const& ctx)
   {
      using namespace radial_consts;
      auto& cnv = ctx.canvas;
      auto const center = center_point(ctx.bounds);
      auto const thick = float(size) / 4;
      auto const radius = ctx.bounds.width()/2 - float(size) / 4;
      auto const round = _shape == track_shape::round;

      auto angle = [](double v) { return float(offset + (pi / 2) + v * range); };
      auto at = [&](double v)
      {
         auto a = angle(v);
         return point{center.x + radius * std::cos(a), center.y + radius * std::sin(a)};
      };

      auto arc = [&](double from, double to, color c)
      {
         cnv.begin_path();
         cnv.arc(center, radius, angle(from), angle(to));
         cnv.stroke_style(c);
         cnv.stroke();
      };

      // A part of the track as one shape, with round ends where asked, so
      // nothing is drawn twice: a translucent color stays even. h is half
      // its thickness.
      auto segment = [&](
         double from, double to, color c, bool round_from, bool round_to
       , float h)
      {
         auto const a0 = angle(from);
         auto const a1 = angle(to);
         cnv.begin_path();
         cnv.arc(center, radius + h, a0, a1);
         if (round_to)
            cnv.arc(at(to), h, a1, a1 + float(pi));
         cnv.arc(center, radius - h, a1, a0, true);
         if (round_from)
            cnv.arc(at(from), h, a0 + float(pi), a0 + float(2 * pi));
         cnv.close_path();
         cnv.fill_style(c);
         cnv.fill();
      };

      {
         auto state = cnv.new_state();
         cnv.line_width(thick);
         cnv.line_cap(round? canvas::round : canvas::butt);
         arc(0, 1, this->track());
      }

      double const value = [&]
      {
         auto dial = find_parent<basic_dial*>(ctx);
         return dial? dial->value() : 1.0;
      }();
      double const origin = (_origin == track_origin::center)? 0.5 : 0.0;
      double const lo = std::min(origin, value);
      double const to = std::max(origin, value);

      if (to > lo)
      {
         // Call f(from, to, color) for each part of the fill in one color:
         // the bands, or slices of a gradient, along the dial's travel
         auto for_each_part = [&](auto f, int slices = 64)
         {
            (void) slices;   // gradients only
            if constexpr (detail::is_gradient_fill<fill_type>::value)
            {
               auto const& g = static_cast<fill_part<fill_type> const&>(*this);
               for (int i = 0; i != slices; ++i)
               {
                  double from = std::max(lo, double(i) / slices);
                  double end = std::min(to, double(i + 1) / slices);
                  if (end <= from)
                     continue;
                  f(from, end, g.at(float((from + end) / 2)));
               }
            }
            else if constexpr (detail::is_band_fill<fill_type>::value)
            {
               auto const& bands = static_cast<fill_part<fill_type> const&>(*this).bands;
               double prev = 0;
               for (std::size_t i = 0; i != bands.size() && prev < to; ++i)
               {
                  double end = (i == bands.size()-1)? 1.0 : bands[i].to;
                  if (end > lo)
                     f(std::max(lo, prev), std::min(to, end), bands[i].c);
                  prev = end;
               }
            }
            else
            {
               f(lo, to, this->fill());
            }
         };

         // Round ends at the two ends of the fill only. An opaque part
         // overlaps the next a bit, so no seam shows.
         auto part = [&](double from, double end, color c)
         {
            bool const last = end == to;
            if (!last && c.alpha >= 1)
               end = std::min(to, end + 0.002);
            segment(from, end, c, round && from == lo, round && last, thick / 2);
         };

         if constexpr (_glow)
         {
            // The glow: the fill, a sixth of its thickness wider at each
            // side and blurred by as much, so it has all but faded where
            // the room outside the track, half its thickness, ends. It is
            // kept within bounds. The parts are blurred one by one and
            // added up in a group, which comes to the blur of the whole.
            auto glow_state = cnv.new_state();
            cnv.begin_path();
            cnv.add_rect(ctx.bounds);
            cnv.clip();
            auto glow_group = cnv.new_group(1);
            cnv.composite_op(canvas::lighter);
            cnv.blur_style(thick / 6);
            for_each_part(
               [&](double from, double end, color c)
               {
                  segment(
                     from, end, c, round && from == lo, round && end == to
                   , thick * 2 / 3);
               }
             , 16
            );
         }

         for_each_part(part);
      }

      // Draw the subject
      base_type::draw(ctx);
   }

   template <
      std::size_t size
    , track_shape shape = track_shape::round
    , track_origin origin = track_origin::start
    , concepts::Element Subject
   >
   inline radial_fill_track_styler<
      size, remove_cvref_t<Subject>
    , fill_track_colors<default_track_color, default_fill_color>
    , shape, false, origin
   >
   radial_fill_track(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }
}

#endif
