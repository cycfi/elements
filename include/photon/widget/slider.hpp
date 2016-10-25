/*=======================================================================================
   Copyright (c) 2016 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=======================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_SLIDER_AUGUST_29_2016)
#define PHOTON_GUI_LIB_WIDGET_SLIDER_AUGUST_29_2016

#include <photon/widget/tracker.hpp>
#include <photon/view.hpp>
#include <photon/support.hpp>

#include <cmath>
#include <functional>

namespace photon
{
   //////////////////////////////////////////////////////////////////////////////////////
   // Sliders
   //////////////////////////////////////////////////////////////////////////////////////
   class slider_base : public tracker<>
   {
   public:

      using tracker<>::value;

                              slider_base(double init_value)
                               : _value(init_value)
                              {}

      virtual widget_limits   limits(basic_context const& ctx) const;
      virtual void            draw(context const& ctx);
      virtual void            layout(context const& ctx);

      virtual bool            scroll(context const& ctx, point dir, point p);
      virtual void            begin_tracking(context const& ctx, info& track_info);
      virtual void            keep_tracking(context const& ctx, info& track_info);
      virtual void            end_tracking(context const& ctx, info& track_info);

      double                  value() const;
      virtual void            value(double val);

      rect                    body_bounds(context const& ctx) const;
      rect                    indicator_bounds(context const& ctx) const;
      virtual double          value_from_point(context const& ctx, point p);

      virtual widget const&   indicator() const = 0;
      virtual widget&         indicator() = 0;
      virtual widget const&   body() const = 0;
      virtual widget&         body() = 0;

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

   //////////////////////////////////////////////////////////////////////////////////////
   template <typename Indicator, typename Body, typename Base = basic_slider_base>
   class basic_slider : public Base
   {
   public:
                              inline basic_slider(Indicator&& indicator, Body&& body, double init_value)
                               : Base(init_value)
                               , _indicator(std::forward<Indicator>(indicator))
                               , _body(std::forward<Body>(body))
                              {}

                              inline basic_slider(Indicator const& indicator, Body const& body, double init_value)
                               : Base(init_value)
                               , _indicator(indicator)
                               , _body(body)
                              {}

      virtual widget const&   indicator() const  { return _indicator; }
      virtual widget&         indicator()        { return _indicator; }
      virtual widget const&   body() const       { return _body; }
      virtual widget&         body()             { return _body; }

   private:

      Indicator               _indicator;
      Body                    _body;
   };

   template <typename Indicator, typename Body>
   inline basic_slider<
      typename std::decay<Indicator>::type,
      typename std::decay<Body>::type,
      basic_slider_base
   >
   slider(Indicator&& indicator, Body&& body, double init_value = 0.0)
   {
      return { std::forward<Indicator>(indicator), std::forward<Body>(body), init_value };
   }

   //////////////////////////////////////////////////////////////////////////////////////
   // Selectors
   //////////////////////////////////////////////////////////////////////////////////////
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
   inline bool selector_base<num_states>::scroll(context const& ctx, point dir, point p)
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

   template <size_t num_states, typename Indicator, typename Body>
   inline basic_slider<
      typename std::decay<Indicator>::type,
      typename std::decay<Body>::type,
      selector_base<num_states>
   >
   selector(Indicator&& indicator, Body&& body, double init_value = 0.0)
   {
      return { std::forward<Indicator>(indicator), std::forward<Body>(body), init_value };
   }
}

#endif