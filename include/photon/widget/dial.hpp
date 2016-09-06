/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_DIAL_AUGUST_30_2016)
#define PHOTON_GUI_LIB_WIDGET_DIAL_AUGUST_30_2016

#include <photon/widget/proxy.hpp>
#include <photon/widget/tracker.hpp>
#include <photon/support.hpp>
#include <functional>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Dials
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class dial_base : public tracker<proxy_base>
   {
   public:
                           dial_base(double init_value = 0.0);
      virtual              ~dial_base() {}

      virtual void         prepare_subject(context& ctx);

      virtual bool         scroll(context const& ctx, point p);
      virtual void         begin_tracking(context const& ctx, info& track_info);
      virtual void         keep_tracking(context const& ctx, info& track_info);
      virtual void         end_tracking(context const& ctx, info& track_info);

      double               value() const        { return _value; }
      void                 value(double val);
      virtual double       value_from_point(context const& ctx, point p);

   private:

      double               _value;
   };

   template <typename Subject>
   inline proxy<typename std::decay<Subject>::type, dial_base>
   dial(Subject&& subject, double init_value = 0.0)
   {
      return { std::forward<Subject>(subject), init_value };
   }
}

#endif