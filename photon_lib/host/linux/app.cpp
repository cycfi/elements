/*=============================================================================
   Copyright (c) 2016-2017 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <photon/host.hpp>
#include <photon/support/load_json.hpp>
#include <photon/support/assert.hpp>
#include <gtk/gtk.h>
#include <string>

namespace x3 = boost::spirit::x3;
namespace json = photon::json;
namespace fs = boost::filesystem;

struct config
{
   std::string application_name;
   std::string application_id;
};

BOOST_FUSION_ADAPT_STRUCT(
   config,
   (std::string, application_name)
   (std::string, application_id)
)

struct activate_info
{
   config& cfg;
};

config get_config()
{
   fs::path path = "config.json";
   PHOTON_ASSERT(boost::filesystem::exists(path), "Error: config.json not exist.");
   auto r = json::load<config>(path);
   PHOTON_ASSERT(r, "Error: Invalid config.json.");
   return r.get();
}

void activate(GtkApplication* app, gpointer user_data)
{
   auto pinfo = static_cast<activate_info const*>(user_data);
   GtkWidget* window = gtk_application_window_new(app);
   gtk_window_set_title(GTK_WINDOW(window), pinfo->cfg.application_name.c_str());
   gtk_widget_show_all(window);
}

///////////////////////////////////////////////////////////////////////////////
// Application event loop entry
namespace photon
{
   int app_main(int argc, const char* argv[])
   {
      config cfg = get_config();
      activate_info info = { cfg };

      GtkApplication* app = gtk_application_new(
         cfg.application_id.c_str(), G_APPLICATION_FLAGS_NONE
      );

      g_signal_connect(app, "activate", G_CALLBACK(activate), &info);
      int status = g_application_run(
         G_APPLICATION(app), argc, const_cast<char**>(argv)
      );
      g_object_unref(app);
      return status;
   }
}

