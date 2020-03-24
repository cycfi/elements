/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#if !defined(ELEMENTS_APP_MARCH_6_2019)
#define ELEMENTS_APP_MARCH_6_2019

#include <string>
#include <infra/support.hpp>

#if defined(_HOST_UI_LIBRARY_GTK_)
using GtkApplication = struct _GtkApplication;
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

#if defined(_HOST_UI_LIBRARY_COCOA_)
      void* _menubar;
#elif defined(_HOST_UI_LIBRARY_GTK_)
      GtkApplication* _app;
#elif defined(_HOST_UI_LIBRARY_WIN32_)
      bool  _running = true;
#endif

      std::string          _app_name;
   };
}}

#endif

