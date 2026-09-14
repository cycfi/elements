/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Shared support for the Elements test suite. Built on the headless host
   (ELEMENTS_HOST_UI_LIBRARY=headless), a test's view has no window at all:
   it lays out, draws and takes events from direct calls, and anything can
   run it, with no display. Drawing goes to an offscreen image; with the
   Artist recording backend that image keeps a journal of what was drawn.
   On a windowing host the view is made on a real, unshown window instead.

   Exactly one translation unit (test_main.cpp) defines the Catch main.
=============================================================================*/
#if !defined(ELEMENTS_TEST_SUPPORT_SEPTEMBER_13_2026)
#define ELEMENTS_TEST_SUPPORT_SEPTEMBER_13_2026

#if defined(_WIN32)
# ifndef UNICODE
#  define UNICODE
# endif
#endif

#include <infra/catch.hpp>
#include <elements.hpp>

#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
# include <elements/headless.hpp>
#endif

namespace cycfi::elements::test
{
   using artist::canvas;
   using artist::extent;
   using artist::offscreen_image;
   using artist::rect;

   // An element that records what it was asked to do, so a test can assert on
   // the traversal itself rather than on pixels: how often it drew, and with
   // what bounds.
   struct probe_element : element
   {
      explicit probe_element(extent size = {20, 20}) : _size(size) {}

      view_limits limits(basic_context const&) const override
      {
         return {{_size.x, _size.y}, {_size.x, _size.y}};
      }

      void draw(context const& ctx) override
      {
         ++draws;
         last_bounds = ctx.bounds;
      }

      bool wants_control() const override   { return true; }

      extent   _size;
      int      draws = 0;
      rect     last_bounds = {};
   };

   // A view plus an offscreen canvas of the same size: the pair a drawing
   // test needs.
   struct test_view
   {
      explicit test_view(extent size = {400, 300})
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
       : view_{size}
#else
       // On a windowing host, go through window + view the way an
       // application does: view{extent} asks for a parentless child window,
       // which Windows will not create.
       : win{"elements test", window::standard, {0, 0, size.x, size.y}}
       , view_{win}
#endif
       , img{size.x, size.y, 1}
       , offscr{img}
       , cnv{offscr.context()}
      {}

      // Draw the content once, optionally through a clip, the way a host
      // repainting only an invalidated area does.
      void draw(rect clip = {})
      {
         if (clip.width() > 0 && clip.height() > 0)
         {
            cnv.add_rect(clip);
            cnv.clip();
         }
         view_.draw(cnv);
      }

#if !defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
      window               win;
#endif
      view                 view_;
      cycfi::artist::image img;
      offscreen_image      offscr;
      canvas               cnv;
   };
}

#endif
