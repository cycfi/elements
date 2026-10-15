/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/application/app.adoc.
=============================================================================*/
#include "test_support.hpp"
#include <elements/app.hpp>
#include <elements/window.hpp>
#include <elements/view.hpp>
#include <elements/element.hpp>
#include <elements/element/style.hpp>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

TEST_CASE("app page: its name is the one given", "[app_page]")
{
   app a{"Elements Test"};
   CHECK(a.name() == "Elements Test");
}

#if !defined(ELEMENTS_HOST_UI_LIBRARY_GTK)
TEST_CASE("app page: run returns once stop is called", "[app_page]")
{
   // On a host with an event loop, a stop posted from the loop ends it,
   // and run returns. The headless host has no loop: run returns at once.
   app a{"Elements Test"};
   window win{"", window::bare, rect{0, 0, 100, 100}};
   view view_{win};
   bool stopped = false;
   view_.post([&] { stopped = true; a.stop(); });
   a.run();
#if !defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   CHECK(stopped);
#endif
}
#endif

#if !defined(ELEMENTS_HOST_UI_LIBRARY_GTK)
TEST_CASE("app page: example", "[app_page]")
{
   app _app("Hello Universe");
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);
   view_.content(
      align_center_middle(label("Hello, Universe")),
      box(rgba(35, 35, 37, 255))
   );

   view_.post([&_app] { _app.stop(); });      // in place of closing the window
   _app.run();
   CHECK(_app.name() == "Hello Universe");
}
#endif
