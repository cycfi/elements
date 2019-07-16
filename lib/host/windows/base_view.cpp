/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements/base_view.hpp>
#include <elements/support/canvas.hpp>
#include <elements/support/resource_paths.hpp>
#include <cairo.h>
#include <cairo-win32.h>

namespace cycfi { namespace elements
{
   namespace
   {
      LRESULT onPaint(HWND hwnd, base_view* view)
      {
         if (view)
         {
            RECT dirty;
            GetUpdateRect(hwnd, &dirty, false);

            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // Create the cairo surface and context.
            cairo_surface_t* surface = cairo_win32_surface_create(hdc);
            cairo_t* context = cairo_create(surface);

            auto scale = GetDpiForWindow(hwnd) / 96.0;
            cairo_scale(context, scale, scale);

            view->draw(context,
               {
                  float(dirty.left),
                  float(dirty.top),
                  float(dirty.right),
                  float(dirty.bottom)
               }
            );

            // Cleanup.
            cairo_destroy(context);
            cairo_surface_destroy (surface);

            EndPaint(hwnd, &ps);
         }
         return 0;
      }

      LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
      {
         auto param = GetWindowLongPtr(hwnd, GWLP_USERDATA);
         auto* view = reinterpret_cast<base_view*>(param);
         switch (message)
         {
            case WM_PAINT: return onPaint(hwnd, view);
            default:
               return DefWindowProc(hwnd, message, wparam, lparam);
         }
         return 0;
      }

      struct init_view_class
      {
         init_view_class()
         {
            WNDCLASS windowClass = {0};
            windowClass.hbrBackground = NULL;
            windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
            windowClass.hInstance = NULL;
            windowClass.lpfnWndProc = WndProc;
            windowClass.lpszClassName = "ElementsView";
            windowClass.style = CS_HREDRAW | CS_VREDRAW;
            if (!RegisterClass(&windowClass))
               MessageBox(nullptr, "Could not register class", "Error", MB_OK);

            auto pwd = fs::current_path();
            auto resource_path = pwd / "resources";
            resource_paths.push_back(resource_path.string());

            canvas::load_fonts(resource_path);
         }
      };
   }

   base_view::base_view(host_window h)
   {
      static init_view_class init;

      _view = CreateWindow(
         "ElementsView",
         nullptr,
         WS_CHILD | WS_VISIBLE,
         0, 0, 0, 0,
         h, nullptr, nullptr,
         nullptr
      );

      RECT bounds;
      GetClientRect(h, &bounds);

      MoveWindow(
         _view, bounds.left, bounds.top,
         bounds.right-bounds.left, bounds.bottom-bounds.top,
         true // repaint
      );

      SetWindowLongPtr(_view, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
   }

   base_view::~base_view()
   {
      DeleteObject(_view);
   }

   point base_view::cursor_pos() const
   {
      return {};
   }

   elements::size base_view::size() const
   {
      float scale = GetDpiForWindow(_view) / 96.0;
      RECT r;
      GetWindowRect(_view, &r);
      return { float(r.right-r.left) / scale, float(r.bottom-r.top) / scale };
   }

   void base_view::size(elements::size p)
   {
      auto scale = GetDpiForWindow(_view) / 96.0;
      auto parent = GetParent(_view);
      RECT bounds;
      GetClientRect(parent, &bounds);

      MoveWindow(
         _view, bounds.left, bounds.top,
         p.x * scale, p.y * scale,
         true // repaint
      );
   }

   void base_view::refresh()
   {
      RECT bounds;
      GetClientRect(_view, &bounds);
      InvalidateRect(_view, &bounds, false);
   }

   void base_view::refresh(rect area)
   {
      RECT r;
      r.left = area.left;
      r.right = area.right;
      r.top = area.top;
      r.bottom = area.bottom;
      InvalidateRect(_view, &r, false);
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
}}

