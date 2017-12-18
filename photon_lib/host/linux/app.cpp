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
namespace ph = photon;

namespace photon
{
   struct host_view
   {
      GtkWidget* window;
      std::unique_ptr<photon::base_view> view;
   };
}

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
   config const& cfg;
   photon::host_view& main_view;
};

config get_config()
{
   fs::path path = "config.json";
   PHOTON_ASSERT(fs::exists(path), "Error: config.json not exist.");
   auto r = json::load<config>(path);
   PHOTON_ASSERT(r, "Error: Invalid config.json.");
   return r.get();
}

void quit(GtkApplication* app, gpointer user_data)
{
   auto& main_view = *reinterpret_cast<photon::host_view*>(user_data);
   main_view.view.reset(nullptr);
}

void activate(GtkApplication* app, gpointer user_data)
{
   auto pinfo = static_cast<activate_info*>(user_data);
   GtkWidget* window = gtk_application_window_new(app);
   gtk_window_set_title(GTK_WINDOW(window), pinfo->cfg.application_name.c_str());
   gtk_widget_show_all(window);
   pinfo->main_view.window = window;
   pinfo->main_view.view = std::move(ph::new_view(&pinfo->main_view));
   g_signal_connect(G_OBJECT(window), "destroy",
      G_CALLBACK(quit), &pinfo->main_view);
}

///////////////////////////////////////////////////////////////////////////////
// Application event loop entry
namespace photon
{
   int app_main(int argc, const char* argv[])
   {
      host_view main_view;
      config cfg = get_config();
      activate_info info = { cfg, main_view };

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

