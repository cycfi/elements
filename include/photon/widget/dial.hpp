/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_DIAL_AUGUST_30_2016)
#define PHOTON_GUI_LIB_WIDGET_DIAL_AUGUST_30_2016

#include <photon/widget/analog.hpp>
#include <photon/support.hpp>
#include <functional>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Dials
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class dial : public analog
   {
   public:
                           dial(widget_ptr indicator, widget_ptr body, double init_value = 0.0)
                            : analog(indicator, body, init_value)
                           {}
      virtual              ~dial() {}

                           dial(dial&& rhs) = default;
      dial&                operator=(dial&& rhs) = default;

      using analog::value;

   protected:

      virtual point        indicator_point(context const& ctx) const;
      virtual void         prepare_indicator(context& ctx);
      rect                 body_bounds(context const& ctx) const;
      virtual void         prepare_body(context& ctx);
      virtual double       value(context const& ctx, point p);
      virtual void         begin_tracking(context const& ctx, info& track_info);
   };
}

#endif