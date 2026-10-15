/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   The X11 and Wayland hosts, driven through a real window by the same pump
   their event loops use. A view is told when it opens and when its size
   changes, as on the other hosts. On X11, a view is repainted once per turn
   of the loop, however many refreshes were asked for in that turn: a
   refresh marks an area, and the loop draws what was marked.
=============================================================================*/
#include "test_support.hpp"

#if defined(ELEMENTS_HOST_UI_LIBRARY_X11) || defined(ELEMENTS_HOST_UI_LIBRARY_WAYLAND)

#include <chrono>
#include <thread>
#include <vector>

#if defined(ELEMENTS_HOST_UI_LIBRARY_X11)
# include <X11/Xlib.h>
# include <X11/keysym.h>
namespace cycfi::elements
{
   Display* get_display();
   ::Window get_window(host_window& h);
   void     dispatch_event(XEvent& ev);
   void     poll_views();
}
#endif

using namespace cycfi::elements;
using cycfi::artist::rect;

namespace
{
   // Fills whatever it is given and counts its draws.
   struct counter : element
   {
      view_limits limits(basic_context const&) const override
      {
         return {{0, 0}, {full_extent, full_extent}};
      }

      void draw(context const&) override { ++draws; }

      int draws = 0;
   };

   // A view that records what its host told it.
   struct told_view : view
   {
      using view::view;

      bool key(key_info const& k) override
      {
         keys.push_back(k);
         return true;
      }

      void on_open(extent size_, float scale_) override
      {
         opened = size_;
         open_scale = scale_;
      }

      void on_size_change(extent size_) override
      {
         resized = size_;
      }

      extent                  opened = {0, 0};
      float                   open_scale = 0;
      extent                  resized = {0, 0};
      std::vector<key_info>   keys;
   };

   // One turn of the event loop, as app::run takes it.
   void turn()
   {
#if defined(ELEMENTS_HOST_UI_LIBRARY_X11)
      Display* d = get_display();
      XSync(d, False);
      while (XPending(d))
      {
         XEvent ev;
         XNextEvent(d, &ev);
         dispatch_event(ev);
      }
      poll_views();
#else
      libdecor_dispatch(get_libdecor(), 10);
#endif
   }

   template <typename F>
   void turn_until(F done)
   {
      auto const until = std::chrono::steady_clock::now() + std::chrono::seconds{3};
      while (!done() && std::chrono::steady_clock::now() < until)
      {
         turn();
         std::this_thread::sleep_for(std::chrono::milliseconds(5));
      }
   }
}

TEST_CASE("host: a view is told when it opens and when it is resized", "[host_events]")
{
   window win{"", window::standard, rect{20, 20, 220, 220}};
   told_view view_(win);
   view_.content(std::make_shared<counter>());

   turn_until([&] { return view_.opened.x > 0; });
   CHECK(view_.opened.x == 200);
   CHECK(view_.opened.y == 200);
   CHECK(view_.open_scale > 0);

   win.size({300, 250});
   turn_until([&] { return view_.resized.x > 0; });
   CHECK(view_.resized.x == 300);
   CHECK(view_.resized.y == 250);
}

#if defined(ELEMENTS_HOST_UI_LIBRARY_X11)
TEST_CASE("x11: a key with Shift is the key, with the shift modifier", "[host_events]")
{
   window win{"", window::standard, rect{20, 20, 220, 220}};
   told_view view_(win);
   view_.content(std::make_shared<counter>());
   turn_until([&] { return view_.opened.x > 0; });

   Display* d = get_display();
   ::Window w = get_window(*win.host());
   for (int type : {KeyPress, KeyRelease})
   {
      XEvent ev{};
      ev.xkey.type = type;
      ev.xkey.display = d;
      ev.xkey.window = w;
      ev.xkey.root = DefaultRootWindow(d);
      ev.xkey.state = ShiftMask;
      ev.xkey.keycode = XKeysymToKeycode(d, XK_a);
      ev.xkey.same_screen = True;
      XSendEvent(d, w, False, NoEventMask, &ev);
   }
   turn_until([&] { return view_.keys.size() >= 2; });

   REQUIRE(view_.keys.size() == 2);
   CHECK(view_.keys[0].key == key_code::a);
   CHECK(view_.keys[0].action == key_action::press);
   CHECK((view_.keys[0].modifiers & mod_shift) != 0);
   CHECK(view_.keys[1].key == key_code::a);
   CHECK(view_.keys[1].action == key_action::release);
}

TEST_CASE("x11: refreshes in one turn of the loop draw once", "[x11_repaint]")
{
   window win{"", window::standard, rect{20, 20, 220, 220}};
   view view_(win);
   auto c = std::make_shared<counter>();
   view_.content(hold(c));

   turn_until([&] { return c->draws > 0; });
   REQUIRE(c->draws > 0);

   // Let any repaint the window manager asks for go by.
   for (int i = 0; i != 20; ++i)
   {
      turn();
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
   }

   c->draws = 0;
   for (int i = 0; i != 8; ++i)
      view_.refresh(rect{float(i * 20), 10, float(i * 20 + 10), 20});
   turn();
   CHECK(c->draws == 1);

   // Nothing more was asked for: nothing more is drawn.
   turn();
   CHECK(c->draws == 1);
}
#endif

#endif
