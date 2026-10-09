/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
// UNICODE must be defined before including Catch: on MSVC the Unicode charset
// defines _UNICODE, which makes Catch emit a wmain entry point that calls its
// wchar_t applyCommandLine overload, and that overload is only declared when
// UNICODE is defined.
#if defined(_WIN32)
# ifndef UNICODE
#  define UNICODE
# endif
#endif

#define CATCH_CONFIG_RUNNER
#include <infra/catch.hpp>
#include <cstdlib>
#include <string>

#if defined(ELEMENTS_HOST_UI_LIBRARY_GTK)

// A GTK window is made only once the app is activated, and the tests run no
// app loop. Activate it once, up front, so the tests' windows and views are
// made at once, as in a running app.
#include <elements/app.hpp>

// From GIO, which the GTK host links; the tests have no GTK include path.
extern "C"
{
   typedef struct _GApplication GApplication;
   typedef struct _GCancellable GCancellable;
   typedef struct _GError GError;
   GApplication* g_application_get_default(void);
   int g_application_register(
      GApplication* application, GCancellable* cancellable, GError** error);
   void g_application_activate(GApplication* application);
}

#endif

namespace
{
   // The suite's log goes next to its other results, not to the user's log
   // directory, with view at debug so the diagnostics page test can read a
   // line back. log_init reads the variables once, on the first message.
   void set_log_env()
   {
      std::string const dir = std::string{RESULTS_PATH} + "logs";
#if defined(_WIN32)
      _putenv_s("CYCFI_LOG_DIR", dir.c_str());
      _putenv_s("CYCFI_LOG", "view=debug,input=off");
#else
      setenv("CYCFI_LOG_DIR", dir.c_str(), 1);
      setenv("CYCFI_LOG", "view=debug,input=off", 1);
#endif
   }
}

int main(int argc, char* argv[])
{
   set_log_env();
#if defined(ELEMENTS_HOST_UI_LIBRARY_GTK)
   cycfi::elements::app a{"elements test"};
   auto* g = g_application_get_default();
   g_application_register(g, nullptr, nullptr);
   g_application_activate(g);
#endif
   return Catch::Session().run(argc, argv);
}
