/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)

   Key mapping ported to C++ from GLFW3

   Copyright (c) 2009-2016 Camilla Berglund <elmindreda@glfw.org>

   This software is provided 'as-is', without any express or implied
   warranty. In no event will the authors be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
      claim that you wrote the original software. If you use this software
      in a product, an acknowledgment in the product documentation would
      be appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not
      be misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
      distribution.
=============================================================================*/
#include <elements/base_view.hpp>
#include <elements/support/canvas.hpp>
#include <elements/support/resource_paths.hpp>
#include <cairo.h>
#include <cairo-win32.h>
#include <Windowsx.h>
#include <chrono>
#include <map>
#include "utils.hpp"

namespace cycfi { namespace elements
{
   key_code translate_key(WPARAM wparam, LPARAM lparam);

   // Convert a wide Unicode string to an UTF8 string
   std::string utf8_encode(std::wstring const& wstr)
   {
      if (wstr.empty())
         return {};
      int size = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), nullptr, 0, nullptr, nullptr);
      std::string result(size, 0);
      WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &result[0], size, nullptr, nullptr);
      return result;
   }

   // Convert an UTF8 string to a wide Unicode String
   std::wstring utf8_decode(std::string const& str)
   {
      if (str.empty())
         return {};
      int size = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), nullptr, 0);
      std::wstring result( size, 0 );
      MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &result[0], size);
      return result;
   }

   namespace
   {
      constexpr unsigned IDT_TIMER1 = 100;
      HCURSOR current_cursor = nullptr;

      struct view_info
      {
         using time_point = std::chrono::time_point<std::chrono::steady_clock>;
         using key_map = std::map<key_code, key_action>;

         base_view*     vptr = nullptr;
         bool           is_dragging = false;
         HDC            hdc = nullptr;
         HDC            offscreen_hdc = nullptr;
         HBITMAP        offscreen_buff = nullptr;
         int            w = 0;
         int            h = 0;
         bool           mouse_in_window = false;
         time_point     click_start = {};
         int            click_count = 0;
         time_point     scroll_start = {};
         double         velocity = 1.0;
         point          scroll_dir;
         key_map        keys = {};
      };

      view_info* get_view_info(HWND hwnd)
      {
         auto param = GetWindowLongPtrW(hwnd, GWLP_USERDATA);
         return reinterpret_cast<view_info*>(param);
      }

      void make_offscreen_dc(HDC hdc, view_info* info, int w, int h)
      {
         info->hdc = hdc;
         info->w = w;
         info->h = h;

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
            mods |= mod_control | mod_action;
         if (test(VK_MENU))
            mods |= mod_alt;
         if (test(VK_LWIN) || test(VK_RWIN))
            mods |= mod_super;

         return mods;
      }

      mouse_button get_button(
         HWND hwnd, view_info* info, UINT message
       , WPARAM /* wparam */, LPARAM lparam)
      {
         float pos_x = GET_X_LPARAM(lparam);
         float pos_y = GET_Y_LPARAM(lparam);

         auto scale = get_scale_for_window(hwnd);
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

         auto const which =
             [message]()
             {
                 switch (message)
                 {
                     case WM_LBUTTONDOWN:
                     case WM_LBUTTONUP:
                         return mouse_button::left;

                     case WM_MBUTTONDOWN:
                     case WM_MBUTTONUP:
                         return mouse_button::middle;

                     case WM_RBUTTONDOWN:
                     case WM_RBUTTONUP:
                         return mouse_button::right;

                     default:
                         return mouse_button::left;
                 }
             }();

         return {
            down,
            info->click_count,
            which,
            get_mods(),
            { pos_x, pos_y }
         };
      }

      bool handle_key(base_view& _view, view_info::key_map& keys, key_info k)
      {
         bool repeated = false;

         if (k.action == key_action::release
            && keys[k.key] == key_action::release)
            return false;

         if (k.action == key_action::press
            && keys[k.key] == key_action::press)
            repeated = true;

         keys[k.key] = k.action;

         if (repeated)
            k.action = key_action::repeat;

         return _view.key(k);
      }

      bool on_key(HWND /* hwnd */, view_info* info, WPARAM wparam, LPARAM lparam)
      {
         auto const key = translate_key(wparam, lparam);
         auto const action = ((lparam >> 31) & 1) ? key_action::release : key_action::press;
         auto const mods = get_mods();

         if (key == key_code::unknown)
            return false;

         if (action == key_action::release && wparam == VK_SHIFT)
         {
            // HACK: Release both Shift keys on Shift up event, as when both
            //       are pressed the first release does not emit any event
            bool r1 = handle_key(*info->vptr, info->keys, { key_code::left_shift, action, mods });
            bool r2 = handle_key(*info->vptr, info->keys, { key_code::right_shift, action, mods });
            return r1 || r2;
         }
         else if (wparam == VK_SNAPSHOT)
         {
            // HACK: Key down is not reported for the Print Screen key
            bool r1 = handle_key(*info->vptr, info->keys, { key, key_action::press, mods });
            bool r2 = handle_key(*info->vptr, info->keys, { key, key_action::release, mods });
            return r1 || r2;
         }

         return handle_key(*info->vptr, info->keys, { key, action, mods });
      }

      void on_cursor(HWND hwnd, base_view* view, LPARAM lparam, cursor_tracking state)
      {
         float pos_x = GET_X_LPARAM(lparam);
         float pos_y = GET_Y_LPARAM(lparam);

         auto scale = get_scale_for_window(hwnd);
         pos_x /= scale;
         pos_y /= scale;

         view->cursor({ pos_x, pos_y }, state);
      }

      void on_scroll(HWND hwnd, view_info* info, LPARAM lparam, point dir)
      {
         constexpr auto acceleration = 1.1;
         auto now = std::chrono::steady_clock::now();
         auto elapsed = now - info->scroll_start;
         info->scroll_start = now;

         std::chrono::duration<double, std::milli> fp_ms = elapsed;

         bool reset_accel =
            elapsed > std::chrono::milliseconds(250) ||
            (info->scroll_dir.x > 0 != dir.x > 0) ||
            (info->scroll_dir.y > 0 != dir.y > 0)
            ;
         info->scroll_dir = dir;

         if (reset_accel)
            info->velocity = 1.0;
         else
            info->velocity *= acceleration;

         dir.x *= info->velocity;
         dir.y *= info->velocity;

         POINT pos;
         pos.x = GET_X_LPARAM(lparam);
         pos.y = GET_Y_LPARAM(lparam);
         ScreenToClient(hwnd, &pos);

         float scale = get_scale_for_window(hwnd);
         info->vptr->scroll(dir, { pos.x / scale, pos.y / scale });
      }

      bool on_text(base_view& view, UINT message, WPARAM wparam)
      {
         if (message == WM_UNICHAR && wparam == UNICODE_NOCHAR)
         {
            // WM_UNICHAR is not sent by Windows, but is sent by some
            // third-party input method engine Returning true here announces
            // support for this message
            return true;
         }

         bool const plain = message != WM_SYSCHAR;
         uint32_t codepoint = wparam;
         if (codepoint < 32 || (codepoint > 126 && codepoint < 160))
            return 0;

         if (plain)
            return view.text({ codepoint, get_mods() });
         return false;
      }

      LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
      {
         constexpr auto mouse_wheel_line_delta = 120.0f;

         auto* info = get_view_info(hwnd);
         switch (message)
         {
            case WM_PAINT:
               return on_paint(hwnd, info);

            case WM_ERASEBKGND:
               return true;

            case WM_LBUTTONDOWN:
            case WM_MBUTTONDOWN:
            case WM_RBUTTONDOWN:
               SetFocus(hwnd);
               [[fallthrough]];

            case WM_LBUTTONUP: case WM_MBUTTONUP: case WM_RBUTTONUP:
               // $$$ JDG $$$ todo: prevent double btn up and down
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
               on_cursor(hwnd, info->vptr, lparam, cursor_tracking::leaving);
               break;

            case WM_MOUSEHOVER:
               on_cursor(hwnd, info->vptr, lparam, cursor_tracking::hovering);
               break;

            case WM_MOUSEWHEEL:
               {
                  float delta = GET_WHEEL_DELTA_WPARAM(wparam);
                  on_scroll(hwnd, info, lparam, { 0, delta / mouse_wheel_line_delta });
               }
               break;

            case WM_MOUSEHWHEEL:
               {
                  float delta = -GET_WHEEL_DELTA_WPARAM(wparam);
                  on_scroll(hwnd, info, lparam, { delta / mouse_wheel_line_delta, 0 });
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

            case WM_KEYDOWN:
            case WM_SYSKEYDOWN:
            case WM_KEYUP:
            case WM_SYSKEYUP:
               {
                  bool handled = on_key(hwnd, info, wparam, lparam);
                  if (!handled)
                  {
                     HWND rootHWnd = GetAncestor(hwnd, GA_ROOT);
                     SendMessage(rootHWnd, message, wparam, lparam);
                     return DefWindowProc(hwnd, message, wparam, lparam);
                  }
                  return handled;
               }
               break;

            case WM_CHAR:
            case WM_SYSCHAR:
            case WM_UNICHAR:
               return on_text(*info->vptr, message, wparam);

            case WM_SETFOCUS:
               info->vptr->begin_focus();
               break;

            case WM_KILLFOCUS:
               info->vptr->end_focus();
               break;

            default:
               return DefWindowProcW(hwnd, message, wparam, lparam);
         }
         return 0;
      }

      struct init_view_class
      {
         init_view_class()
         {
            WNDCLASSW windowClass = {};
            windowClass.hbrBackground = nullptr;
            windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
            windowClass.hInstance = nullptr;
            windowClass.lpfnWndProc = WndProc;
            windowClass.lpszClassName = L"ElementsView";
            windowClass.style = CS_HREDRAW | CS_VREDRAW;
            if (!RegisterClassW(&windowClass))
               MessageBoxW(nullptr, L"Could not register class", L"Error", MB_OK);

            auto pwd = fs::current_path();
            auto resource_path = pwd / "resources";
            add_search_path(resource_path);
         }
      };
   }

   namespace
   {
      HWND make_window(base_view* _this, host_window_handle parent, RECT bounds)
      {
         static init_view_class init;

         HWND _view = CreateWindowW(
            L"ElementsView",
            nullptr,
            WS_CHILD | WS_VISIBLE,
            0, 0, 0, 0,
            parent, nullptr, nullptr,
            nullptr
         );

         MoveWindow(
            _view, bounds.left, bounds.top,
            bounds.right-bounds.left, bounds.bottom-bounds.top,
            true // repaint
         );

         view_info* info = new view_info{ _this };
         SetWindowLongPtrW(_view, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(info));

         // Create 1ms timer
         SetTimer(_view, IDT_TIMER1, 1, (TIMERPROC) nullptr);

         return _view;
      }
   }

   base_view::base_view(extent size_)
   {
	   RECT bounds = { 0, 0, LONG(size_.x), LONG(size_.y) };
	   _view = make_window(this, nullptr, bounds);
   }

   base_view::base_view(host_window_handle h)
   {
      RECT bounds;
      GetClientRect(h, &bounds);
      _view = make_window(this, h, bounds);
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
      float scale = get_scale_for_window(_view);
      return { float(pos.x) / scale, float(pos.y) / scale };
   }

   elements::extent base_view::size() const
   {
      float scale = get_scale_for_window(_view);
      RECT r;
      GetWindowRect(_view, &r);
      return { float(r.right-r.left) / scale, float(r.bottom-r.top) / scale };
   }

   void base_view::size(elements::extent p)
   {
      auto scale = get_scale_for_window(_view);
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
      auto scale = get_scale_for_window(_view);
      RECT r;
      r.left = area.left * scale;
      r.right = area.right * scale;
      r.top = area.top * scale;
      r.bottom = area.bottom * scale;
      InvalidateRect(_view, &r, false);
   }

   float base_view::hdpi_scale() const
   {
      return get_scale_for_window(_view);
   }

   std::string clipboard()
   {
      if (!OpenClipboard(nullptr))
         return {};

      HANDLE object = GetClipboardData(CF_UNICODETEXT);
      if (!object)
         return {};

      WCHAR* buffer = static_cast<WCHAR*>(GlobalLock(object));
      if (!buffer)
         return {};

      std::wstring source{ buffer, std::char_traits<WCHAR>::length(buffer) };

      GlobalUnlock(object);
      CloseClipboard();

      return utf8_encode(source);
   }

   void clipboard(std::string const& text)
   {
      auto len = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, nullptr, 0);
      if (!len)
         return;

      HANDLE object = GlobalAlloc(GMEM_MOVEABLE, len * sizeof(WCHAR));
      if (!object)
         return;

      WCHAR* buffer = static_cast<WCHAR*>(GlobalLock(object));
      if (!buffer)
      {
         GlobalFree(object);
         return;
      }

      MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, buffer, len);
      GlobalUnlock(object);

      if (!OpenClipboard(nullptr))
         return;

      EmptyClipboard();
      SetClipboardData(CF_UNICODETEXT, object);
      CloseClipboard();
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

   point scroll_direction()
   {
      return { +1.0f, +1.0f };
   }
}}

