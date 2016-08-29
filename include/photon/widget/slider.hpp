/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_SLIDER_APRIL_16_2016)
#define PHOTON_GUI_LIB_WIDGET_SLIDER_APRIL_16_2016

#include <photon/widget/analog.hpp>
#include <photon/support.hpp>
#include <functional>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Sliders
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class slider : public analog
   {
   public:
                           slider(widget_ptr indicator, widget_ptr body, double init_value = 0.0)
                            : analog(indicator, body, init_value)
                           {}
       
                           slider(slider&& rhs) = default;
      slider&              operator=(slider&& rhs) = default;

      virtual              ~slider() {}

      using analog::value;

      virtual rect         limits(basic_context const& ctx) const;

   protected:

      virtual void         prepare_indicator(context& ctx);
      virtual void         prepare_body(context& ctx);
      virtual double       value(context const& ctx, point p);
   };
}

#endif