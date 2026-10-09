/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
// Two libraries, each with its own copy of Elements, embedding views in one
// process, as two plugins do in a host. Anything one copy keeps for the
// whole process, a window class, a factory, a request left in a buffer,
// can collide with the other's or outlive its library. Both views are
// opened at once, closed, both libraries unloaded, then one loaded and
// opened again, as when a project is closed and reopened.
//
// A failure can be a crash, at unload or at exit, or a hang on a message
// box, so the caller runs this with a timeout. It exits 0 on success.
//
//    embed_test <path to embed_view_a> <path to embed_view_b>
#include "host_window.hpp"
#include <cstdio>
#include <string>

#if defined(_WIN32)
# include <windows.h>
#else
# include <dlfcn.h>
#endif

using namespace embed_test;

namespace
{
   struct library
   {
      using open_fn = void* (*)(void*, int, int);
      using close_fn = void (*)(void*);
      using pump_fn = void (*)();

      bool load(char const* path)
      {
#if defined(_WIN32)
         _lib = LoadLibraryA(path);
#else
         _lib = dlopen(path, RTLD_NOW | RTLD_LOCAL);
#endif
         if (!_lib)
            return false;
         open = reinterpret_cast<open_fn>(symbol("embed_open"));
         close = reinterpret_cast<close_fn>(symbol("embed_close"));
         pump = reinterpret_cast<pump_fn>(symbol("embed_pump"));
         return open && close && pump;
      }

      void unload()
      {
#if defined(_WIN32)
         FreeLibrary(static_cast<HMODULE>(_lib));
#else
         dlclose(_lib);
#endif
         _lib = nullptr;
      }

      void* symbol(char const* name)
      {
#if defined(_WIN32)
         return reinterpret_cast<void*>(
            GetProcAddress(static_cast<HMODULE>(_lib), name));
#else
         return dlsym(_lib, name);
#endif
      }

      void*       _lib = nullptr;
      open_fn     open = nullptr;
      close_fn    close = nullptr;
      pump_fn     pump = nullptr;
   };

   struct embedded
   {
      void*    view = nullptr;
      window   win;
   };

   embedded open(library& lib)
   {
      embedded e;
      e.win = open_window(400, 300);
      e.view = lib.open(e.win.handle, 400, 300);
      return e;
   }

   void close(library& lib, embedded& e)
   {
      lib.close(e.view);
      close_window(e.win);
   }

   void run(std::initializer_list<library*> libs, int ms)
   {
      for (int t = 0; t < ms; t += 16)
      {
         run_events(16);
         for (auto lib : libs)
            lib->pump();
      }
   }

   int fail(char const* what)
   {
      std::fprintf(stderr, "embed_test: %s\n", what);
      return 1;
   }
}

int main(int argc, char const* argv[])
{
   if (argc != 3)
      return fail("usage: embed_test <embed_view_a> <embed_view_b>");

   library a, b;
   if (!a.load(argv[1]))
      return fail("cannot load the first library");
   if (!b.load(argv[2]))
      return fail("cannot load the second library");

   auto va = open(a);
   auto vb = open(b);
   if (!va.view || !vb.view)
      return fail("a view did not open");
   run({&a, &b}, 500);

   close(b, vb);
   close(a, va);
   run({&a, &b}, 100);
   b.unload();
   a.unload();

   // Loaded again after it was unloaded: whatever the first load left
   // registered must not be in the way.
   if (!a.load(argv[1]))
      return fail("cannot load the first library again");
   auto again = open(a);
   if (!again.view)
      return fail("the view did not open again");
   run({&a}, 300);
   close(a, again);
   run({&a}, 100);
   a.unload();

   std::puts("embed_test: passed");
   return 0;
}
