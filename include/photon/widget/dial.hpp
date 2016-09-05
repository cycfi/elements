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
                           dial(widget_ptr image, double init_value = 0.0);
      virtual              ~dial() {}

                           dial(dial&& rhs) = default;
      dial&                operator=(dial&& rhs) = default;

      virtual rect         limits(basic_context const& ctx) const;
      virtual void         draw(context const& ctx);
      virtual void         begin_tracking(context const& ctx, info& track_info);
      virtual void         keep_tracking(context const& ctx, info& track_info);
      virtual void         end_tracking(context const& ctx, info& track_info);

      double               value() const        { return _value; }
      void                 value(double val);
      virtual double       value_from_point(context const& ctx, point p);

   private:

      double               _value;
      widget_ptr           _image;
   };
}

#endif