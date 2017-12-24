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
      GtkWidget* window = 0;

      // This (cr) is only valid within the view's draw member function
      cairo_t* cr = 0;

      // Mouse button click tracking
      std::uint32_t click_time = 0;
      std::uint32_t click_count = 0;
   };

   config get_config();
   void activate(GtkApplication* app, gpointer user_data);
   void make_main_window(base_view& main_view, GtkWidget* window);
}

#endif