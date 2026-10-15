/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/application/window.adoc.
=============================================================================*/
#include "test_support.hpp"
#include <elements/app.hpp>
#include <elements/window.hpp>
#include <elements/view.hpp>
#include <chrono>
#include <thread>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   // GTK and Wayland apply a size from their event loop, which the tests
   // do not run: one turn of it, as test_view takes.
   void pump()
   {
#if defined(ELEMENTS_HOST_UI_LIBRARY_GTK)
      while (g_main_context_iteration(nullptr, 0))
         ;
#elif defined(ELEMENTS_HOST_UI_LIBRARY_WAYLAND)
      libdecor_dispatch(get_libdecor(), 10);
#endif
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
   }

   // A host applies a size or position when it gets to it: wait for it,
   // within a bound.
   template <typename F>
   void settled(F done)
   {
      auto const until = std::chrono::steady_clock::now() + std::chrono::seconds{3};
      while (!done() && std::chrono::steady_clock::now() < until)
         pump();
   }

   bool size_is(window const& w, point p)
   {
      settled([&] { return w.size() == p; });
      return w.size() == p;
   }

   bool position_is(window const& w, point p)
   {
      settled([&] { return w.position() == p; });
      return w.position() == p;
   }

   // The view fills its window, once the host has sized it
   bool fills(view const& v, window const& w)
   {
      settled([&] { return v.size() == w.size(); });
      return v.size() == w.size();
   }
}

TEST_CASE("window page: the size is the content's", "[window_page]")
{
   // A title bar, but no title: a window with a title remembers its
   // frame by its title on macOS, which a test must not leave behind.
   window win{"", window::standard, rect{20, 20, 340, 260}};
   CHECK(size_is(win, {320, 240}));               // the bounds' size

   view view_{win};

   // A decorated GTK window under a window manager sizes its view late,
   // or not at all, and ignores a new size once the view has set its
   // limits: see the TODO in the KB.
#if !defined(ELEMENTS_HOST_UI_LIBRARY_GTK)
   CHECK(fills(view_, win));                      // the view fills it
   win.size({400, 300});
   CHECK(size_is(win, {400, 300}));
#endif
}

#if !defined(ELEMENTS_HOST_UI_LIBRARY_GTK)
TEST_CASE("window page: the size stays within the limits", "[window_page]")
{
   window win{"", window::standard, rect{20, 20, 340, 260}};
   win.limits({{200, 150}, {500, 400}});
   win.size({600, 600});
   CHECK(size_is(win, {500, 400}));
   win.size({10, 10});
   CHECK(size_is(win, {200, 150}));
}
#endif

// Wayland has no window positions: see the page's Host Notes.
#if !defined(ELEMENTS_HOST_UI_LIBRARY_WAYLAND)
TEST_CASE("window page: the position is the top left, on the screen", "[window_page]")
{
   window win{"", window::standard, rect{20, 20, 340, 260}};
   win.position({100, 120});
   CHECK(position_is(win, {100, 120}));
   CHECK(size_is(win, {320, 240}));               // moving keeps the size
}
#endif

TEST_CASE("window page: the styles", "[window_page]")
{
   CHECK(window::bare == 0);
   CHECK(window::standard == (window::with_title | window::closable
      | window::miniaturizable | window::resizable));
}

TEST_CASE("window page: example", "[window_page]")
{
   app _app("Settings");

   window _win(
      _app.name()
    , window::with_title | window::closable
    , rect{100, 100, 500, 400}
   );
   _win.on_close = [&_app]() { _app.stop(); };

#if !defined(ELEMENTS_HOST_UI_LIBRARY_COCOA)
   // On macOS, a window with a title takes a frame saved under it
   CHECK(size_is(_win, {400, 300}));
#endif
   CHECK(_win.on_close);
}
