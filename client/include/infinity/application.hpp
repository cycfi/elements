/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(INFINITY_APP_SEPTEMBER_11_2016)
#define INFINITY_APP_SEPTEMBER_11_2016

#include <photon/widget.hpp>
#include <infinity/pickup.hpp>

namespace infinity
{
   using photon::view;
   using photon::reference;
   using photon::basic_slider_base;
   using photon::basic_selector_base;
   using photon::dial_base;
   using photon::basic_button;

   class application : public photon::application
   {
   public:
                        application(view& view_);

      void              init_defaults();
      void              pickup_enable(int which, bool enable);
      void              pickup_type(int which, pickup::type type_);
      void              pickup_phase(int which, bool in_phase);
      void              pickup_frequency(int which, double f);
      void              pickup_resonance(int which, double q);
      void              pickup_level(int which, double val);
      void              pickup_position(int which, double val);
      void              pickup_slant(int which, double val);

   private:

      friend struct application_impl;
      using pickup_ref = reference<pickup>;
      using selector_ref = reference<basic_selector_base>;
      using dial_ref = reference<dial_base>;
      using button_ref = reference<basic_button>;
      using slider_ref = reference<basic_slider_base>;

      view&             _view;

      struct controls
      {
         pickup_ref     pickup;
         selector_ref   type;
         selector_ref   phase;
         dial_ref       frequency;
         dial_ref       resonance;
         button_ref     enable;
         slider_ref     level;
      };

      controls          _controls[3];
   };
}

#endif