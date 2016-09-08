/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_SLIDER_AUGUST_29_2016)
#define PHOTON_GUI_LIB_WIDGET_SLIDER_AUGUST_29_2016

#include <photon/widget/tracker.hpp>
#include <photon/view.hpp>
#include <photon/support.hpp>
#include <cmath>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Sliders
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class slider_base : public tracker<>
   {
   public:
                              slider_base(double init_value)
                               : _value(init_value)
                              {}

      virtual rect            limits(basic_context const& ctx) const;
      virtual void            draw(context const& ctx);
      virtual void            layout(context const& ctx);

      virtual bool            scroll(context const& ctx, point p);
      virtual void            begin_tracking(context const& ctx, info& track_info);
      virtual void            keep_tracking(context const& ctx, info& track_info);
      virtual void            end_tracking(context const& ctx, info& track_info);

      virtual double          value() const        { return _value; }
      virtual void            value(double val)    { _value = val; }

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

   ////////////////////////////////////////////////////////////////////////////////////////////////
   template <typename Indicator, typename Body, typename Base = slider_base>
   class basic_slider : public Base
   {
   public:
                              inline basic_slider(Indicator&& indicator, Body&& body, double init_value)
                               : Base(init_value)
                               , _indicator(std::forward<Indicator>(indicator))
                               , _body(std::forward<Body>(body))
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
      slider_base
   >
   slider(Indicator&& indicator, Body&& body, double init_value = 0.0)
   {
      return { std::forward<Indicator>(indicator), std::forward<Body>(body), init_value };
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Selectors
   ////////////////////////////////////////////////////////////////////////////////////////////////
   template <size_t num_states>
   class selector_base : public slider_base
   {
   public:

      static_assert(num_states > 1, "Error: not enough states.");

      using slider_base::slider_base;
      using slider_base::value;

      virtual bool         scroll(context const& ctx, point p);
      virtual void         value(double val);
   };

   template <size_t num_states>
   inline bool selector_base<num_states>::scroll(context const& ctx, point p)
   {
      // We don't allow selector move via the scroll wheel.
      return false;
   }

   template <size_t num_states>
   inline void selector_base<num_states>::value(double val)
   {
      constexpr auto max = num_states-1;
      slider_base::value(std::round(val * max) / max);
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