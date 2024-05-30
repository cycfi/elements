/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements/app.hpp>
#include <infra/assert.hpp>
#include <elements/base_view.hpp>
#include <elements/window.hpp>
#include <artist/resources.hpp>
#include <artist/canvas.hpp>

#include <limits.h>
#include <unistd.h>
#include <gtk/gtk.h>
#include <GL/gl.h>
#include <GL/glx.h>

#include "GrDirectContext.h"
#include "gl/GrGLInterface.h"
#include "gl/GrGLAssembleInterface.h"
#include "SkImage.h"
#include "SkColorSpace.h"
#include "SkCanvas.h"
#include "SkSurface.h"

#include <map>
#include <string>

#if defined ELEMENTS_PRINT_FPS
# include <iostream>
#endif

namespace cycfi::artist
{
   namespace
   {
      fs::path exe_path()
      {
         char result[PATH_MAX];
         ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
         return std::string(result, (count > 0)? count : 0);
      }

      fs::path find_resources()
      {
         fs::path const app_path = exe_path();
         fs::path const app_dir = app_path.parent_path();

         if (app_dir.filename() == "bin")
         {
            fs::path const path = app_dir.parent_path() / "share" / app_path.filename() / "resources";
            if (fs::is_directory(path))
               return path;
         }

         fs::path const app_resources_dir = app_dir / "resources";
         if (fs::is_directory(app_resources_dir))
            return app_resources_dir;

         return fs::current_path() / "resources";
      }
   }

   void init_paths()
   {
      add_search_path(find_resources());
   }

   fs::path get_user_fonts_directory()
   {
      return find_resources();
   }
}

namespace cycfi::elements
{
   struct host_view
   {
      host_view();
      ~host_view();

      using key_map = std::map<key_code, key_action>;

      GtkWidget*                 _widget = nullptr;   // The GTK widget
      std::uint32_t              _click_time = 0;     // Mouse button click tracking
      std::uint32_t              _click_count = 0;    // Mouse clicks count
      std::uint32_t              _scroll_time = 0;    // Scroll acceleration tracking
      point                      _cursor_position;    // Current cursor position
      key_map                    _keys;               // The key map
      int                        _modifiers = 0;      // the latest modifiers
      GtkIMContext*              _im_context;         // GTK input method context
      GdkCursorType              _active_cursor_type; // The cursor type

      point                      _size;               // The current view size
      sk_sp<const GrGLInterface> _xface;              // Skia Open GL Interface
      sk_sp<GrDirectContext>     _ctx;                // Skia Open GL context
      sk_sp<SkSurface>           _surface;            // Skia surface
      cairo_t*                   _cr;                 // The current cairo context

      std::unique_ptr<drop_info> _drop_info;          // For drag and drop
   };

   struct platform_access
   {
      inline static host_view* get_host_view(base_view& view)
      {
         return view.host();
      }
   };

   // Defined in app.cpp
   float get_scale(GtkWidget* widget);

   host_view::host_view()
    : _im_context(gtk_im_context_simple_new())
    , _active_cursor_type(GDK_ARROW)
   {
   }

   host_view::~host_view()
   {
      _widget = nullptr;
   }

   namespace
   {
      // Some globals
      host_view* host_view_under_cursor = nullptr;
      GdkCursorType view_cursor_type = GDK_ARROW;

      inline base_view& get(gpointer user_data)
      {
         return *reinterpret_cast<base_view*>(user_data);
      }

      gboolean on_draw(GtkWidget* /*widget*/, cairo_t* cr, gpointer user_data)
      {
         auto& view = get(user_data);
         auto* host_view_h = platform_access::get_host_view(view);
         host_view_h->_cr = cr;
         return false;
      }

      void realize(GtkGLArea* area, gpointer user_data)
      {
         auto error = [](char const* msg) { throw std::runtime_error(msg); };

         gtk_gl_area_make_current(area);
         if (gtk_gl_area_get_error(area) != nullptr)
            error("Error. gtk_gl_area_get_error failed");

         auto& view = get(user_data);
         auto* host_view_h = platform_access::get_host_view(view);

         host_view_h->_xface = GrGLMakeNativeInterface();
         if (host_view_h->_xface = GrGLMakeNativeInterface(); host_view_h->_xface == nullptr)
         {
            //backup plan. see https://gist.github.com/ad8e/dd150b775ae6aa4d5cf1a092e4713add?permalink_comment_id=4680136#gistcomment-4680136
            host_view_h->_xface = GrGLMakeAssembledInterface(
               nullptr, (GrGLGetProc) *
                  [](void*, const char* p) -> void*
                  {
                     return (void*)glXGetProcAddress((const GLubyte*)p);
                  }
               );
            if (host_view_h->_xface == nullptr)
               error("Error. GLMakeNativeInterface failed");
         }
         if (host_view_h->_ctx = GrDirectContext::MakeGL(host_view_h->_xface); host_view_h->_ctx == nullptr)
            error("Error. GrDirectContext::MakeGL failed");
      }

      gboolean render(GtkGLArea* /*area*/, GdkGLContext* /*context*/, gpointer user_data)
      {
         auto& view = get(user_data);
         auto* host_view_h = platform_access::get_host_view(view);
         auto error = [](char const* msg) { throw std::runtime_error(msg); };

         auto w = gtk_widget_get_allocated_width(host_view_h->_widget);
         auto h = gtk_widget_get_allocated_height(host_view_h->_widget);
         if (host_view_h->_size.x != w || host_view_h->_size.y != h)
         {
            host_view_h->_surface.reset();
            host_view_h->_size.x = w;
            host_view_h->_size.y = h;
         }

         auto scale = get_scale(host_view_h->_widget);

         if (!host_view_h->_surface)
         {
            GrGLint buffer;
            glGetIntegerv(GL_FRAMEBUFFER_BINDING, &buffer);
            GrGLFramebufferInfo info;
            info.fFBOID = (GrGLuint) buffer;
            SkColorType colorType = kRGBA_8888_SkColorType;

            info.fFormat = GL_RGBA8;
            GrBackendRenderTarget target(
                w * scale,
                h * scale,
                0, 8, info
            );

            host_view_h->_surface =
               SkSurface::MakeFromBackendRenderTarget(
                  host_view_h->_ctx.get(), target,
                  kBottomLeft_GrSurfaceOrigin, colorType, nullptr, nullptr
               );

            if (!host_view_h->_surface)
               error("Error: SkSurface::MakeRenderTarget returned null");

            gtk_widget_draw(host_view_h->_widget, host_view_h->_cr);
            return true;
         }

         SkCanvas* gpu_canvas = host_view_h->_surface->getCanvas();
         gpu_canvas->save();

         // Note that cr (cairo_t) is already clipped to only draw the
         // exposed areas of the widget.
         gpu_canvas->scale(scale, scale);
         auto cnv = canvas{gpu_canvas};

#if defined ELEMENTS_PRINT_FPS
         auto start = std::chrono::steady_clock::now();
#endif
         view.draw(cnv);

#if defined ELEMENTS_PRINT_FPS
         auto stop = std::chrono::steady_clock::now();
         auto elapsed = std::chrono::duration<double>{stop - start}.count();
         std::cout << (1.0/elapsed) << " fps" << std::endl;
#endif
         gpu_canvas->restore();
         host_view_h->_surface->flush();
         return true;
      }

      template <typename Event>
      bool get_mouse(Event* event, mouse_button& btn, host_view* view)
      {
         btn.modifiers = 0;
         if (event->state & GDK_SHIFT_MASK)
            btn.modifiers |= mod_shift;
         if (event->state & GDK_CONTROL_MASK)
            btn.modifiers |= mod_control | mod_action;
         if (event->state & GDK_MOD1_MASK)
            btn.modifiers |= mod_alt;
         if (event->state & GDK_SUPER_MASK)
            btn.modifiers |= mod_action;

         btn.num_clicks = view->_click_count;
         btn.pos = {float(event->x), float(event->y)};
         return true;
      }

      bool get_button(GdkEventButton* event, mouse_button& btn, host_view* view)
      {
         if (event->button > 4)
            return false;

         gint dbl_click_time;
         g_object_get(
            gtk_settings_get_default()
          , "gtk-double-click-time", &dbl_click_time
          , nullptr
         );

         switch (event->type)
         {
            case GDK_BUTTON_PRESS:
               btn.down = true;
               if ((event->time - view->_click_time) < guint32(dbl_click_time))
                  ++view->_click_count;
               else
                  view->_click_count = 1;
               view->_click_time = event->time;
               break;

            case GDK_BUTTON_RELEASE:
               btn.down = false;
               break;

            default:
               return false;
         }

         switch (event->button)
         {
            case 1:
               btn.state = mouse_button::left;
               break;
            case 2:
               btn.state = mouse_button::middle;
               break;
            case 3:
               btn.state = mouse_button::right;
               break;
            default:
               return false;
         }

         if (!get_mouse(event, btn, view))
            return false;
         return true;
      }

      gboolean on_button(GtkWidget* /* widget */, GdkEventButton* event, gpointer user_data)
      {
         auto& view = get(user_data);
         mouse_button btn;
         if (get_button(event, btn, platform_access::get_host_view(view)))
            view.click(btn);
         return true;
      }

      gboolean on_motion(GtkWidget* /* widget */, GdkEventMotion* event, gpointer user_data)
      {
         auto& base_view = get(user_data);
         host_view* view = platform_access::get_host_view(base_view);
         mouse_button btn;
         if (get_mouse(event, btn, view))
         {
            view->_cursor_position = btn.pos;

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
               base_view.drag(btn);
            else
               base_view.cursor(view->_cursor_position, cursor_tracking::hovering);
         }
         return true;
      }

      gboolean on_scroll(GtkWidget* /* widget */, GdkEventScroll* event, gpointer user_data)
      {
         auto& base_view = get(user_data);
         auto* host_view_h = platform_access::get_host_view(base_view);
         auto elapsed = std::max<float>(10.0f, event->time - host_view_h->_scroll_time);
         static constexpr float _1s = 100;
         host_view_h->_scroll_time = event->time;
         static constexpr float smooth_speed = 10.0f;

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
               dx = -step;
               break;
            case GDK_SCROLL_SMOOTH:
               dx = -event->delta_x * smooth_speed;
               dy = -event->delta_y * smooth_speed;
               break;
            default:
               break;
         }

         base_view.scroll(
            {dx, dy},
            {float(event->x), float(event->y)}
         );
         return true;
      }
   }

   static void change_window_cursor(GtkWidget* widget, GdkCursorType type)
   {
      GdkDisplay* display = gtk_widget_get_display(widget);
      GdkWindow* window = gtk_widget_get_window(widget);
      GdkCursor* cursor = gdk_cursor_new_for_display(display, type);
      gdk_window_set_cursor(window, cursor);
      g_object_unref(cursor);
   }

   gboolean on_event_crossing(GtkWidget* widget, GdkEventCrossing* event, gpointer user_data)
   {
      auto& base_view = get(user_data);
      auto* host_view_h = platform_access::get_host_view(base_view);
      host_view_h->_cursor_position = point{float(event->x), float(event->y)};
      if (event->type == GDK_ENTER_NOTIFY)
      {
         base_view.cursor(host_view_h->_cursor_position, cursor_tracking::entering);
         host_view_under_cursor = host_view_h;
         if (host_view_h->_active_cursor_type != view_cursor_type)
         {
            change_window_cursor(widget, view_cursor_type);
            host_view_h->_active_cursor_type = view_cursor_type;
         }
      }
      else
      {
         base_view.cursor(host_view_h->_cursor_position, cursor_tracking::leaving);
         host_view_under_cursor = nullptr;
      }
      return true;
   }

   // Defined in key.cpp
   key_code translate_key(unsigned key);

   static void on_text_entry(GtkIMContext* /* context */, const gchar* str, gpointer user_data)
   {
      auto& base_view = get(user_data);
      auto* host_view_h = platform_access::get_host_view(base_view);
      auto cp = codepoint(str);
      base_view.text({cp, host_view_h->_modifiers});
   }

   int get_mods(int state)
   {
      int mods = 0;
      if (state & GDK_SHIFT_MASK)
         mods |= mod_shift;
      if (state & GDK_CONTROL_MASK)
         mods |= mod_control | mod_action;
      if (state & GDK_MOD1_MASK)
         mods |= mod_alt;
      if (state & GDK_SUPER_MASK)
         mods |= mod_super;

      return mods;
   }

   void handle_key(base_view& _view, host_view::key_map& keys, key_info k)
   {
      bool repeated = false;

      if (k.action == key_action::release)
      {
         keys.erase(k.key);
         return;
      }

      if (k.action == key_action::press
         && keys[k.key] == key_action::press)
         repeated = true;

      keys[k.key] = k.action;

      if (repeated)
         k.action = key_action::repeat;

      _view.key(k);
   }

   gboolean on_key(GtkWidget* widget, GdkEventKey* event, gpointer user_data)
   {
      auto& base_view = get(user_data);
      auto* host_view_h = platform_access::get_host_view(base_view);
      gtk_im_context_filter_keypress(host_view_h->_im_context, event);

      int modifiers = get_mods(event->state);
      auto const action = event->type == GDK_KEY_PRESS? key_action::press : key_action::release;
      host_view_h->_modifiers = modifiers;

      // We don't want the shift key handled when obtaining the keyval,
      // so we do this again here, instead of relying on event->keyval
      guint keyval = 0;
      gdk_keymap_translate_keyboard_state(
         gdk_keymap_get_for_display(gtk_widget_get_display(widget)),
         event->hardware_keycode,
         GdkModifierType(event->state & ~GDK_SHIFT_MASK),
         event->group,
         &keyval,
         nullptr, nullptr, nullptr);

      auto const key = translate_key(keyval);
      if (key == key_code::unknown)
         return false;

      handle_key(base_view, host_view_h->_keys, {key, action, modifiers});
      return true;
   }

   void on_focus(GtkWidget* /* widget */, GdkEventFocus* event, gpointer user_data)
   {
      auto& base_view = get(user_data);
      if (event->in)
         base_view.begin_focus();
      else
         base_view.end_focus();
   }

   int poll_function(gpointer user_data)
   {
      auto& base_view = get(user_data);
      base_view.poll();
      return true;
   }

   gboolean on_drag_motion(GtkWidget* /* widget */, GdkDragContext* context, gint x, gint y, guint time, gpointer user_data)
   {
      auto& base_view = get(user_data);
      auto* host_view_h = platform_access::get_host_view(base_view);

      if (!host_view_h->_drop_info)
      {
         host_view_h->_drop_info = std::make_unique<drop_info>();
         host_view_h->_drop_info->data["text/uri-list"] = ""; // We do not have the data yet
         host_view_h->_drop_info->where = point{float(x), float(y)};
         base_view.track_drop(*host_view_h->_drop_info, cursor_tracking::entering);
      }
      else
      {
         base_view.track_drop(*host_view_h->_drop_info, cursor_tracking::hovering);
         host_view_h->_drop_info->where = point{float(x), float(y)};
      }
      host_view_h->_cursor_position = point{float(x), float(y)};

      // You can set the drag context to indicate whether the drop is accepted or not
      gdk_drag_status(context, GDK_ACTION_COPY, time);
      return true;
   }

   void on_drag_leave(GtkWidget* /* widget */, GdkDragContext* /* context */, guint /* time */, gpointer user_data)
   {
      auto& base_view = get(user_data);
      auto* host_view_h = platform_access::get_host_view(base_view);
      if (host_view_h->_drop_info)
         base_view.track_drop(*host_view_h->_drop_info, cursor_tracking::leaving);
   }

   void on_drag_data_received(
      GtkWidget* /* widget */, GdkDragContext* context, gint /* x */, gint /* y */
    , GtkSelectionData* data, guint info, guint time, gpointer user_data)
   {
      auto& base_view = get(user_data);
      auto* host_view_h = platform_access::get_host_view(base_view);
      bool success = false;

      if (host_view_h->_drop_info)
      {
         auto& base_view = get(user_data);
         auto* host_view_h = platform_access::get_host_view(base_view);

         if (info == 0)
         {
            std::string paths;
            gchar** uris = gtk_selection_data_get_uris(data);
            for (gchar** i = uris; i && *i; ++i)
            {
               if (!paths.empty())
                  paths += "\n";
               paths += *i;
            }

            host_view_h->_drop_info->data["text/uri-list"] = paths;
            success = base_view.drop(*host_view_h->_drop_info);
            g_strfreev(uris);
         }

         host_view_h->_drop_info.reset();
      }
      gtk_drag_finish(context, success, false, time);
   }

   // $$$ TODO: Investigate $$$
   // Somehow, this prevents us from having linker errors
   // Without this, we get undefined reference to `glXGetCurrentContext'
   auto proc = &glXGetProcAddress;

   GtkWidget* make_view(base_view& view, GtkWidget* parent)
   {
      auto error = [](char const* msg) { throw std::runtime_error(msg); };
      if (!proc)
         error("Error: glXGetProcAddress is null");

      auto* content_view = gtk_gl_area_new();
      gtk_container_add(GTK_CONTAINER(parent), content_view);

      g_signal_connect(content_view, "render",
         G_CALLBACK(render), &view);
      g_signal_connect(content_view, "realize",
         G_CALLBACK(realize), &view);

      // Subscribe to content_view events
      g_signal_connect(content_view, "draw",
         G_CALLBACK(on_draw), &view);
      g_signal_connect(content_view, "button-press-event",
         G_CALLBACK(on_button), &view);
      g_signal_connect (content_view, "button-release-event",
         G_CALLBACK(on_button), &view);
      g_signal_connect(content_view, "motion-notify-event",
         G_CALLBACK(on_motion), &view);
      g_signal_connect(content_view, "scroll-event",
         G_CALLBACK(on_scroll), &view);
      g_signal_connect(content_view, "enter-notify-event",
         G_CALLBACK(on_event_crossing), &view);
      g_signal_connect(content_view, "leave-notify-event",
         G_CALLBACK(on_event_crossing), &view);

      g_signal_connect(content_view, "drag-motion",
         G_CALLBACK(on_drag_motion), &view);
      g_signal_connect(content_view, "drag-leave",
         G_CALLBACK(on_drag_leave), &view);
      g_signal_connect(content_view, "drag-data-received",
         G_CALLBACK(on_drag_data_received), &view);

      // Enable drag-and-drop for uri-list(s)
      static GtkTargetEntry target_entries[] =
      {
         {const_cast<char*>("text/uri-list"), 0, 0}
      };

      gtk_drag_dest_set(content_view, GTK_DEST_DEFAULT_ALL,
         target_entries, G_N_ELEMENTS(target_entries), GDK_ACTION_COPY);

      gtk_widget_set_events(content_view,
         gtk_widget_get_events(content_view)
         | GDK_BUTTON_PRESS_MASK
         | GDK_BUTTON_RELEASE_MASK
         | GDK_POINTER_MOTION_MASK
         | GDK_SCROLL_MASK
         | GDK_ENTER_NOTIFY_MASK
         | GDK_LEAVE_NOTIFY_MASK
         | GDK_SMOOTH_SCROLL_MASK
      );

      // Subscribe to parent events
      g_signal_connect(parent, "key-press-event",
         G_CALLBACK(on_key), &view);
      g_signal_connect(parent, "key-release-event",
         G_CALLBACK(on_key), &view);
      g_signal_connect(parent, "focus-in-event",
         G_CALLBACK(on_focus), &view);
      g_signal_connect(parent, "focus-out-event",
         G_CALLBACK(on_focus), &view);

      gtk_widget_set_events(parent,
         gtk_widget_get_events(parent)
         | GDK_KEY_PRESS_MASK
         | GDK_FOCUS_CHANGE_MASK
      );

      // Subscribe to text entry commit
      g_signal_connect(view.host()->_im_context, "commit",
         G_CALLBACK(on_text_entry), &view);

      // Create 1ms timer
      g_timeout_add(1, poll_function, &view);

      // $$$ TODO: do this $$$
      // host_view_h->_scale = gdk_window_get_scale_factor(w);
      return content_view;
   }

   // Defined in window.cpp
   GtkWidget* get_window(host_window& h);
   void on_window_activate(host_window& h, std::function<void()> f);

   // Defined in app.cpp
   bool app_is_activated();

   std::filesystem::path get_app_path()
   {
      char result[PATH_MAX];
      // get the full path to the executable file of the current process.
      if (auto count = readlink("/proc/self/exe", result, PATH_MAX); count != -1)
      {
         result[count] = '\0'; // null terminate the string because readlink doesn't
         return std::filesystem::path(result);
      }
      throw std::runtime_error("Fatal Error: Failed to get application path");
   }

   fs::path find_resources()
   {
      fs::path const app_path = get_app_path();
      fs::path const app_dir = app_path.parent_path();

      if (app_dir.filename() == "bin")
      {
         fs::path path = app_dir.parent_path() / "share" / app_path.filename() / "resources";
         if (fs::is_directory(path))
            return path;
      }

      const fs::path app_resources_dir = app_dir / "resources";
      if (fs::is_directory(app_resources_dir))
         return app_resources_dir;

      return fs::current_path() / "resources";
   }

   struct init_view_class
   {
      init_view_class()
      {
         auto resource_path = find_resources();
         artist::add_search_path(resource_path);
      }
   };

   base_view::base_view(extent /* size_ */)
    : base_view(new host_view)
   {
      // $$$ FIXME: Implement Me $$$
      CYCFI_ASSERT(false, "Unimplemented");
   }

   base_view::base_view(host_view_handle h)
    : _view(h)
   {
   }

   base_view::base_view(host_window_handle h)
    : base_view(new host_view)
   {
      auto make_view =
         [this, h]()
         {
            _view->_widget = elements::make_view(*this, get_window(*h));
         };

      if (app_is_activated())
         make_view();
      else
         on_window_activate(*h, make_view);
   }

   base_view::~base_view()
   {
      if (host_view_under_cursor == _view)
         host_view_under_cursor = nullptr;
      delete _view;
      _view = nullptr;
   }

   point base_view::cursor_pos() const
   {
      return _view->_cursor_position;
   }

   elements::extent base_view::size() const
   {
      auto x = gtk_widget_get_allocated_width(_view->_widget);
      auto y = gtk_widget_get_allocated_height(_view->_widget);
      return {float(x), float(y)};
   }

   void base_view::size(elements::extent p)
   {
      // $$$ Wrong: don't size the window!!! $$$
      gtk_window_resize(GTK_WINDOW(_view->_widget), p.x, p.y);
   }

   void base_view::refresh()
   {
      gtk_widget_queue_draw(_view->_widget);
   }

   void base_view::refresh(rect area)
   {
      GtkAllocation alloc;
      gtk_widget_get_allocation(_view->_widget, &alloc);

      // Note: GTK uses int coordinates. Make sure area is not empty
      // when converting from float to int.
      gtk_widget_queue_draw_area(_view->_widget,
         std::floor(alloc.x + area.left),
         std::floor(alloc.y + area.top),
         std::max<float>(area.width(), 1),
         std::max<float>(area.height(), 1)
      );
   }

   std::string clipboard()
   {
      GtkClipboard* clip = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
      gchar* text = gtk_clipboard_wait_for_text(clip);
      return std::string(text);
   }

   void clipboard(std::string_view text)
   {
      GtkClipboard* clip = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
      gtk_clipboard_set_text(clip, text.data(), text.size());
   }

   void set_cursor(cursor_type type)
   {
      switch (type)
      {
         case cursor_type::arrow:
            view_cursor_type = GDK_ARROW;
            break;
         case cursor_type::ibeam:
            view_cursor_type = GDK_XTERM;
            break;
         case cursor_type::cross_hair:
            view_cursor_type = GDK_CROSSHAIR;
            break;
         case cursor_type::hand:
            view_cursor_type = GDK_HAND2;
            break;
         case cursor_type::h_resize:
            view_cursor_type = GDK_SB_H_DOUBLE_ARROW;
            break;
         case cursor_type::v_resize:
            view_cursor_type = GDK_SB_V_DOUBLE_ARROW;
            break;
      }

      auto* host_view_h = host_view_under_cursor;
      if (host_view_h && host_view_h->_active_cursor_type != view_cursor_type)
      {
         change_window_cursor(host_view_under_cursor->_widget, view_cursor_type);
         host_view_h->_active_cursor_type = view_cursor_type;
      }
   }

   namespace
   {
      std::string exec(char const* cmd)
      {
         std::array<char, 128> buffer;
         std::string result;
         std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
         if (!pipe)
            throw std::runtime_error("popen() failed!");
         while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
            result += buffer.data();
         return result;
      }

      point get_scroll_direction()
      {
         std::string output = exec("gsettings get org.gnome.desktop.peripherals.touchpad natural-scroll");
         output.erase(remove(output.begin(), output.end(), '\n'), output.end());

         if (output == "true")
            return {+1.0f, +1.0f};  // Assuming positive for natural scrolling
         else
            return {-1.0f, -1.0f};  // Assuming negative for traditional scrolling
      }
   }

   point scroll_direction()
   {
      using namespace std::chrono;
      static auto last_call = steady_clock::now() - seconds(10);
      static point dir = get_scroll_direction(); // Initial call

      // In case the user changed the scroll direction settings, we will call
      // get_scroll_direction() if 10 seconds have passed since the last call.
      auto now = steady_clock::now();
      if (duration_cast<seconds>(now - last_call) >= seconds(10))
      {
         dir = get_scroll_direction(); // Update the direction if 10 seconds have passed
         last_call = now;              // Update the last call time
      }

      return dir;
   }

}

