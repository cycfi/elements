/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements/window.hpp>
#include <elements/support.hpp>

#include "utils.hpp"

namespace elements = cycfi::elements;

namespace cycfi { namespace elements
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
      }

      void disable_maximize(HWND hwnd)
      {
         SetWindowLongW(hwnd, GWL_STYLE,
            GetWindowLongW(hwnd, GWL_STYLE) & ~WS_MAXIMIZEBOX);
      }

      void disable_resize(HWND hwnd)
      {
         SetWindowLongW(hwnd, GWL_STYLE,
            GetWindowLongW(hwnd, GWL_STYLE) & ~WS_SIZEBOX);
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
         auto scale = get_scale_for_window(hwnd);
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
               return DefWindowProcW(hwnd, message, wparam, lparam);
         }
         return 0;
      }

      struct init_window_class
      {
         init_window_class()
         {
            WNDCLASSW windowClass = {};
            windowClass.hbrBackground = nullptr;
            windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
            windowClass.hInstance = GetModuleHandleW(nullptr);
            windowClass.lpfnWndProc = handle_event;
            windowClass.lpszClassName = L"ElementsWindow";
            windowClass.style = CS_HREDRAW | CS_VREDRAW;
            if (!RegisterClassW(&windowClass))
               MessageBoxW(nullptr, L"Could not register class", L"Error", MB_OK);
         }
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

      _window = CreateWindowW(
         L"ElementsWindow",
         wname.c_str(),
         WS_OVERLAPPEDWINDOW,
         bounds.left * scale, bounds.top * scale,
         bounds.width() * scale, bounds.height() * scale,
         nullptr, nullptr, nullptr,
         nullptr
      );

      auto* info = new window_info{ this };
      SetWindowLongPtrW(_window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(info));

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
      auto scale = get_scale_for_window(_window);
      RECT frame;
      GetWindowRect(_window, &frame);
      return {
         float((frame.right - frame.left) / scale),
         float((frame.bottom - frame.top) / scale)
      };
   }

   void window::size(point const& p)
   {
      auto scale = get_scale_for_window(_window);
      RECT frame;
      GetWindowRect(_window, &frame);
      frame.right = frame.left + (p.x * scale);
      frame.bottom = frame.top + (p.y * scale);
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
      return { float(frame.left / scale), float(frame.top / scale) };
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
}}

