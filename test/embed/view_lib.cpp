/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
// A library that puts an Elements view inside a window someone else owns,
// the way a plugin does in its host. It is built twice, embed_view_a and
// embed_view_b, each with its own copy of Elements and its own class
// prefix, for embed_test to load into one process. Only the three
// functions below are exported.
#include <elements.hpp>
#include <cstdint>

#if defined(__APPLE__)
# import <Cocoa/Cocoa.h>
#endif

#if defined(_WIN32)
# define EMBED_EXPORT extern "C" __declspec(dllexport)
#else
# define EMBED_EXPORT extern "C" __attribute__((visibility("default")))
#endif

#if defined(__linux__)
# include <X11/Xlib.h>
// From Elements' X11 host, compiled into this library.
namespace cycfi::elements
{
   Display* get_display();
   void     dispatch_event(XEvent& ev);
   void     poll_views();
}
#endif

using namespace cycfi::elements;

// The view made inside the parent, a native window of the host's: an HWND,
// an NSView or an X11 Window.
EMBED_EXPORT void* embed_open(void* parent, int width, int height)
{
#if defined(_WIN32)
   auto v = new view(static_cast<host_view_handle>(parent));
#elif defined(__APPLE__)
   auto v = new view(extent{float(width), float(height)});
   NSView* parent_ = (__bridge NSView*) parent;
   NSView* child = (__bridge NSView*) v->host();
   child.frame = parent_.bounds;
   [parent_ addSubview:child];
#else
   auto v = new view(static_cast<unsigned long>(
      reinterpret_cast<std::uintptr_t>(parent)));
#endif
   (void) width;
   (void) height;
   v->content(box(EMBED_COLOR));
   return v;
}

EMBED_EXPORT void embed_close(void* v)
{
   delete static_cast<view*>(v);
}

// Delivers the library's own window events where the window system leaves
// that to it: X11, where each library has its own connection.
EMBED_EXPORT void embed_pump()
{
#if defined(__linux__)
   auto d = get_display();
   while (XPending(d))
   {
      XEvent ev;
      XNextEvent(d, &ev);
      dispatch_event(ev);
   }
   poll_views();
#endif
}
