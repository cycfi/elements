/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements/window.hpp>
#include <elements/support.hpp>
#include <functional>
#include <vector>
#include <gtk/gtk.h>

namespace elements = cycfi::elements;

namespace cycfi { namespace elements
{
   struct host_window
   {
      GtkWidget* host = nullptr;
      std::vector<std::function<void()>> on_activate;
   };

   extern std::vector<std::function<void()>> on_activate;
   GtkApplication* get_app();
   bool app_is_activated();

   GtkWidget* get_window(host_window& h)
   {
      return h.host;
   }

   void on_window_activate(host_window& h, std::function<void()> f)
   {
      h.on_activate.push_back(f);
   }

   float get_scale(GtkWidget* widget)
   {
      auto gdk_win = gtk_widget_get_window(widget);
      return 1.0f / gdk_window_get_scale_factor(gdk_win);
   }

   window::window(std::string const& name, int style_, rect const& bounds)
    :  _window(new host_window)
   {
      cycfi::ignore(style_);

      // Chicken and egg. GTK wants us to create windows only
      // after the app is activated. So we have a scheme to
      // defer actions. If the app is not activated make_window is
      // added to the on_activate vector, otherwise, it is called
      // immediately.

      auto make_window =
         [this, name, bounds]()
         {
            GtkWidget* win = gtk_application_window_new(get_app());
            g_object_ref(win);
            gtk_window_set_title(GTK_WINDOW(win), name.c_str());
            _window->host = win;

            for (auto f : _window->on_activate)
               f();

            gtk_widget_show_all(win);

            position(bounds.top_left());
            size(bounds.bottom_right());
         };

      if (app_is_activated())
         make_window();
      else
         on_activate.push_back(make_window);
   }

   window::~window()
   {
      g_object_unref(_window->host);
      delete _window;
   }

   point window::size() const
   {
      auto win = GTK_WINDOW(_window->host);
      auto scale = get_scale(_window->host);
      gint width, height;
      gtk_window_get_size(win, &width, &height);
      return { float(width) / scale, float(height) / scale };
   }

   void window::size(point const& p)
   {
      auto win = GTK_WINDOW(_window->host);
      auto scale = get_scale(_window->host);
      gtk_window_resize(win, p.x * scale, p.y * scale);
   }

   void window::limits(view_limits limits_)
   {
      auto set_limits =
         [this, limits_]()
         {
            constexpr float max = 10E6;
            auto win = GTK_WINDOW(_window->host);
            GdkGeometry hints;
            hints.min_width = limits_.min.x;
            hints.max_width = std::min<float>(limits_.max.x, max);
            hints.min_height = limits_.min.y;
            hints.max_height = std::min<float>(limits_.max.y, max);

            gtk_window_set_geometry_hints(
               win,
               _window->host,
               &hints,
               GdkWindowHints(GDK_HINT_MIN_SIZE | GDK_HINT_MAX_SIZE)
            );
         };

      if (app_is_activated())
         set_limits();
      else
         on_activate.push_back(set_limits);
   }

   point window::position() const
   {
      return {}; // $$$ TODO $$$
   }

   void window::position(point const& p)
   {
      auto set_position =
         [this, p]()
         {
            auto win = GTK_WINDOW(_window->host);
            auto scale = get_scale(_window->host);
            gtk_window_move(win, p.x * scale, p.y * scale);
         };

      if (app_is_activated())
         set_position();
      else
         on_activate.push_back(set_position);
   }
}}

