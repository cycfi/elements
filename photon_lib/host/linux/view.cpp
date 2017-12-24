/*=============================================================================
   Copyright (c) 2016-2017 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include "view_impl.hpp"
#include <cairo.h>

namespace photon
{
   ///////////////////////////////////////////////////////////////////////////
   // Main view creation callback
   std::function<std::unique_ptr<base_view>(host_view* h)> new_view;

   struct platform_access
   {
      inline static host_view* get_host_view(base_view& main_view)
      {
         return main_view.h;
      }
   };

   namespace
   {
      gboolean on_draw(GtkWidget* widget, cairo_t* cr, gpointer user_data)
      {
         auto& main_view = *reinterpret_cast<base_view*>(user_data);
         platform_access::get_host_view(main_view)->cr = cr;

         // Note that cr (cairo_t) is already clipped to only draw the
         // exposed areas of the widget.
         double left, top, right, bottom;
         cairo_clip_extents(cr, &left, &top, &right, &bottom);
         main_view.draw(rect{ float(left), float(top), float(right), float(bottom) });

         return false;
      }

      static gboolean
      on_button_press(GtkWidget* widget, GdkEventButton* event, gpointer data)
      {

         return TRUE;
      }
   }

   void make_main_window(base_view& main_view, GtkWidget* window)
   {
      auto* drawing_area = gtk_drawing_area_new();

      gtk_container_add(GTK_CONTAINER(window), drawing_area);

      g_signal_connect(G_OBJECT(drawing_area), "draw", G_CALLBACK(on_draw), &main_view);

      //gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
      gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
   }

   point base_view::cursor_pos() const
   {
      return {};
   }

   point base_view::size() const
   {
      auto x = gtk_widget_get_allocated_width(h->window);
      auto y = gtk_widget_get_allocated_height(h->window);
      return { float(x), float(y) };
   }

   void base_view::size(point p)
   {
   }

   void base_view::refresh()
   {
   }

   void base_view::refresh(rect area)
   {
   }

   void base_view::limits(view_limits limits_, bool maintain_aspect)
   {
   }

   bool base_view::is_focus() const
   {
      return false;
   }

   cairo_t* base_view::begin_drawing()
   {
      // GTK already does this for us
      return h->cr;
   }

   void base_view::end_drawing(cairo_t* ctx)
   {
      // GTK will do this for us
   }

   std::string clipboard()
   {
      return "";
   }

   void clipboard(std::string const& text)
   {
   }

   void set_cursor(cursor_type type)
   {
      switch (type)
      {
         case cursor_type::arrow:
            break;
         case cursor_type::ibeam:
            break;
         case cursor_type::cross_hair:
            break;
         case cursor_type::hand:
            break;
         case cursor_type::h_resize:
            break;
         case cursor_type::v_resize:
            break;
      }
   }
}

