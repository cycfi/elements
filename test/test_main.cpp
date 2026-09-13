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

#define CATCH_CONFIG_MAIN
#include <infra/catch.hpp>
