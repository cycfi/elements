/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

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

namespace cycfi::elements
{
   namespace colors = cycfi::artist::colors;

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
}

#endif
