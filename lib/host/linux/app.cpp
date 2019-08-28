/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/app.hpp>
#include <infra/assert.hpp>
#include <json/json_io.hpp>
#include <string>
#include <gtk/gtk.h>

namespace cycfi { namespace elements
{
   namespace fs = boost::filesystem;

   struct config
   {
      std::string application_title;
      std::string application_id;
   };
}}

BOOST_FUSION_ADAPT_STRUCT(
   cycfi::elements::config,
   (std::string, application_title)
   (std::string, application_id)
)

namespace cycfi { namespace elements
{
   config get_config()
   {
      fs::path path = "config.json";
      CYCFI_ASSERT(fs::exists(path), "Error: config.json not exist.");
      auto r = json::load<config>(path);
      CYCFI_ASSERT(r, "Error: Invalid config.json.");
      return r.get();
   }

   config app_config;
   int argc = 0;
   const char** argv = nullptr;

   struct init_app
   {
      init_app()
      {
         app_config = get_config();
      }
   };

   app::app(int argc_, const char* argv_[])
   {
      static init_app init;
      _app_name = app_config.application_title;
      argc = argc_;
      argv = argv_;

      _app = gtk_application_new(
         _app_name.c_str()
       , G_APPLICATION_FLAGS_NONE
      );
   }

   app::~app()
   {
      g_object_unref(_app);
   }

   void app::run()
   {
      int status = g_application_run(
         G_APPLICATION(_app), argc, const_cast<char**>(argv)
      );
   }

   void app::stop()
   {
      g_application_release(G_APPLICATION(_app));
   }
}}

