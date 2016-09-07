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
   class slider : public tracker<>
   {
   public:
                           slider(widget_ptr indicator, widget_ptr body, double init_value = 0.0);
                           slider(slider&& rhs) = default;
      slider&              operator=(slider&& rhs) = default;

      virtual rect         limits(basic_context const& ctx) const;
      virtual void         draw(context const& ctx);

      virtual bool         scroll(context const& ctx, point p);
      virtual void         begin_tracking(context const& ctx, info& track_info);
      virtual void         keep_tracking(context const& ctx, info& track_info);
      virtual void         end_tracking(context const& ctx, info& track_info);

      virtual double       value() const        { return _value; }
      virtual void         value(double val)    { _value = val; }

      rect                 indicator_bounds(context const& ctx) const;
      virtual double       value_from_point(context const& ctx, point p);
      widget_ptr           indicator() const    { return _indicator; }
      widget_ptr           body() const         { return _body; }

   private:

      double               _value;
      widget_ptr           _indicator;
      widget_ptr           _body;
      mutable bool         _is_horiz;
   };

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Selectors
   ////////////////////////////////////////////////////////////////////////////////////////////////
   template <size_t num_states>
   class selector : public slider
   {
   public:

      static_assert(num_states > 1, "Error: not enough states.");

      using slider::slider;
      using slider::value;

      virtual bool         scroll(context const& ctx, point p);
      virtual void         value(double val);
   };

   template <size_t num_states>
   inline bool selector<num_states>::scroll(context const& ctx, point p)
   {
      // We don't allow selector move via the scroll wheel.
      return false;
   }

   template <size_t num_states>
   inline void selector<num_states>::value(double val)
   {
      constexpr auto max = num_states-1;
      slider::value(std::round(val * max) / max);
   }
}

#endif