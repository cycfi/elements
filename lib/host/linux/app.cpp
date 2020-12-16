/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/app.hpp>
#include <elements/support/font.hpp>
#include <elements/support/resource_paths.hpp>
#include <infra/filesystem.hpp>
#include <string>
#include <functional>
#include <vector>
#include <gtk/gtk.h>

namespace cycfi { namespace elements
{
   // Some app globals
   int argc = 0;
   char** argv = nullptr;
   GtkApplication* the_app = nullptr;
   bool is_activated = false;

   std::vector<std::function<void()>> on_activate;

   GtkApplication* get_app()
   {
      return the_app;
   }

   bool app_is_activated()
   {
      return is_activated;
   }

   void activate(GtkApplication* /* app */, gpointer /* user_data */)
   {
      is_activated = true;
      for (auto f : on_activate)
         f();
      on_activate.clear();
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

   struct init_app
   {
      init_app(std::string id)
      {
         const fs::path resources_path = find_resources();
         font_paths().push_back(resources_path);
         add_search_path(resources_path);

         the_app = gtk_application_new(
            id.c_str()
          , G_APPLICATION_FLAGS_NONE
         );
         g_signal_connect(the_app, "activate", G_CALLBACK(activate), nullptr);
      }
   };

   app::app(
      int         argc_
    , char*       argv_[]
    , std::string name
    , std::string id
   )
   : _app_name(name)
   {
      argc = argc_;
      argv = argv_;
      static init_app init{ id };
      _app = the_app;
   }

   app::~app()
   {
      g_object_unref(_app);
   }

   void app::run()
   {
      g_application_run(G_APPLICATION(_app), argc, argv);
   }

   void app::stop()
   {
      g_application_release(G_APPLICATION(_app));
   }
}}

