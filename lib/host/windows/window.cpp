/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements/window.hpp>
#include <elements/support.hpp>

namespace elements = cycfi::elements;

namespace cycfi { namespace elements
{
   LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
   {
      switch (message)
      {
         case WM_CHAR:
         if (wparam==VK_ESCAPE)
         {
            DestroyWindow(hwnd);
         }

         case WM_DESTROY:
            PostQuitMessage(0);
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
         windowClass.lpfnWndProc = WndProc;
         windowClass.lpszClassName = "ElementsWindow";
         windowClass.style = CS_HREDRAW | CS_VREDRAW;
         if (!RegisterClass(&windowClass))
            MessageBox(nullptr, "Could not register class", "Error", MB_OK);
      }
   };

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
         this
      );
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

