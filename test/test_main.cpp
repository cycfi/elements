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

#if defined(ELEMENTS_HOST_UI_LIBRARY_GTK)

// A GTK window is made only once the app is activated, and the tests run no
// app loop. Activate it once, up front, so the tests' windows and views are
// made at once, as in a running app.
#define CATCH_CONFIG_RUNNER
#include <infra/catch.hpp>
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

int main(int argc, char* argv[])
{
   cycfi::elements::app a{"elements test"};
   auto* g = g_application_get_default();
   g_application_register(g, nullptr, nullptr);
   g_application_activate(g);
   return Catch::Session().run(argc, argv);
}

#else

#define CATCH_CONFIG_MAIN
#include <infra/catch.hpp>

#endif
