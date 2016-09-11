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

   class application : public photon::application
   {
   public:
                        application(view& view_);

      void              pickup_enable(int which, bool enable);
      void              pickup_set_type(int which, pickup::type type_);

   private:

      friend struct application_impl;
      using pickup_ref = reference<pickup>;

      view&             _view;
      pickup_ref        _pickups[3];
   };
}

#endif