/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_DIAL_AUGUST_30_2016)
#define PHOTON_GUI_LIB_WIDGET_DIAL_AUGUST_30_2016

#include <photon/widget/tracker.hpp>
#include <photon/support.hpp>
#include <functional>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Dials
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class dial : public tracker
   {
   public:
                           dial(widget_ptr indicator, widget_ptr body, double init_value = 0.0);
      virtual              ~dial() {}

                           dial(dial&& rhs) = default;
      dial&                operator=(dial&& rhs) = default;

      virtual void         draw(context const& ctx);
      virtual void         begin_tracking(context const& ctx, info& track_info);
      virtual void         keep_tracking(context const& ctx, info& track_info);
      virtual void         end_tracking(context const& ctx, info& track_info);

      double               value() const        { return _value; }
      void                 value(double value_) { _value = value_; }

   protected:

      virtual point        indicator_point(context const& ctx) const;
      virtual void         prepare_indicator(context& ctx);
      rect                 body_bounds(context const& ctx) const;
      virtual void         prepare_body(context& ctx);
      virtual double       value(context const& ctx, point p);

   private:

      double               _value;
      widget_ptr           _indicator;
      widget_ptr           _body;
   };
}

#endif