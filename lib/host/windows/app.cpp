/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements/app.hpp>

// int wmain(int, wchar_t**)
// {
//    return 0;
// }

// #undef UNICODE
// #define UNICODE
// #include <windows.h>
// #include <shellapi.h>

namespace cycfi { namespace elements
{
   app::app(int argc, const char* argv[])
   {
   }

   app::~app()
   {
   }

   void app::run()
   {
      // struct Args
      // {
      //    int n;
      //    wchar_t** p;

      //    ~Args() {  if (p != 0) ::LocalFree( p ); }
      //    Args(): p(::CommandLineToArgvW( ::GetCommandLine(), &n)) {}
      // };

      // Args args;

      // if (!args.p == 0)
      //    wmain(args.n, args.p);
   }

   void app::stop()
   {
   }
}}

