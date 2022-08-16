/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/app.hpp>
#include <elements/support/font.hpp>
#include <elements/support/resource_paths.hpp>
#include <elements/base_view.hpp>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <cairo/cairo.h>

namespace cycfi { namespace elements
{
   // Some app globals
   char** argv = nullptr;

   std::unordered_map<Window, base_view*> gs_windows_map;

   // The singleton connection to the X11 server - all API calls are relative to this connection
   Display* the_display = XOpenDisplay(nullptr);
   Display* get_display()
   {
      return the_display;
   }

   fs::path find_resources()
   {
      const fs::path app_path = fs::path(argv[0]);
      const fs::path app_dir = app_path.parent_path();

      if (app_dir.filename() == "bin")
      {
         fs::path path = app_dir.parent_path() / "share" / app_path.filename() / "resources";
         if (fs::is_directory(path))
            return path;
      }

       const fs::path app_resources_dir = app_dir / "resources";
       if (fs::is_directory(app_resources_dir))
          return app_resources_dir;

       return fs::current_path() / "resources";
   }

   app::app(
     int       /*argc_*/
   , char*       argv_[]
   , std::string name
   , std::string /*id*/
   )
   : _app_name(name)
   {
      argv = argv_;
      const fs::path resources_path = find_resources();
      font_paths().push_back(resources_path);
      add_search_path(resources_path);
   }

   app::~app()
   {
      //XCloseDisplay(the_display);
   }

   void app::run()
   {
      do {} while(tick());
   }

   bool on_event(base_view *, const XEvent&);

   bool app::tick()
   {
      XEvent event;
      bool result = true;

      while(XPending(the_display) > 0) {
         XNextEvent(the_display, &event);

         base_view* view = gs_windows_map[event.xany.window];
         if(view) {
            result &= on_event(view, event);
            view->poll();
         }
      }

      // Poll everyone at least once
      for(auto &[window, view]: gs_windows_map) {
         if(view) {
            view->poll();
         }
      }

      return result;
   }

   void app::stop()
   {
   }
}}
