/*=============================================================================
   Copyright (c) 2016-2017 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#if !defined(PHOTON_GUI_LIB_HOST_VIEW_IMPL_DECEMBER_24_2017)
#define PHOTON_GUI_LIB_HOST_VIEW_IMPL_DECEMBER_24_2017

#include <photon/host.hpp>
#include <photon/support/load_json.hpp>
#include <gtk/gtk.h>
#include <string>

namespace photon
{
   namespace x3 = boost::spirit::x3;
   namespace fs = boost::filesystem;
   namespace ph = photon;

   struct config
   {
      std::string application_name;
      std::string application_id;
   };
}

BOOST_FUSION_ADAPT_STRUCT(
   photon::config,
   (std::string, application_name)
   (std::string, application_id)
)

namespace photon
{
   struct host_view
   {
      ~host_view();

      cairo_surface_t* surface = nullptr;
      GtkWidget* window = nullptr;

      // Mouse button click tracking
      std::uint32_t click_time = 0;
      std::uint32_t click_count = 0;

      // Scroll acceleration tracking
      std::uint32_t scroll_time = 0;
   };

   config get_config();
   void activate(GtkApplication* app, gpointer user_data);
   void make_main_window(base_view& main_view, GtkWidget* window);
}

#endif