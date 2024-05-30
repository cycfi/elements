/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/app.hpp>
#include <infra/filesystem.hpp>
#include <string>
#include <functional>
#include <vector>
#include <gtk/gtk.h>

namespace cycfi::elements
{
   // Some app globals
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

   struct init_app
   {
      init_app()
      {
         the_app = gtk_application_new(nullptr, G_APPLICATION_FLAGS_NONE);
         g_signal_connect(the_app, "activate", G_CALLBACK(activate), nullptr);
      }
   };

   app::app(std::string name)
    : _app_name(name)
   {
      static init_app init{};
      _app = the_app;
   }

   app::~app()
   {
      g_object_unref(_app);
   }

   void app::run()
   {
      // g_application_run is intended to be run from main() and its return value is intended to
      // be returned by main(). It is possible to pass nullptr if argv since commandline handling
      // is not required. Also, on Windows, argc and argv are ignored.
      //
      // Given this, we will not pass argc and argv anymore. If you need to process these, you can
      // do so somewhere in main.

      g_application_run(G_APPLICATION(_app), 0, nullptr);
   }

   void app::stop()
   {
      g_application_release(G_APPLICATION(_app));
   }
}

