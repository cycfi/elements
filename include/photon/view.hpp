/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_VIEW_AUGUST_15_2016)
#define PHOTON_GUI_LIB_VIEW_AUGUST_15_2016

#include <photon/support/rect.hpp>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // The View. There's only one of this per window.
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class view
   {
   public:

      void              draw(rect area);
   };
}

#endif