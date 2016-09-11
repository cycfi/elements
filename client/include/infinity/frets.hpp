/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(INFINITY_FRETS_MAY_14_2016)
#define INFINITY_FRETS_MAY_14_2016

#include <photon/widget.hpp>

namespace infinity
{
   using photon::widget;
   using photon::basic_context;
   using photon::context;
   using photon::rect;
   using photon::widget_limits;

   class frets : public widget
   {
   public:

      virtual widget_limits   limits(basic_context const& ctx) const;
      virtual void            draw(context const& ctx);
   };
}

#endif