/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include "view_impl.hpp"
#include <photon/support/assert.hpp>

namespace cycfi { namespace photon
{
   config get_config()
   {
      fs::path path = "config.json";
      CYCFI_ASSERT(fs::exists(path), "Error: config.json not exist.");
      auto r = json::load<config>(path);
      CYCFI_ASSERT(r, "Error: Invalid config.json.");
      return r.get();
   }

   struct activate_info
   {
      config const& cfg;
      std::unique_ptr<base_view>& main_view;
      host_view& host_view_;
   };

   void activate(GtkApplication* app, gpointer user_data)
   {
      auto pinfo = static_cast<activate_info*>(user_data);
      GtkWidget* window = gtk_application_window_new(app);
      gtk_window_set_title(
         GTK_WINDOW(window), pinfo->cfg.application_name.c_str());
      pinfo->host_view_.window = window;
      pinfo->main_view = std::move(ph::new_view(&pinfo->host_view_));
      make_main_window(*pinfo->main_view, window);
      gtk_widget_show_all(window);
   }

///////////////////////////////////////////////////////////////////////////////
// Application event loop entry

   int app_main(int argc, const char* argv[])
   {
      config cfg = get_config();
      std::unique_ptr<base_view> main_view;
      host_view host_view_;
      activate_info info = { cfg, main_view, host_view_ };

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
}}

