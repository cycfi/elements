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
      inline base_view& get(gpointer user_data)
      {
         return *reinterpret_cast<base_view*>(user_data);
      }

      gboolean on_draw(GtkWidget* widget, cairo_t* cr, gpointer user_data)
      {
         auto& main_view = get(user_data);
         platform_access::get_host_view(main_view)->cr = cr;

         // Note that cr (cairo_t) is already clipped to only draw the
         // exposed areas of the widget.
         double left, top, right, bottom;
         cairo_clip_extents(cr, &left, &top, &right, &bottom);
         main_view.draw(rect{ float(left), float(top), float(right), float(bottom) });

         return false;
      }

      // void draw(base_view& main_view)
      // {
      //    platform_access::get_host_view(main_view)->cr = cr;

      //    // Note that cr (cairo_t) is already clipped to only draw the
      //    // exposed areas of the widget.
      //    double left, top, right, bottom;
      //    cairo_clip_extents(cr, &left, &top, &right, &bottom);
      //    main_view.draw(rect{ float(left), float(top), float(right), float(bottom) });
      // }

      template <typename Event>
      bool get_mouse(Event* event, mouse_button& btn, host_view* view)
      {
         btn.modifiers = 0;
         if (event->state & GDK_SHIFT_MASK)
            btn.modifiers |= mod_shift;
         if (event->state & GDK_CONTROL_MASK)
            btn.modifiers |= mod_control;
         if (event->state & GDK_MOD1_MASK)
            btn.modifiers |= mod_alt;
         if (event->state & GDK_SUPER_MASK)
            btn.modifiers |= mod_super;

         btn.num_clicks = view->click_count;
         btn.pos = { float(event->x), float(event->y) };
         return true;
      }

      bool get_button(GdkEventButton* event, mouse_button& btn, host_view* view)
      {
         if (event->button > 4)
            return false;

         static constexpr auto _250ms = 250;
         switch (event->type)
         {
            case GDK_BUTTON_PRESS:
               btn.down = true;
               if ((event->time - view->click_time) < _250ms)
                  ++view->click_count;
               else
                  view->click_count = 1;
               view->click_time = event->time;
               break;
            case GDK_BUTTON_RELEASE:
               btn.down = false;
               view->click_count = 0;
               break;
            default:
               return false;
         }

         if (!get_mouse(event, btn, view))
            return false;
         return true;
      }

      static gboolean
      on_button(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
      {
         auto& main_view = get(user_data);
         mouse_button btn;
         if (get_button(event, btn, platform_access::get_host_view(main_view)))
            main_view.click(btn);
         return TRUE;
      }

      static gboolean
      on_motion(GtkWidget* widget, GdkEventMotion* event, gpointer user_data)
      {
         auto& main_view = get(user_data);
         mouse_button btn;
         if (get_mouse(event, btn, platform_access::get_host_view(main_view)))
         {
            if (event->state & GDK_BUTTON1_MASK)
            {
               btn.down = true;
               btn.state = mouse_button::left;
            }
            else if (event->state & GDK_BUTTON2_MASK)
            {
               btn.down = true;
               btn.state = mouse_button::middle;
            }
            else if (event->state & GDK_BUTTON3_MASK)
            {
               btn.down = true;
               btn.state = mouse_button::right;
            }
            else
            {
               btn.down = false;
            }

            if (btn.down)
               main_view.drag(btn);
         }
         return TRUE;
      }

      static gboolean
      on_scroll(GtkWidget* widget, GdkEventScroll* event, gpointer user_data)
      {
         auto& main_view = get(user_data);
         auto* host_view = platform_access::get_host_view(main_view);
         auto elapsed = event->time - host_view->scroll_time;
         static constexpr float _1s = 1000;
         host_view->scroll_time = event->time;

         float dx = 0;
         float dy = 0;
         float step = _1s / elapsed;

         switch (event->direction)
         {
            case GDK_SCROLL_UP:
               dy = step;
               break;
            case GDK_SCROLL_DOWN:
               dy = -step;
               break;
            case GDK_SCROLL_LEFT:
               dx = step;
               break;
            case GDK_SCROLL_RIGHT:
               dy = -step;
               break;
            case GDK_SCROLL_SMOOTH:
               dx = event->delta_x;
               dy = event->delta_y;
               break;
            default:
               break;
         }

         main_view.scroll(
            { dx, dy },
            { float(event->x), float(event->y) }
         );
         return TRUE;
      }
   }

   void make_main_window(base_view& main_view, GtkWidget* window)
   {
      auto* drawing_area = gtk_drawing_area_new();

      gtk_container_add(GTK_CONTAINER(window), drawing_area);

      g_signal_connect(G_OBJECT(drawing_area), "draw",
         G_CALLBACK(on_draw), &main_view);

      //gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
      gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

      g_signal_connect(drawing_area, "button-press-event",
         G_CALLBACK(on_button), &main_view);
      g_signal_connect (drawing_area, "button-release-event",
         G_CALLBACK(on_button), &main_view);
      g_signal_connect(drawing_area, "motion-notify-event",
         G_CALLBACK(on_motion), &main_view);
      g_signal_connect(drawing_area, "scroll-event",
         G_CALLBACK(on_scroll), &main_view);

      // Ask to receive events the drawing area doesn't normally
      // subscribe to. In particular, we need to ask for the
      // button press and motion notify events that want to handle.
      gtk_widget_set_events (drawing_area,
         gtk_widget_get_events(drawing_area)
         | GDK_BUTTON_PRESS_MASK
         | GDK_BUTTON_RELEASE_MASK
         | GDK_POINTER_MOTION_MASK
         | GDK_SCROLL_MASK
      );
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
      auto x = gtk_widget_get_allocated_width(h->window);
      auto y = gtk_widget_get_allocated_height(h->window);
      refresh({ 0, 0, float(x), float(y) });
   }

   void base_view::refresh(rect area)
   {
      gtk_widget_queue_draw_area(h->window,
         area.left, area.top, area.width(), area.height());
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

