/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_PHOTON_GUI_LIB_WIDGET_SLIDER_AUGUST_29_2016)
#define CYCFI_PHOTON_GUI_LIB_WIDGET_SLIDER_AUGUST_29_2016

#include <photon/element/proxy.hpp>
#include <photon/element/tracker.hpp>
#include <photon/view.hpp>
#include <photon/support.hpp>

#include <cmath>
#include <functional>
#include <type_traits>

namespace cycfi { namespace photon
{
   ////////////////////////////////////////////////////////////////////////////
   // Sliders
   ////////////////////////////////////////////////////////////////////////////
   class slider_base : public tracker<>
   {
   public:

      using tracker<>::value;

                              slider_base(double init_value)
                               : _value(init_value)
                              {}

      virtual view_limits     limits(basic_context const& ctx) const;
      virtual void            draw(context const& ctx);
      virtual void            layout(context const& ctx);

      virtual bool            scroll(context const& ctx, point dir, point p);
      virtual void            begin_tracking(context const& ctx, info& track_info);
      virtual void            keep_tracking(context const& ctx, info& track_info);
      virtual void            end_tracking(context const& ctx, info& track_info);

      double                  value() const;
      virtual void            value(double val);

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

   class basic_slider_base : public slider_base
   {
   public:

      using slider_function = std::function<void(double pos)>;
      using slider_base::slider_base;
      using slider_base::value;

      slider_function         on_change;
      virtual void            value(double val);
   };

   ////////////////////////////////////////////////////////////////////////////
   template <typename Thumb, typename Track, typename Base = basic_slider_base>
   class basic_slider : public Base
   {
   public:
                              inline basic_slider(Thumb&& thumb, Track&& track, double init_value)
                               : Base(init_value)
                               , _thumb(std::forward<Thumb>(thumb))
                               , _body(std::forward<Track>(track))
                              {}

                              inline basic_slider(Thumb const& thumb, Track const& track, double init_value)
                               : Base(init_value)
                               , _thumb(thumb)
                               , _body(track)
                              {}

      virtual element const&  thumb() const  { return _thumb; }
      virtual element&        thumb()        { return _thumb; }
      virtual element const&  track() const   { return _body; }
      virtual element&        track()         { return _body; }

   private:

      Thumb                   _thumb;
      Track                   _body;
   };

   template <typename Thumb, typename Track>
   inline basic_slider<
      typename std::decay<Thumb>::type,
      typename std::decay<Track>::type,
      basic_slider_base
   >
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
      using slider_base::value;

      selector_function       on_change;
      void                    select(size_t val);
   };

   template <size_t num_states>
   class selector_base : public basic_selector_base
   {
   public:

      static_assert(num_states > 1, "Error: not enough states.");

      using basic_selector_base::basic_selector_base;
      using basic_selector_base::value;

      virtual bool         scroll(context const& ctx, point dir, point p);
      virtual void         value(double val);
      virtual void         value(int val);
   };

   template <size_t num_states>
   inline bool selector_base<num_states>::scroll(
      context const& ctx, point dir, point p)
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

   template <size_t num_states>
   inline void selector_base<num_states>::value(int val)
   {
      value(double(val) * (num_states-1));
   }

   template <size_t num_states, typename Thumb, typename Track>
   inline basic_slider<
      typename std::decay<Thumb>::type,
      typename std::decay<Track>::type,
      selector_base<num_states>
   >
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

      virtual view_limits     limits(basic_context const& ctx) const;
      virtual void            draw(context const& ctx);

   private:

      color                   _color;
   };

   template <unsigned size>
   inline view_limits basic_thumb_element<size>
      ::limits(basic_context const& ctx) const
   {
      return { { size, size }, { size, size } };
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

      virtual view_limits     limits(basic_context const& ctx) const;
      virtual void            draw(context const& ctx);

   private:

      color                   _color;
   };

   template <unsigned size, bool vertical>
   inline view_limits basic_track_element<size, vertical>
      ::limits(basic_context const& ctx) const
   {
      return vertical?
         view_limits{ { size, min_length }, { size, full_extent } } :
         view_limits{ { min_length, size }, { full_extent, size } }
         ;
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

                              slider_element_base(Subject&& subject)
                               : base_type(std::move(subject))
                              {}

                              slider_element_base(Subject const& subject)
                               : base_type(subject)
                              {}

      virtual view_limits     limits(basic_context const& ctx) const;
      virtual void            prepare_subject(context& ctx);
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
   template <std::size_t _size, typename Subject>
   class slider_marks_element : public slider_element_base<_size, Subject>
   {
   public:

      static unsigned const size = _size;
      using base_type = slider_element_base<_size, Subject>;
      using base_type::base_type;

      virtual void            draw(context const& ctx);
   };

   template <std::size_t size, typename Subject>
   inline void
   slider_marks_element<size, Subject>::draw(context const& ctx)
   {
      void draw_slider_marks(canvas& cnv, rect bounds, float size, color c);

      // Draw radial lines
      draw_slider_marks(ctx.canvas, ctx.bounds, size, colors::light_gray);

      // Draw the subject
      base_type::draw(ctx);
   }

   template <std::size_t size, typename Subject>
   inline slider_marks_element<size, Subject>
   slider_marks(Subject&& subject)
   {
      return {std::move(subject)};
   }

   ////////////////////////////////////////////////////////////////////////////
   // Slider Labels (You can use this to place slider labels with sliders)
   ////////////////////////////////////////////////////////////////////////////
   template <int _size, typename Subject, std::size_t num_labels>
   class slider_labels_element
    : public slider_element_base<abs(_size), Subject>
   {
   public:

      static std::size_t const size = _size;
      using base_type = slider_element_base<abs(_size), Subject>;
      using string_array = std::array<std::string, num_labels>;

                              slider_labels_element(Subject&& subject, float font_size)
                               : base_type(std::move(subject))
                               , _font_size(font_size)
                              {}

                              slider_labels_element(Subject const& subject, float font_size)
                               : base_type(subject)
                               , _font_size(font_size)
                              {}

      virtual void            prepare_subject(context& ctx);
      virtual void            draw(context const& ctx);

      string_array            _labels;
      float                   _font_size;
   };

   template <int size, typename Subject, std::size_t num_labels>
   inline void
   slider_labels_element<size, Subject, num_labels>::prepare_subject(context& ctx)
   {
      bool reverse = size < 0;
      int size_ = abs(size);

      if (ctx.bounds.width() < ctx.bounds.height()) // is vertical?
      {
         if (reverse)
            ctx.bounds.left += size_;
         else
            ctx.bounds.right -= size_;
      }
      else
      {
         if (reverse)
            ctx.bounds.top += size_;
         else
            ctx.bounds.bottom -= size_;
      }
   }

   template <int size, typename Subject, std::size_t num_labels>
   inline void
   slider_labels_element<size, Subject, num_labels>::draw(context const& ctx)
   {
      void draw_slider_labels(
         canvas& cnv
       , rect bounds
       , float size
       , point thumb_size
       , float font_size
       , std::string const labels[]
       , std::size_t _num_labels
      );

      // Draw the subject
      base_type::draw(ctx);

      auto thl = static_cast<slider_base const&>(this->subject()).thumb().limits(ctx);
      CYCFI_ASSERT( // assert that the thumb is not resizable
         (thl.min.x == thl.max.x && thl.min.y == thl.max.y),
         "Error. The slider thumb should not be resizable."
      );

      // Draw the labels
      draw_slider_labels(
         ctx.canvas, ctx.bounds, size, point{ thl.max.x, thl.max.y }
       , _font_size, _labels.data(), num_labels);
   }

   template <int size, typename Subject, typename... S>
   inline slider_labels_element<size, Subject, sizeof...(S)>
   slider_labels(Subject&& subject, float font_size, S&&... s)
   {
      static_assert(std::is_base_of<slider_base, Subject>::value,
         "Error! Subject must be a slider"
      );

      auto r = slider_labels_element<size, Subject, sizeof...(S)>
         {std::move(subject), font_size};
      r._labels = {{ std::move(s)... }};
      return r;
   }
}}

#endif
