/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/app.hpp>
#include <elements/base_view.hpp>
#include "x11_host.hpp"

#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <map>
#include <stdexcept>
#include <cstdlib>

namespace cycfi::elements
{
   namespace
   {
      Display*                          the_display = nullptr;
      Atom                              the_wm_delete = 0;
      std::map<::Window, base_view*>    the_views;
      std::map<::Window, std::function<void()>> the_closers;

      struct init_display
      {
         init_display()
         {
            the_display = XOpenDisplay(nullptr);
            if (!the_display)
               throw std::runtime_error("Failed to open X display");
            the_wm_delete = XInternAtom(the_display, "WM_DELETE_WINDOW", False);
         }
      };
   }

   Display* get_display()        { return the_display; }
   Atom     get_wm_delete_atom() { return the_wm_delete; }

   double display_scale()
   {
      double scale = 1.0;
      if (char* rms = XResourceManagerString(the_display))
      {
         if (XrmDatabase db = XrmGetStringDatabase(rms))
         {
            XrmValue value;
            char* type = nullptr;
            if (XrmGetResource(db, "Xft.dpi", "Xft.Dpi", &type, &value) && value.addr)
               if (double dpi = std::atof(value.addr); dpi > 0)
                  scale = dpi / 96.0;
            XrmDestroyDatabase(db);
         }
      }
      return scale;
   }

   void register_view(::Window w, base_view* view) { the_views[w] = view; }
   void unregister_view(::Window w)                { the_views.erase(w); }
   base_view* find_view(::Window w)
   {
      auto i = the_views.find(w);
      return i == the_views.end() ? nullptr : i->second;
   }

   void register_close(::Window w, std::function<void()> on_close)
   {
      the_closers[w] = std::move(on_close);
   }
   void unregister_close(::Window w) { the_closers.erase(w); }
   bool fire_close(::Window w)
   {
      auto i = the_closers.find(w);
      if (i == the_closers.end())
         return false;
      if (i->second)
         i->second();
      return true;
   }

   app::app(std::string name)
    : _app_name(std::move(name))
   {
      static init_display init{};
   }

   app::~app()
   {
   }

   void app::run()
   {
      _running = true;
      XEvent ev;
      while (_running)
      {
         XNextEvent(the_display, &ev);

         // Window-close protocol takes precedence over view dispatch.
         if (ev.type == ClientMessage &&
             Atom(ev.xclient.data.l[0]) == the_wm_delete)
         {
            fire_close(ev.xclient.window);
            continue;
         }

         if (base_view* view = find_view(ev.xany.window))
            handle_view_event(*view, ev);
      }
   }

   void app::stop()
   {
      _running = false;
   }
}
