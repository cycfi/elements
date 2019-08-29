/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements/window.hpp>
#include <elements/support.hpp>
#include <gtk/gtk.h>

namespace elements = cycfi::elements;

namespace cycfi { namespace elements
{
   struct _host_window
   {
      GtkWidget* host;
   };

   extern std::vector<std::function<void()>> on_activate;
   GtkApplication* get_app();
   bool app_is_activated();

   GtkWidget* get_window(_host_window& h)
   {
      return h.host;
   }

   window::window(std::string const& name, int style_, rect const& bounds)
   {
      // Chicken and egg. GTK wants us to create windows only
      // after the app is activated. So we have a scheme to
      // defer actions. If the app is not activated make_window is
      // added to the on_activate vector, otherwise, it is called
      // immediately.

      auto make_window =
         [this, name, style_, bounds]()
         {
            GtkWidget* win = gtk_application_window_new(get_app());
            gtk_window_set_title(GTK_WINDOW(win), name.c_str());

            _window = new _host_window;
            _window->host = win;

            position(bounds.top_left());
            size(bounds.bottom_right());
            gtk_widget_show_all(win);
         };

      if (app_is_activated())
         make_window();
      else
         on_activate.push_back(make_window);
   }

   window::~window()
   {
      gtk_widget_destroy(_window->host);
      delete _window;
   }

   point window::size() const
   {
      gint width, height;
      gtk_window_get_size(GTK_WINDOW(_window->host), &width, &height);
      return { float(width), float(height) };
   }

   void window::size(point const& p)
   {
      gtk_window_resize(GTK_WINDOW(_window->host), p.x, p.y);
   }

   void window::limits(view_limits limits_)
   {
   }

   point window::position() const
   {
      return {};
   }

   void window::position(point const& p)
   {
      gtk_window_move(GTK_WINDOW(_window->host), p.x, p.y);
   }
}}

