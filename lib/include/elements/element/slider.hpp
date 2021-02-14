/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_SLIDER_AUGUST_29_2016)
#define ELEMENTS_SLIDER_AUGUST_29_2016

#include <elements/element/proxy.hpp>
#include <elements/element/tracker.hpp>
#include <elements/view.hpp>
#include <elements/support.hpp>
#include <infra/support.hpp>

#include <cmath>
#include <functional>
#include <type_traits>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Sliders
   ////////////////////////////////////////////////////////////////////////////
   class slider_base : public tracker<>, public receiver<double>
   {
   public:

                              slider_base(double init_value)
                               : _value(init_value)
                              {}

      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;
      void                    layout(context const& ctx) override;

      bool                    scroll(context const& ctx, point dir, point p) override;
      void                    begin_tracking(context const& ctx, tracker_info& track_info) override;
      void                    keep_tracking(context const& ctx, tracker_info& track_info) override;
      void                    end_tracking(context const& ctx, tracker_info& track_info) override;

      double                  value() const override;
      void                    value(double val) override;
      void                    edit(view& view_, double val) override;
      virtual void            edit_value(double val) { value(val);}

      rect                    track_bounds(context const& ctx) const;
      rect                    thumb_bounds(context const& ctx) const;
      virtual double          value_from_point(context const& ctx, point p);

      virtual element const&  thumb() const = 0;
      virtual element&        thumb() = 0;
      virtual element const&  track() const = 0;
      virtual element&        track() = 0;

   private:

      double                  _value;
      mutable bool            _is_horiz = false;
   };

   inline void slider_base::edit(view& view_, double val)
   {
      edit_value(val);
      receiver<double>::notify_edit(view_);
   }

   class basic_slider_base : public slider_base
   {
   public:

      using slider_function = std::function<void(double pos)>;
      using slider_base::slider_base;

      slider_function         on_change;
      void                    edit_value(double val) override;
   };

   ////////////////////////////////////////////////////////////////////////////
   template <typename Thumb, typename Track, typename Base = basic_slider_base>
   class basic_slider : public Base
   {
   public:

      using thumb_type = typename std::decay<Thumb>::type;
      using track_type = typename std::decay<Track>::type;

                              inline basic_slider(Thumb&& thumb, Track&& track, double init_value)
                               : Base(init_value)
                               , _thumb(std::forward<Thumb>(thumb))
                               , _body(std::forward<Track>(track))
                              {}

      element const&          thumb() const override { return _thumb; }
      element&                thumb()       override { return _thumb; }
      element const&          track() const override { return _body; }
      element&                track()       override { return _body; }

   private:

      thumb_type              _thumb;
      track_type              _body;
   };

   template <typename Thumb, typename Track>
   inline basic_slider<Thumb, Track, basic_slider_base>
   slider(Thumb&& thumb, Track&& track, double init_value = 0.0)
   {
      return {
         std::forward<Thumb>(thumb),
         std::forward<Track>(track),
         init_value
      };
   }

   ////////////////////////////////////////////////////////////////////////////
   // Selectors
   ////////////////////////////////////////////////////////////////////////////
   class basic_selector_base : public slider_base
   {
   public:

      using selector_function = std::function<void(size_t pos)>;
      using slider_function = std::function<void(double pos)>;
      using slider_base::slider_base;

      selector_function       on_change;
      void                    select(size_t val);
   };

   template <size_t num_states>
   class selector_base : public basic_selector_base
   {
   public:

      static_assert(num_states > 1, "Error: not enough states.");

      using basic_selector_base::basic_selector_base;

      bool                 scroll(context const& ctx, point dir, point p) override;
      void                 value(double val) override;
   };

   template <size_t num_states>
   inline bool selector_base<num_states>::scroll(
      context const& /* ctx */, point /* dir */, point /* p */)
   {
      // We don't allow selector move via the scroll wheel.
      return false;
   }

   template <size_t num_states>
   inline void selector_base<num_states>::value(double val)
   {
      constexpr auto    max = num_states-1;
      auto const        state = std::round(val * max);
      basic_selector_base::value(state / max);
      basic_selector_base::select(state);
   }

   template <size_t num_states, typename Thumb, typename Track>
   inline basic_slider<Thumb, Track, selector_base<num_states>>
   selector(Thumb&& thumb, Track&& track, double init_value = 0.0)
   {
      return {
         std::forward<Thumb>(thumb),
         std::forward<Track>(track),
         init_value
      };
   }

   ////////////////////////////////////////////////////////////////////////////
   // Basic Thumb (You can use this as the slider's thumb)
   ////////////////////////////////////////////////////////////////////////////
   template <unsigned _size>
   class basic_thumb_element : public element
   {
   public:

      static unsigned const size = _size;

                              basic_thumb_element(color c = colors::black)
                               : _color(c)
                              {}

      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;

   private:

      color                   _color;
   };

   template <unsigned size>
   inline view_limits basic_thumb_element<size>
      ::limits(basic_context const& /* ctx */) const
   {
	  auto pt = point{ float(size), float(size) };
	  return view_limits{ pt, pt };
   }

   template <unsigned size>
   inline void basic_thumb_element<size>::draw(context const& ctx)
   {
      auto& thm = get_theme();
      auto& cnv = ctx.canvas;
      auto  indicator_color = thm.indicator_color.level(1.5);
      auto  cp = circle{ center_point(ctx.bounds), size/2.0f };

      draw_thumb(cnv, cp, _color, indicator_color);
   }

   template <unsigned size>
   inline basic_thumb_element<size> basic_thumb(color c = colors::black)
   {
      return {c};
   }

   ////////////////////////////////////////////////////////////////////////////
   // Basic Track (You can use this as the slider's track)
   ////////////////////////////////////////////////////////////////////////////
   template <unsigned _size, bool _vertical = false>
   class basic_track_element : public element
   {
   public:

      static unsigned const size = _size;
      static bool const vertical = _vertical;
      static unsigned const min_length = 64;

                              basic_track_element(color c = colors::black)
                               : _color(c)
                              {}

      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;

   private:

      color                   _color;
   };

   template <unsigned size, bool vertical>
   inline view_limits basic_track_element<size, vertical>
      ::limits(basic_context const& /* ctx */) const
   {
	  auto sz = float(size);
	  auto min_length_ = float(min_length);
	  auto p1 = vertical ? point{ sz, min_length_ } : point{ min_length_, sz };
	  auto p2 = vertical ? point{ sz, full_extent } : point{ full_extent, sz };
	  return view_limits{ p1, p2 };
   }

   template <unsigned size, bool vertical>
   inline void basic_track_element<size, vertical>
      ::draw(context const& ctx)
   {
      draw_track(ctx.canvas, ctx.bounds);
   }

   template <unsigned size, bool vertical = false>
   inline basic_track_element<size, vertical>
   basic_track(color c = colors::black)
   {
      return {c};
   }

   ////////////////////////////////////////////////////////////////////////////
   // Slider Element Base (common base class for slider elements)
   ////////////////////////////////////////////////////////////////////////////
   template <int _size, typename Subject>
   class slider_element_base : public proxy<Subject>
   {
   public:

      static unsigned const size = _size;
      using base_type = proxy<Subject>;
      using base_type::base_type;

      view_limits             limits(basic_context const& ctx) const override;
      void                    prepare_subject(context& ctx) override;
   };

   template <int size, typename Subject>
   inline view_limits
   slider_element_base<size, Subject>::limits(basic_context const& ctx) const
   {
      auto sl = this->subject().limits(ctx);
      if (sl.min.x < sl.min.y) // is vertical?
      {
         sl.min.x += size;
         sl.max.x += size;
         clamp_max(sl.max.x, full_extent);
      }
      else
      {
         sl.min.y += size;
         sl.max.y += size;
         clamp_max(sl.max.y, full_extent);
      }
      return sl;
   }

   template <int size, typename Subject>
   inline void
   slider_element_base<size, Subject>::prepare_subject(context& ctx)
   {
      if (ctx.bounds.width() < ctx.bounds.height()) // is vertical?
      {
         ctx.bounds.left += size/2;
         ctx.bounds.right -= size/2;
      }
      else
      {
         ctx.bounds.top += size/2;
         ctx.bounds.bottom -= size/2;
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   // Slider Marks (You can use this to place tick marks on slider)
   ////////////////////////////////////////////////////////////////////////////
   template <
      std::size_t _size, std::size_t _num_divs
    , std::size_t _major_divs, typename Subject>
   class slider_marks_element : public slider_element_base<_size, Subject>
   {
   public:

      static_assert(_major_divs > 0, "Major divisions must be greater than zero");

      using base_type = slider_element_base<_size, Subject>;
      using base_type::base_type;

      void                    draw(context const& ctx) override;
   };

   void draw_slider_marks(
      canvas& cnv, rect bounds, float size, std::size_t major_divs
    , std::size_t minor_divs, color c);

   template <
      std::size_t _size, std::size_t _num_divs
    , std::size_t _major_divs, typename Subject>
   inline void
   slider_marks_element<_size, _num_divs, _major_divs, Subject>
      ::draw(context const& ctx)
   {
      // Draw linear lines
      draw_slider_marks(
         ctx.canvas, ctx.bounds, _size, _num_divs
       , _major_divs, colors::light_gray);

      // Draw the subject
      base_type::draw(ctx);
   }

   template <
      std::size_t _size, std::size_t _num_divs = 50
    , std::size_t _major_divs = 10, typename Subject>
   inline slider_marks_element<_size, _num_divs, _major_divs, remove_cvref_t<Subject>>
   slider_marks(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }

   ////////////////////////////////////////////////////////////////////////////
   // Slider Labels (You can use this to place slider labels with sliders)
   ////////////////////////////////////////////////////////////////////////////
   template <int _size, typename Subject, std::size_t num_labels>
   class slider_labels_element
    : public slider_element_base<abs(_size), Subject>
   {
   public:

      constexpr static int const size = _size;
      using base_type = slider_element_base<abs(_size), Subject>;
      using string_array = std::array<std::string, num_labels>;

                              slider_labels_element(
                                 Subject subject
                               , float font_size
                              )
                               : base_type(std::move(subject))
                               , _font_size(font_size)
                              {}

      void                    draw(context const& ctx) override;

      string_array            _labels;
      float                   _font_size;
   };

   void draw_slider_labels(
      canvas& cnv
    , rect bounds
    , float size
    , float font_size
    , std::string const labels[]
    , std::size_t _num_labels
   );

   template <int size, typename Subject, std::size_t num_labels>
   inline void
   slider_labels_element<size, Subject, num_labels>::draw(context const& ctx)
   {
      // Draw the subject
      base_type::draw(ctx);

      // Draw the labels
      draw_slider_labels(
         ctx.canvas, ctx.bounds, size
       , _font_size, _labels.data(), num_labels);
   }

   template <int size, typename Subject, typename... S>
   inline slider_labels_element<size, remove_cvref_t<Subject>, sizeof...(S)>
   slider_labels(Subject&& subject, float font_size, S&&... s)
   {
      auto r = slider_labels_element<size, remove_cvref_t<Subject>, sizeof...(S)>
         {std::forward<Subject>(subject), font_size};
      r._labels = {{ std::move(s)... }};
      return r;
   }
}}

#endif
