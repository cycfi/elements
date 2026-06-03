/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_X11_HOST_HPP)
#define ELEMENTS_X11_HOST_HPP

#include <X11/Xlib.h>
#include <functional>

namespace cycfi::elements
{
   class base_view;

   // The shared X11 display connection (opened by the first app instance).
   Display* get_display();

   // HiDPI scale from the X resource manager (Xft.dpi / 96), display-wide.
   double display_scale();

   // The WM_DELETE_WINDOW atom (interned once the display is open).
   Atom get_wm_delete_atom();

   // Window → base_view registry, for routing X events to the right view.
   // (Defined in app.cpp; populated by base_view.cpp.)
   void        register_view(::Window w, base_view* view);
   void        unregister_view(::Window w);
   base_view*  find_view(::Window w);

   // Window-close hookup: window.cpp registers a callback fired on WM_DELETE.
   void register_close(::Window w, std::function<void()> on_close);
   void unregister_close(::Window w);
   bool fire_close(::Window w);   // returns true if a handler ran

   // Dispatch a single X event to the matching view (implemented in base_view.cpp).
   void handle_view_event(base_view& view, XEvent& ev);
}

#endif
