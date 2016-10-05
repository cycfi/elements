/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_EXCEPTION_OCTOBER_3_2016)
#define PHOTON_GUI_LIB_EXCEPTION_OCTOBER_3_2016

#include <stdexcept>

namespace photon
{
   struct exception : std::runtime_error
   {
                     exception()
                       : std::runtime_error("")
                     {}
       
      explicit       exception(std::string const& what)
                       : std::runtime_error(what)
                     {}
   };
}

#endif
