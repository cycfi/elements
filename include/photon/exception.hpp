/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_EXCEPTION_MAY_8_2016)
#define PHOTON_GUI_LIB_EXCEPTION_MAY_8_2016

#include <stdexcept>

namespace photon
{
   struct glfw_exception : std::runtime_error
   {
      glfw_exception(int error, char const* msg)
       : std::runtime_error(msg)
       , error(error)
      {};

      int error;
   };

   struct nanovg_exception : std::runtime_error
   {
      nanovg_exception(char const* msg)
       : std::runtime_error(msg)
      {};
   };
}

#endif  // CIERE_JSON_EXCEPTION_HPP
