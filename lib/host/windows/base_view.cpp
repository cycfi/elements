/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements/base_view.hpp>
#include <elements/support/canvas.hpp>
#include <elements/support/resource_paths.hpp>
#include <cairo.h>
#include <cairo-win32.h>
#include <Windowsx.h>

namespace cycfi { namespace elements
{
   namespace
   {
      constexpr unsigned IDT_TIMER1 = 100;

      struct view_info
      {
         base_view*  vptr = nullptr;
         bool        is_dragging = false;
      };

      view_info* get_view_info(HWND hwnd)
      {
         auto param = GetWindowLongPtr(hwnd, GWLP_USERDATA);
         return reinterpret_cast<view_info*>(param);
      }

      LRESULT on_paint(HWND hwnd, base_view* view)
      {
         if (view)
         {
            RECT dirty;
            GetUpdateRect(hwnd, &dirty, false);

            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            SetBkMode(hdc, TRANSPARENT);

            RECT r;
            GetWindowRect(hwnd, &r);
            auto win_width = r.right-r.left;
            auto win_height = r.bottom-r.top;

            // Create an off-screen DC for double-buffering
            HDC offscreen_hdc = CreateCompatibleDC(hdc);
            HBITMAP offscreen_buff = CreateCompatibleBitmap(hdc, win_width, win_height);
            HANDLE offscreen_handle = SelectObject(offscreen_hdc, offscreen_buff);

            // Create the cairo surface and context.
            cairo_surface_t* surface = cairo_win32_surface_create(offscreen_hdc);
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

            // Transfer the off-screen DC to the screen
            auto w = dirty.right-dirty.left;
            auto h = dirty.bottom-dirty.top;
            BitBlt(hdc, dirty.left, dirty.top, w, h, offscreen_hdc
              , dirty.left, dirty.top, SRCCOPY);

            // Free-up the off-screen DC
            SelectObject(offscreen_hdc, offscreen_handle);

            DeleteObject(offscreen_buff);
            DeleteDC(offscreen_hdc);

            EndPaint(hwnd, &ps);
         }
         return 0;
      }

      int get_mods()
      {
         int mods = 0;

         auto&& test = [](int key)
         {
            return GetAsyncKeyState(key) & 0x8000;
         };

         if (test(VK_SHIFT))
            mods |= mod_shift;
         if (test(VK_CONTROL))
            mods |= mod_control;
         if (test(VK_MENU))
            mods |= mod_alt;
         if (test(VK_LWIN) || test(VK_RWIN))
            mods |= mod_super;

         return mods;
      }

      mouse_button get_button(
         HWND hwnd, view_info* info, UINT message
       , WPARAM wparam, LPARAM lparam)
      {
         float pos_x = GET_X_LPARAM(lparam);
         float pos_y = GET_Y_LPARAM(lparam);

         auto scale = GetDpiForWindow(hwnd) / 96.0;
         pos_x /= scale;
         pos_y /= scale;

         bool down = info->is_dragging;
         switch (message)
         {
            case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
            case WM_MBUTTONDBLCLK: case WM_MBUTTONDOWN:
            case WM_RBUTTONDBLCLK: case WM_RBUTTONDOWN:
               if (!info->is_dragging)
               {
                  info->is_dragging = true;
                  SetCapture(hwnd);
               }
               down = true;
               break;

            case WM_LBUTTONUP: case WM_MBUTTONUP: case WM_RBUTTONUP:
               down = false;
               if (info->is_dragging)
               {
                  info->is_dragging = false;
                  ReleaseCapture();
               }
               break;
         }

         int click_count = 1;
         switch (message)
         {
            case WM_LBUTTONDBLCLK: case WM_MBUTTONDBLCLK: case WM_RBUTTONDBLCLK:
               click_count = 2;
         }

         mouse_button::what which;
         switch (message)
         {
            case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK: case WM_LBUTTONUP:
               which = mouse_button::left;
               break;

            case WM_MBUTTONDBLCLK:
            case WM_MBUTTONDOWN:
            case WM_MBUTTONUP:
               which = mouse_button::middle;
               break;

            case WM_RBUTTONDBLCLK:
            case WM_RBUTTONDOWN:
            case WM_RBUTTONUP:
               which = mouse_button::right;
               break;
         }

         return {
            down,
            click_count,
            mouse_button::left,
            get_mods(),
            { pos_x, pos_y }
         };
      }

      LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
      {
         auto param = GetWindowLongPtr(hwnd, GWLP_USERDATA);
         auto* info = get_view_info(hwnd);
         switch (message)
         {
            case WM_PAINT:
               return on_paint(hwnd, info->vptr);

            case WM_ERASEBKGND:
               return true;

            case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
            case WM_MBUTTONDBLCLK: case WM_MBUTTONDOWN:
            case WM_RBUTTONDBLCLK: case WM_RBUTTONDOWN:
            case WM_LBUTTONUP: case WM_MBUTTONUP: case WM_RBUTTONUP:
               info->vptr->click(get_button(hwnd, info, message, wparam, lparam));
               break;

            case WM_MOUSEMOVE:
               if (info->is_dragging)
               {
                  // TRACKMOUSEEVENT tme;
                  // tme.cbSize = sizeof(tme);
                  // tme.hwndTrack = hwnd;
                  // tme.dwFlags = TME_HOVER | TME_LEAVE;
                  // tme.dwHoverTime = HOVER_DEFAULT;
                  // TrackMouseEvent(&tme);
                  info->vptr->drag(get_button(hwnd, info, message, wparam, lparam));
               }
               break;

            case WM_MOUSELEAVE:
               info->is_dragging = false;
               break;

            case WM_MOUSEHOVER:
               info->is_dragging = false;
               break;

            case WM_TIMER:
               if (wparam == IDT_TIMER1)
                  info->vptr->poll();
               break;

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

      view_info* info = new view_info{ this };
      SetWindowLongPtr(_view, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(info));

      // Create 16ms (60Hz) timer
      SetTimer(_view, IDT_TIMER1, 16, (TIMERPROC) nullptr);
   }

   base_view::~base_view()
   {
      KillTimer(_view, IDT_TIMER1);
      delete get_view_info(_view);
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
      auto scale = GetDpiForWindow(_view) / 96.0;
      RECT r;
      r.left = area.left * scale;
      r.right = area.right * scale;
      r.top = area.top * scale;
      r.bottom = area.bottom * scale;
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

