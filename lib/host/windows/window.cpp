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
      void DisableCloseButton(HWND hwnd)
      {
         EnableMenuItem(GetSystemMenu(hwnd, FALSE), SC_CLOSE,
            MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
      }

      void DisableMinimizeButton(HWND hwnd)
      {
         SetWindowLong(hwnd, GWL_STYLE,
            GetWindowLong(hwnd, GWL_STYLE) & ~WS_MINIMIZEBOX);
      }

      void DisableMaximizeButton(HWND hwnd)
      {
         SetWindowLong(hwnd, GWL_STYLE,
            GetWindowLong(hwnd, GWL_STYLE) & ~WS_MAXIMIZEBOX);
      }

      void DisableResizing(HWND hwnd)
      {
         SetWindowLong(hwnd, GWL_STYLE,
            GetWindowLong(hwnd, GWL_STYLE) & ~WS_SIZEBOX);
         DisableMaximizeButton(hwnd);
      }

      LRESULT onClose(window* win)
      {
         if (win)
            win->on_close();
         return 0;
      }

      BOOL CALLBACK EnumChildProc(HWND child, LPARAM lParam)
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

      LRESULT onSize(HWND hwnd)
      {
         RECT bounds;
         GetClientRect(hwnd, &bounds);
         EnumChildWindows(hwnd, EnumChildProc, (LPARAM) &bounds);
         return 0;
      }

      LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
      {
         auto param = GetWindowLongPtr(hwnd, GWLP_USERDATA);
         auto* win = reinterpret_cast<window*>(param);
         switch (message)
         {
            case WM_CLOSE: return onClose(win);
            case WM_SIZE: return onSize(hwnd);

            case WM_DPICHANGED:
            {
               static int xxx = 123;
            }

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
            windowClass.lpfnWndProc = WndProc;
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
      SetWindowLongPtr(_window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

      int iDpi = GetDpiForWindow(_window);

      if (!(style_ & closable))
         DisableCloseButton(_window);
      if (!(style_ & miniaturizable))
         DisableMinimizeButton(_window);
      if (!(style_ & resizable))
         DisableResizing(_window);

      ShowWindow(_window, SW_RESTORE);
   }

   window::~window()
   {
      DeleteObject(_window);
   }

   point window::size() const
   {
      return {};
   }

   void window::size(point const& p)
   {
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
   }
}}

