/*=======================================================================================
   Copyright (c) 2016 Joel de Guzman

   Distributed under the The MIT License (https://opensource.org/licenses/MIT)
=======================================================================================*/
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
