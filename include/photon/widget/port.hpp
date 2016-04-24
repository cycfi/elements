/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_PORT_APRIL_24_2016)
#define PHOTON_GUI_LIB_WIDGET_PORT_APRIL_24_2016

#include <photon/widget/widget.hpp>
#include <memory>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Port widgets
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class port_widget : public proxy
   {
   public:

      port_widget(std::shared_ptr<widget> subject)
       : proxy(subject)
       , _halign(0.5)
       , _valign(0.5)
      {}

      ~port_widget() {}

      virtual rect   limits(basic_context const& ctx) const;
      virtual void   prepare_subject(context& ctx);
      virtual void   draw(context const& ctx);

   private:

      double         _halign;
      double         _valign;
   };

   inline widget_ptr port(std::shared_ptr<widget> subject)
   {
      return widget_ptr{ new port_widget{ subject } };
   }
}

#endif