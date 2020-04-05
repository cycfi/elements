/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/app.hpp>
#include <elements/detail/filesystem.hpp>
#include <infra/assert.hpp>
#include <json/json_io.hpp>
#include <string>
#include <functional>
#include <vector>
#include <gtk/gtk.h>

namespace cycfi { namespace elements
{
   struct config
   {
      std::string application_title;
      std::string application_copyright;
      std::string application_id;
      std::string application_version;
   };
}}

BOOST_FUSION_ADAPT_STRUCT(
   cycfi::elements::config,
   (std::string, application_title)
   (std::string, application_copyright)
   (std::string, application_id)
   (std::string, application_version)
)

namespace cycfi { namespace elements
{
   config get_config()
   {
      fs::path path = "config.json";
      CYCFI_ASSERT(fs::exists(path), "Error: config.json not exist.");
      auto r = json::load<config>(path);
      CYCFI_ASSERT(r, "Error: Invalid config.json.");
      return *r;
   }

   // Some app globals
   config app_config;
   int argc = 0;
   const char** argv = nullptr;
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
         app_config = get_config();
         the_app = gtk_application_new(
            app_config.application_id.c_str()
          , G_APPLICATION_FLAGS_NONE
         );
         g_signal_connect(the_app, "activate", G_CALLBACK(activate), nullptr);
      }
   };

   app::app(int argc_, const char* argv_[])
   {
      static init_app init;
      _app_name = app_config.application_title;
      argc = argc_;
      argv = argv_;
      _app = the_app;
   }

   app::~app()
   {
      g_object_unref(_app);
   }

   void app::run()
   {
      g_application_run(
         G_APPLICATION(_app), argc, const_cast<char**>(argv)
      );
   }

   void app::stop()
   {
      g_application_release(G_APPLICATION(_app));
   }
}}

