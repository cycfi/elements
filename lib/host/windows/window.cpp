/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements/window.hpp>
#include <elements/support.hpp>

namespace elements = cycfi::elements;

namespace cycfi { namespace elements
{
   namespace
   {
      struct window_info
      {
         window*     wptr = nullptr;
         view_limits limits;
      };

      window_info* get_window_info(HWND hwnd)
      {
         auto param = GetWindowLongPtr(hwnd, GWLP_USERDATA);
         return reinterpret_cast<window_info*>(param);
      }

      void disable_close(HWND hwnd)
      {
         EnableMenuItem(GetSystemMenu(hwnd, FALSE), SC_CLOSE,
            MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
      }

      void disable_minimize(HWND hwnd)
      {
         SetWindowLong(hwnd, GWL_STYLE,
            GetWindowLong(hwnd, GWL_STYLE) & ~WS_MINIMIZEBOX);
      }

      void disable_maximize(HWND hwnd)
      {
         SetWindowLong(hwnd, GWL_STYLE,
            GetWindowLong(hwnd, GWL_STYLE) & ~WS_MAXIMIZEBOX);
      }

      void disable_resize(HWND hwnd)
      {
         SetWindowLong(hwnd, GWL_STYLE,
            GetWindowLong(hwnd, GWL_STYLE) & ~WS_SIZEBOX);
         disable_maximize(hwnd);
      }

      LRESULT on_close(window* win)
      {
         if (win)
            win->on_close();
         return 0;
      }

      BOOL CALLBACK for_each_child(HWND child, LPARAM lParam)
      {
         LPRECT bounds = (LPRECT) lParam;
         MoveWindow(
            child,
            0, 0,
            bounds->right,
            bounds->bottom,
            TRUE);

         // Make sure the child window is visible.
         ShowWindow(child, SW_SHOW);
         return true;
      }

      LRESULT on_size(HWND hwnd)
      {
         RECT bounds;
         GetClientRect(hwnd, &bounds);
         EnumChildWindows(hwnd, for_each_child, (LPARAM) &bounds);
         return 0;
      }

      POINT window_frame_size(HWND hwnd)
      {
         RECT content, frame;
         POINT extra;
         GetClientRect(hwnd, &content);
         GetWindowRect(hwnd, &frame);
         extra.x = (frame.right - frame.left) - content.right;
         extra.y = (frame.bottom - frame.top) - content.bottom;
         return extra;
      }

      void constrain_size(HWND hwnd, RECT& r, view_limits limits)
      {
         auto scale = GetDpiForWindow(hwnd) / 96.0;
         auto extra = window_frame_size(hwnd);
         auto w = ((r.right - r.left) - extra.x) / scale;
         auto h = ((r.bottom - r.top) - extra.y) / scale;

         if (w > limits.max.x)
            r.right = r.left + extra.x + (limits.max.x * scale);
         if (w < limits.min.x)
            r.right = r.left + extra.x + (limits.min.x * scale);
         if (h > limits.max.y)
            r.bottom = r.top + extra.y + (limits.max.y * scale);
         if (h < limits.min.y)
            r.bottom = r.top + extra.y + (limits.min.y * scale);
      }

      LRESULT CALLBACK handle_event(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
      {
         auto* info = get_window_info(hwnd);
         switch (message)
         {
            case WM_CLOSE: return on_close(info->wptr);

            case WM_DPICHANGED:
            case WM_SIZE: return on_size(hwnd);

            case WM_SIZING:
               if (info)
               {
                  auto& r = *reinterpret_cast<RECT*>(lparam);
                  constrain_size(hwnd, r, info->limits);
               }
               break;

            default:
               return DefWindowProc(hwnd, message, wparam, lparam);
         }
         return 0;
      }

      struct init_window_class
      {
         init_window_class()
         {
            WNDCLASS windowClass = {0};
            windowClass.hbrBackground = NULL;
            windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
            windowClass.hInstance = NULL;
            windowClass.lpfnWndProc = handle_event;
            windowClass.lpszClassName = "ElementsWindow";
            windowClass.style = CS_HREDRAW | CS_VREDRAW;
            if (!RegisterClass(&windowClass))
               MessageBox(nullptr, "Could not register class", "Error", MB_OK);
         }
      };
   }

   window::window(std::string const& name, int style_, rect const& bounds)
   {
      static init_window_class init;

      _window = CreateWindow(
         "ElementsWindow",
         name.c_str(),
         WS_OVERLAPPEDWINDOW,
         bounds.left, bounds.top,
         bounds.width(), bounds.height(),
         nullptr, nullptr, nullptr,
         nullptr
      );

      window_info* info = new window_info{ this };
      SetWindowLongPtr(_window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(info));

      if (!(style_ & closable))
         disable_close(_window);
      if (!(style_ & miniaturizable))
         disable_minimize(_window);
      if (!(style_ & resizable))
         disable_resize(_window);

      ShowWindow(_window, SW_RESTORE);
   }

   window::~window()
   {
      delete get_window_info(_window);
      DeleteObject(_window);
   }

   point window::size() const
   {
      RECT frame;
      GetWindowRect(_window, &frame);
      return {
         float(frame.right - frame.left),
         float(frame.bottom - frame.top)
      };
   }

   void window::size(point const& p)
   {
      RECT frame;
      GetWindowRect(_window, &frame);
      frame.right = frame.left + p.x;
      frame.bottom = frame.top + p.y;
      constrain_size(
         _window, frame, get_window_info(_window)->limits);

      MoveWindow(
         _window, frame.left, frame.top,
         frame.right - frame.left,
         frame.bottom - frame.top,
         true // repaint
      );
   }

   void window::limits(view_limits limits_)
   {
      get_window_info(_window)->limits = limits_;
      RECT frame;
      GetWindowRect(_window, &frame);
      constrain_size(
         _window, frame, get_window_info(_window)->limits);

      MoveWindow(
         _window, frame.left, frame.top,
         frame.right - frame.left,
         frame.bottom - frame.top,
         true // repaint
      );
   }

   point window::position() const
   {
      RECT frame;
      GetWindowRect(_window, &frame);
      return { float(frame.left), float(frame.top) };
   }

   void window::position(point const& p)
   {
      RECT frame;
      GetWindowRect(_window, &frame);

      MoveWindow(
         _window, p.x, p.y,
         frame.right - frame.left,
         frame.bottom - frame.top,
         true // repaint
      );
   }
}}

