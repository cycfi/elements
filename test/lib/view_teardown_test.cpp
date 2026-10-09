/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include "test_support.hpp"
#include <functional>
#include <chrono>
#include <memory>
#include <vector>

#if defined(ELEMENTS_HOST_UI_LIBRARY_COCOA)
# include <AppKit/AppKit.h>   // built as Objective-C++ on macOS
#elif defined(ELEMENTS_HOST_UI_LIBRARY_GTK)
// From GIO, which the GTK host links; the test has no GTK include path.
extern "C"
{
   typedef struct _GApplication GApplication;
   GApplication* g_application_get_default(void);
   void g_application_quit(GApplication* application);
}
#endif

using namespace cycfi::elements;

// A host defers some of a view's work: the first on_open, a poll timer, the
// window's own events. A view destroyed before that work runs must leave
// none of it pointing at the view, which a plugin's editor, closed while its
// host carries on, does all the time. Run under a sanitizer, a callback that
// outlives its view is reported where it happens.
#if !defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
TEST_CASE("A view destroyed before its deferred work runs leaves nothing behind")
{
   app a{"elements view teardown"};

   // Drives the steps from its own window, through the host's event loop.
   window driver_win{"driver", window::standard, {0, 0, 100, 100}};
   view driver{driver_win};

   window win{"teardown", window::standard, {0, 0, 200, 200}};
   int made = 0;
   int const count = 300;

#if defined(ELEMENTS_HOST_UI_LIBRARY_COCOA)
   // A plugin host may keep the NSView alive after the view is gone, as
   // pluginval does; hold each one the same way until the end.
   std::vector<NSView*> held;
#endif

   std::function<void()> step = [&]
   {
      {
         // On the heap, as a plugin's view is, so a callback that reads it
         // after it is gone reads freed memory.
         auto v = std::make_unique<view>(win);
#if defined(ELEMENTS_HOST_UI_LIBRARY_COCOA)
         held.push_back((__bridge NSView*) v->host());
#endif
      }
      // Each step a few milliseconds after the last, so what the host
      // queued for the views already gone runs in between.
      if (++made < count)
      {
         driver.post(std::chrono::milliseconds{5}, step);
      }
      else
      {
#if defined(ELEMENTS_HOST_UI_LIBRARY_COCOA)
         // app::stop on macOS terminates the process. Stop the loop instead,
         // after a last pass that runs what the views queued.
         driver.post(std::chrono::milliseconds{50}, []
         {
            [NSApp stop : nil];
            [NSApp postEvent : [NSEvent otherEventWithType : NSEventTypeApplicationDefined
               location : NSZeroPoint modifierFlags : 0 timestamp : 0
               windowNumber : 0 context : nil subtype : 0 data1 : 0 data2 : 0]
               atStart : NO];
         });
#elif defined(ELEMENTS_HOST_UI_LIBRARY_GTK)
         // app::stop releases the application, which every open window
         // still holds; quit it outright.
         driver.post(std::chrono::milliseconds{50}, []
         {
            g_application_quit(g_application_get_default());
         });
#else
         driver.post(std::chrono::milliseconds{50}, [&a] { a.stop(); });
#endif
      }
   };
   driver.post(step);
   a.run();

#if defined(ELEMENTS_HOST_UI_LIBRARY_COCOA)
   // What the host does next with an NSView it kept: resize it. That calls
   // back into the view through the NSView's pointer to it, which must be
   // cleared, not left pointing at the view that is gone.
   for (NSView* v : held)
      [v setFrameSize : NSMakeSize(50, 50)];
   held.clear();
#endif
   CHECK(made == count);
}
#endif
