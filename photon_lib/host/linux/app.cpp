/*=============================================================================
   Copyright (c) 2016-2017 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <photon/host.hpp>
#include <gtk/gtk.h>

namespace
{
   void activate(GtkApplication* app, gpointer user_data)
   {
      GtkWidget* window = gtk_application_window_new(app);
      gtk_window_set_title(GTK_WINDOW(window), "Hello GNOME");
      gtk_widget_show_all(window);
   }
}

///////////////////////////////////////////////////////////////////////////////
// Application event loop entry
namespace photon
{
   int app_main(int argc, const char* argv[])
   {
      GtkApplication* app = gtk_application_new(
         "org.gtk.example", G_APPLICATION_FLAGS_NONE
      );

      g_signal_connect(app, "activate", G_CALLBACK(activate), nullptr);
      int status = g_application_run(
         G_APPLICATION(app), argc, const_cast<char**>(argv)
      );
      g_object_unref(app);
      return status;
   }
}

