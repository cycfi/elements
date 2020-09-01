/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#if !defined(ELEMENTS_APP_MARCH_6_2019)
#define ELEMENTS_APP_MARCH_6_2019

#include <string>
#include <infra/support.hpp>

#if defined(ELEMENTS_HOST_UI_LIBRARY_GTK)
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
                           app(
                              int argc
                            , char* argv[]
                            , std::string name
                            , std::string id);
                           ~app();

      std::string const&   name() const { return _app_name; }
      void                 run();
      void                 stop();

   private:

#if defined(ELEMENTS_HOST_UI_LIBRARY_COCOA)
      void* _menubar;
#elif defined(ELEMENTS_HOST_UI_LIBRARY_GTK)
      GtkApplication* _app;
#elif defined(ELEMENTS_HOST_UI_LIBRARY_WIN32)
      bool  _running = true;
#endif

      std::string          _app_name;
   };
}}

#endif

