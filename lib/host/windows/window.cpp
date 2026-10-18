/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements/window.hpp>
#include <elements/support.hpp>

#include "utils.hpp"
#include <algorithm>

namespace elements = cycfi::elements;

namespace cycfi::elements
{
   // UTF8 conversion utils defined in base_view.cpp

   // Convert a wide Unicode string to an UTF8 string
   std::string utf8_encode(std::wstring const& wstr);

   // Convert an UTF8 string to a wide Unicode String
   std::wstring utf8_decode(std::string const& str);

   namespace
   {
      struct window_info
      {
         window*     wptr = nullptr;
         view_limits limits = {};
         UINT        dpi = 96;
      };

      window_info* get_window_info(HWND hwnd)
      {
         auto param = GetWindowLongPtrW(hwnd, GWLP_USERDATA);
         return reinterpret_cast<window_info*>(param);
      }

      void disable_close(HWND hwnd)
      {
         EnableMenuItem(GetSystemMenu(hwnd, FALSE), SC_CLOSE,
            MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
      }

      void disable_minimize(HWND hwnd)
      {
         SetWindowLongW(hwnd, GWL_STYLE,
         GetWindowLongW(hwnd, GWL_STYLE) & ~WS_MINIMIZEBOX);
         SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
      }

      [[maybe_unused]]
      void disable_maximize(HWND hwnd)
      {
         SetWindowLongW(hwnd, GWL_STYLE,
         GetWindowLongW(hwnd, GWL_STYLE) & ~WS_MAXIMIZEBOX);
         SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
      }

      void disable_resize(HWND hwnd)
      {
         SetWindowLongW(hwnd, GWL_STYLE,
         GetWindowLongW(hwnd, GWL_STYLE) & ~WS_SIZEBOX & ~WS_MAXIMIZEBOX);
         SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
      }

      LRESULT on_close(window* win)
      {
         if (win && win->on_close)
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
         auto scale = get_scale_for_window(hwnd);
         auto extra = window_frame_size(hwnd);

         // Unbounded limits (full_extent) do not fit a LONG; on x64 the
         // conversion gives LONG_MIN, and the window collapses. Keep the
         // minimum within the monitor's work area, as the GTK host keeps it
         // within the screen, and the maximum within what Win32 can size.
         MONITORINFO mi{sizeof(mi)};
         GetMonitorInfoW(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST), &mi);
         float const work_w = (mi.rcWork.right - mi.rcWork.left) / scale;
         float const work_h = (mi.rcWork.bottom - mi.rcWork.top) / scale;
         float const max_size = 32767 / scale;
         limits.min.x = std::min(limits.min.x, work_w);
         limits.min.y = std::min(limits.min.y, work_h);
         limits.max.x = std::min(limits.max.x, max_size);
         limits.max.y = std::min(limits.max.y, max_size);

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
            case WM_CLOSE:
               ShowWindow(hwnd, SW_HIDE);
               return on_close(info->wptr);

            case WM_DPICHANGED:
               if (info)
               {
                  // Keep the client's logical size. Windows suggests a window
                  // rect scaled as a whole, but under per-monitor (v1)
                  // awareness the frame does not scale, so its suggestion
                  // gives the client the frame's surplus. Scale the client by
                  // the DPI ratio and add the frame back; the suggested
                  // position stays. The resulting WM_SIZE fits the view.
                  auto const& r = *reinterpret_cast<RECT const*>(lparam);
                  auto const new_dpi = UINT(LOWORD(wparam));
                  auto const ratio = float(new_dpi) / info->dpi;
                  info->dpi = new_dpi;

                  RECT client;
                  GetClientRect(hwnd, &client);
                  auto extra = window_frame_size(hwnd);
                  SetWindowPos(
                     hwnd, nullptr, r.left, r.top
                   , int(client.right * ratio + 0.5f) + extra.x
                   , int(client.bottom * ratio + 0.5f) + extra.y
                   , SWP_NOZORDER | SWP_NOACTIVATE
                  );
               }
               return 0;

            case WM_SIZE:
               return on_size(hwnd);

            case WM_SIZING:
               if (info)
               {
                  auto& r = *reinterpret_cast<RECT*>(lparam);
                  constrain_size(hwnd, r, info->limits);
               }
               break;

            default:
               return DefWindowProcW(hwnd, message, wparam, lparam);
         }
         return 0;
      }

      // Registered under the module that holds this code, as the view's
      // class is (see base_view.cpp), so each library that links Elements
      // into one process gets its own class.
      struct init_window_class
      {
         init_window_class()
         {
            GetModuleHandleExW(
               GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
               (LPCWSTR) &handle_event, &module
            );

            WNDCLASSW windowClass = {};
            windowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); ;
            windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
            windowClass.hInstance = module;
            windowClass.lpfnWndProc = handle_event;
            windowClass.lpszClassName = L"ElementsWindow";
            windowClass.style = CS_HREDRAW | CS_VREDRAW;
            if (!RegisterClassW(&windowClass))
               MessageBoxW(nullptr, L"Could not register class", L"Error", MB_OK);
         }

         ~init_window_class()
         {
            UnregisterClassW(L"ElementsWindow", module);
         }

         HMODULE module = nullptr;
      };
   }

   window::window(std::string const& name, int style_, rect const& bounds)
   {
      static init_window_class init;

      std::wstring wname = utf8_decode(name);
      #ifdef ELEMENTS_HOST_ONLY_WIN7
      auto scale = 1.0f;
      #else
      auto scale = GetDpiForSystem() / 96.0f;
      #endif

      // Without a title, a bare window: no frame at all, as on macOS.
      DWORD const win_style =
         (style_ & with_title)? WS_OVERLAPPEDWINDOW : WS_POPUP;

      _window = CreateWindowW(
         L"ElementsWindow",
         wname.c_str(),
         win_style,
         bounds.left * scale, bounds.top * scale,
         bounds.width() * scale, bounds.height() * scale,
         nullptr, nullptr, init.module,
         nullptr
      );

      auto* info = new window_info{this};
      #ifndef ELEMENTS_HOST_ONLY_WIN7
      info->dpi = GetDpiForWindow(_window);
      #endif
      SetWindowLongPtrW(_window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(info));

      if (style_ & with_title)
      {
         if (!(style_ & closable))
            disable_close(_window);
         if (!(style_ & miniaturizable))
            disable_minimize(_window);
         if (!(style_ & resizable))
            disable_resize(_window);
      }

      // Sets the app icon for the window to show on the titlebar.
      // The IDI_ELEMENTS_APP_ICON icon id should be defined in a resource file.
      HICON hIcon = LoadIcon(GetModuleHandle(nullptr), TEXT("IDI_ELEMENTS_APP_ICON"));
      if (hIcon)
         ::SendMessage(_window, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

      // The bounds give the content's size: the frame goes around it
      size({bounds.width(), bounds.height()});

      ShowWindow(_window, SW_RESTORE);
   }

   window::~window()
   {
      delete get_window_info(_window);
      DeleteObject(_window);
   }

   // The size is the content's, the client area, the frame left out, as
   // on every host
   point window::size() const
   {
      auto scale = get_scale_for_window(_window);
      RECT content;
      GetClientRect(_window, &content);
      return {
         float((content.right - content.left) / scale),
         float((content.bottom - content.top) / scale)
      };
   }

   void window::size(point const& p)
   {
      auto scale = get_scale_for_window(_window);
      auto extra = window_frame_size(_window);
      RECT frame;
      GetWindowRect(_window, &frame);
      frame.right = frame.left + extra.x + LONG(p.x * scale);
      frame.bottom = frame.top + extra.y + LONG(p.y * scale);
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
      auto scale = get_scale_for_window(_window);
      RECT frame;
      GetWindowRect(_window, &frame);
      return {float(frame.left / scale), float(frame.top / scale)};
   }

   void window::position(point const& p)
   {
      auto scale = get_scale_for_window(_window);
      RECT frame;
      GetWindowRect(_window, &frame);

      MoveWindow(
         _window, p.x * scale, p.y * scale,
         frame.right - frame.left,
         frame.bottom - frame.top,
         true // repaint
      );
   }
}

