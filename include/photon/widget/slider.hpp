/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_SLIDER_AUGUST_29_2016)
#define PHOTON_GUI_LIB_WIDGET_SLIDER_AUGUST_29_2016

#include <photon/widget/tracker.hpp>
#include <photon/support.hpp>
#include <functional>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Sliders
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class slider : public tracker
   {
   public:
                           slider(widget_ptr indicator, widget_ptr body, double init_value = 0.0);
      virtual              ~slider() {}

                           slider(slider&& rhs) = default;
      slider&              operator=(slider&& rhs) = default;

      virtual rect         limits(basic_context const& ctx) const;
      virtual void         draw(context const& ctx);

      virtual void         begin_tracking(context const& ctx, info& track_info);
      virtual void         keep_tracking(context const& ctx, info& track_info);
      virtual void         end_tracking(context const& ctx, info& track_info);

      double               value() const        { return _value; }
      void                 value(double value_) { _value = value_; }
      widget_ptr           indicator() const    { return _indicator; }
      widget_ptr           body() const         { return _body; }

   protected:

      rect                 indicator_bounds(context const& ctx) const;
      virtual double       value_from_point(context const& ctx, point p);

   private:

      double               _value;
      widget_ptr           _indicator;
      widget_ptr           _body;
      mutable bool         _is_horiz;
   };
}

#endif