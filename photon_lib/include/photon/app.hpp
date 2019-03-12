/*=============================================================================
   Copyright (c) 2016-2018 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#if !defined(CYCFI_PHOTON_GUI_LIB_APP_MARCH_6_2019)
#define CYCFI_PHOTON_GUI_LIB_APP_MARCH_6_2019

#include <string>
#include <infra/support.hpp>

namespace cycfi { namespace photon
{
   ////////////////////////////////////////////////////////////////////////////
   // Application class
   ////////////////////////////////////////////////////////////////////////////
   class app : non_copyable
   {
   public:
                           app(int argc, const char* argv[]);
                           ~app();

      std::string const&   name() const { return _app_name; }
      void                 run();
      void                 stop();

   private:

#if defined(__APPLE__)
      void* _menubar;
#elif defined(_WIN32)
      bool  _running = true;
#endif

      std::string          _app_name;
   };
}}

#endif

