/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#if !defined(CYCFI_ELEMENTS_GUI_LIB_APP_MARCH_6_2019)
#define CYCFI_ELEMENTS_GUI_LIB_APP_MARCH_6_2019

#include <string>
#include <infra/support.hpp>

#if defined(__linux__)
typedef struct _GtkApplication GtkApplication;
#endif

namespace cycfi { namespace elements
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
#elif defined(__linux__)
      GtkApplication* _app;
#endif

      std::string          _app_name;
   };
}}

#endif

