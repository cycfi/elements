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
#include <chrono>

namespace cycfi { namespace elements
{
   namespace
   {
      constexpr unsigned IDT_TIMER1 = 100;
      HCURSOR current_cursor = nullptr;

      struct view_info
      {
         base_view*     vptr = nullptr;
         bool           is_dragging = false;
         HDC            hdc = nullptr;
         HDC            offscreen_hdc = nullptr;
         HBITMAP        offscreen_buff = nullptr;
         int            w = 0;
         int            h = 0;
         bool           mouse_in_window = false;

         using time_point = std::chrono::time_point<std::chrono::steady_clock>;

         time_point     click_start;
         int            click_count = 0;
         time_point     scroll_start;
         double         velocity = 0;
      };

      view_info* get_view_info(HWND hwnd)
      {
         auto param = GetWindowLongPtr(hwnd, GWLP_USERDATA);
         return reinterpret_cast<view_info*>(param);
      }

      void make_offscreen_dc(HDC hdc, view_info* info, int w, int h)
      {
         info->hdc = hdc;
         info->w = w;
         info->h = w;

         // Free-up the previous off-screen DC
         if (info->offscreen_buff)
            DeleteObject(info->offscreen_buff);
         if (info->offscreen_hdc)
            DeleteDC(info->offscreen_hdc);

         // Create an off-screen DC for double-buffering
         info->offscreen_hdc = CreateCompatibleDC(hdc);
         info->offscreen_buff = CreateCompatibleBitmap(hdc, w, h);
      }

      LRESULT on_paint(HWND hwnd, view_info* info)
      {
         if (base_view* view = info->vptr)
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

            if (hdc != info->hdc || win_width != info->w || win_height != info->h)
               make_offscreen_dc(hdc, info, win_width, win_height);

            HANDLE hold = SelectObject(info->offscreen_hdc, info->offscreen_buff);

            // Create the cairo surface and context.
            cairo_surface_t* surface = cairo_win32_surface_create(info->offscreen_hdc);
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
            cairo_surface_destroy(surface);

            // Transfer the off-screen DC to the screen
            auto w = dirty.right-dirty.left;
            auto h = dirty.bottom-dirty.top;
            BitBlt(hdc, dirty.left, dirty.top, w, h, info->offscreen_hdc
              , dirty.left, dirty.top, SRCCOPY);

            SelectObject(info->offscreen_hdc, hold);
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
            case WM_LBUTTONDOWN:
            case WM_MBUTTONDOWN:
            case WM_RBUTTONDOWN:
               {
                  auto now = std::chrono::steady_clock::now();
                  auto elapsed = now - info->click_start;
                  info->click_start = now;
                  if (elapsed > std::chrono::milliseconds(GetDoubleClickTime()))
                     info->click_count = 1;
                  else
                     ++info->click_count;
                  if (!info->is_dragging)
                  {
                     info->is_dragging = true;
                     SetCapture(hwnd);
                  }
                  down = true;
               }
               break;

            case WM_LBUTTONUP:
            case WM_MBUTTONUP:
            case WM_RBUTTONUP:
               down = false;
               if (info->is_dragging)
               {
                  info->is_dragging = false;
                  ReleaseCapture();
               }
               break;
         }

         int click_count = 1;

         mouse_button::what which;
         switch (message)
         {
            case WM_LBUTTONDOWN:
            case WM_LBUTTONUP:
               which = mouse_button::left;
               break;

            case WM_MBUTTONDOWN:
            case WM_MBUTTONUP:
               which = mouse_button::middle;
               break;

            case WM_RBUTTONDOWN:
            case WM_RBUTTONUP:
               which = mouse_button::right;
               break;
         }

         return {
            down,
            info->click_count,
            mouse_button::left,
            get_mods(),
            { pos_x, pos_y }
         };
      }

      void on_cursor(HWND hwnd, base_view* view, LPARAM lparam, cursor_tracking state)
      {
         float pos_x = GET_X_LPARAM(lparam);
         float pos_y = GET_Y_LPARAM(lparam);

         auto scale = GetDpiForWindow(hwnd) / 96.0;
         pos_x /= scale;
         pos_y /= scale;

         view->cursor({ pos_x, pos_y }, state);
      }

      void on_scroll(HWND hwnd, view_info* info, LPARAM lparam, point dir)
      {
         auto now = std::chrono::steady_clock::now();
         auto elapsed = now - info->scroll_start;
         info->scroll_start = now;

         std::chrono::duration<double, std::milli> fp_ms = elapsed;
         auto velocity = (1.0 / fp_ms.count());

         if (elapsed > std::chrono::milliseconds(500))
            info->velocity = velocity;
         else
            // Leaky integrator
            info->velocity = velocity + 0.9 * (info->velocity - velocity);

         dir.x *= info->velocity;
         dir.y *= info->velocity;

         POINT pos;
         pos.x = GET_X_LPARAM(lparam);
         pos.y = GET_Y_LPARAM(lparam);
         ScreenToClient(hwnd, &pos);

         float scale = GetDpiForWindow(hwnd) / 96.0;
         info->vptr->scroll(dir, { pos.x / scale, pos.y / scale });
      }

      LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
      {
         auto param = GetWindowLongPtr(hwnd, GWLP_USERDATA);
         auto* info = get_view_info(hwnd);
         switch (message)
         {
            case WM_PAINT:
               return on_paint(hwnd, info);

            case WM_ERASEBKGND:
               return true;

            case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
            case WM_MBUTTONDBLCLK: case WM_MBUTTONDOWN:
            case WM_RBUTTONDBLCLK: case WM_RBUTTONDOWN:
            case WM_LBUTTONUP: case WM_MBUTTONUP: case WM_RBUTTONUP:
               // $$$JDG$$$ todo: prevent double btn up and down
               info->vptr->click(get_button(hwnd, info, message, wparam, lparam));
               break;

            case WM_MOUSEMOVE:
               if (info->is_dragging)
               {
                  info->vptr->drag(get_button(hwnd, info, message, wparam, lparam));
               }
               else
               {
                  if (!info->mouse_in_window)
                  {
                     on_cursor(hwnd, info->vptr, lparam, cursor_tracking::entering);
                     info->mouse_in_window = true;
                  }
                  TRACKMOUSEEVENT tme;
                  tme.cbSize = sizeof(tme);
                  tme.hwndTrack = hwnd;
                  tme.dwFlags = TME_HOVER | TME_LEAVE;
                  tme.dwHoverTime = 16;
                  TrackMouseEvent(&tme);
               }
               break;

            case WM_MOUSELEAVE:
               info->mouse_in_window = false;
               on_cursor(hwnd, info->vptr, lparam, cursor_tracking::entering);
               break;

            case WM_MOUSEHOVER:
               on_cursor(hwnd, info->vptr, lparam, cursor_tracking::hovering);
               break;

            case WM_MOUSEWHEEL:
               {
                  float delta = GET_WHEEL_DELTA_WPARAM(wparam);
                  on_scroll(hwnd, info, lparam, { 0, delta });
               }
               break;

            case WM_MOUSEHWHEEL:
               {
                  float delta = -GET_WHEEL_DELTA_WPARAM(wparam);
                  on_scroll(hwnd, info, lparam, { delta, 0 });
               }
               break;

            case WM_SETCURSOR:
               if (LOWORD(lparam) == HTCLIENT && current_cursor != GetCursor())
                  SetCursor(current_cursor);
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
      auto info = get_view_info(_view);

      // Free-up the off-screen DC
      if (info->offscreen_buff)
         DeleteObject(info->offscreen_buff);
      if (info->offscreen_hdc)
         DeleteDC(info->offscreen_hdc);

      KillTimer(_view, IDT_TIMER1);
      delete info;
      DeleteObject(_view);
   }

   point base_view::cursor_pos() const
   {
      POINT pos;
      GetCursorPos(&pos);
      ScreenToClient(_view, &pos);
      float scale = GetDpiForWindow(_view) / 96.0;
      return { float(pos.x) / scale, float(pos.y) / scale };
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
      struct cursors
      {
         cursors()
         {
            _cursors[cursor_type::arrow]        = LoadCursor(nullptr, IDC_ARROW);
            _cursors[cursor_type::ibeam]        = LoadCursor(nullptr, IDC_IBEAM);
            _cursors[cursor_type::cross_hair]   = LoadCursor(nullptr, IDC_CROSS);
            _cursors[cursor_type::hand]         = LoadCursor(nullptr, IDC_HAND);
            _cursors[cursor_type::h_resize]     = LoadCursor(nullptr, IDC_SIZEWE);
            _cursors[cursor_type::v_resize]     = LoadCursor(nullptr, IDC_SIZENS);
         }

         std::map<cursor_type, HCURSOR> _cursors;
      };
      static cursors data;

      HCURSOR cursor = data._cursors[type];
      if (cursor != GetCursor())
      {
         current_cursor = cursor;
         SetCursor(cursor);
         ShowCursor(true);
      }
   }
}}

