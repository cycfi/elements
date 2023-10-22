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
#include <artist/canvas.hpp>
#include <artist/resources.hpp>

#ifndef UNICODE
# define UNICODE
#endif

#include <SDKDDKVer.h>
#include <windows.h>
#include <gl/gl.h>

#include <ShellScalingAPI.h>
#include <Windowsx.h>
#include <chrono>
#include <map>

#if defined ELEMENTS_PRINT_FPS
# include <iostream>
#endif

#include <GrContext.h>
#include <gl/GrGLInterface.h>
#include <SkImage.h>
#include <SkSurface.h>
#include <tools/sk_app/DisplayParams.h>
#include <tools/sk_app/WindowContext.h>
#include <tools/sk_app/win/WindowContextFactory_win.h>

#include "utils.hpp"

namespace cycfi::artist
{
   namespace
   {
      fs::path find_resources()
      {
         TCHAR exe_path[MAX_PATH];
         GetModuleFileName(nullptr, exe_path, MAX_PATH);

         fs::path const app_path = exe_path;
         fs::path const app_dir = app_path.parent_path();

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
         using skia_context = std::unique_ptr<sk_app::WindowContext>;

         base_view*     _vptr = nullptr;
         bool           _is_dragging = false;
         HDC            _hdc = nullptr;
         bool           _mouse_in_window = false;
         time_point     _click_start = {};
         int            _click_count = 0;
         time_point     _scroll_start = {};
         double         _velocity = 0;
         point          _scroll_dir;
         key_map        _keys = {};
         skia_context   _skia_context;
      };

      view_info* get_view_info(HWND hwnd)
      {
         auto param = GetWindowLongPtrW(hwnd, GWLP_USERDATA);
         return reinterpret_cast<view_info*>(param);
      }

      LRESULT on_paint(HWND hwnd, view_info* info)
      {
         if (base_view* view = info->_vptr)
         {
            RECT dirty;
            GetUpdateRect(hwnd, &dirty, false);

            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            SetBkMode(hdc, TRANSPARENT);

#if defined ELEMENTS_PRINT_FPS
            auto start = std::chrono::high_resolution_clock::now();
#endif

            RECT r;
            GetWindowRect(hwnd, &r);
            auto win_width = r.right-r.left;
            auto win_height = r.bottom-r.top;
            auto const& _skia_context = info->_skia_context;

            if (_skia_context->width() != win_width || _skia_context->height() != win_height)
               _skia_context->resize(win_width, win_height);

            auto surface = _skia_context->getBackbufferSurface();
            if (surface)
            {
               auto draw =
                  [&](auto& cnv_)
                  {
                     auto scale = get_scale_for_window(hwnd);
                     auto cnv = canvas{cnv_};
                     cnv.pre_scale(scale);

                     view->draw(cnv,
                        {
                           float(dirty.left),
                           float(dirty.top),
                           float(dirty.right),
                           float(dirty.bottom)
                        }
                     );
                  };

               SkCanvas* gpu_canvas = surface->getCanvas();
               gpu_canvas->save();
               draw(gpu_canvas);
               gpu_canvas->restore();
               surface->flush();
               _skia_context->swapBuffers();
            }

#if defined ELEMENTS_PRINT_FPS
            auto stop = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration<double>{stop - start}.count();
            std::cout << (1.0/elapsed) << " fps" << std::endl;
#endif
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

         bool down = info->_is_dragging;
         switch (message)
         {
            case WM_LBUTTONDOWN:
            case WM_MBUTTONDOWN:
            case WM_RBUTTONDOWN:
               {
                  auto now = std::chrono::steady_clock::now();
                  auto elapsed = now - info->_click_start;
                  info->_click_start = now;
                  if (elapsed > std::chrono::milliseconds(GetDoubleClickTime()))
                     info->_click_count = 1;
                  else
                     ++info->_click_count;
                  if (!info->_is_dragging)
                  {
                     info->_is_dragging = true;
                     SetCapture(hwnd);
                  }
                  down = true;
               }
               break;

            case WM_LBUTTONUP:
            case WM_MBUTTONUP:
            case WM_RBUTTONUP:
               down = false;
               if (info->_is_dragging)
               {
                  info->_is_dragging = false;
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
            info->_click_count,
            which,
            get_mods(),
            {pos_x, pos_y}
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
            bool r1 = handle_key(*info->_vptr, info->_keys, {key_code::left_shift, action, mods});
            bool r2 = handle_key(*info->_vptr, info->_keys, {key_code::right_shift, action, mods});
            return r1 || r2;
         }
         else if (wparam == VK_SNAPSHOT)
         {
            // HACK: Key down is not reported for the Print Screen key
            bool r1 = handle_key(*info->_vptr, info->_keys, {key, key_action::press, mods});
            bool r2 = handle_key(*info->_vptr, info->_keys, {key, key_action::release, mods});
            return r1 || r2;
         }

         return handle_key(*info->_vptr, info->_keys, {key, action, mods});
      }

      void on_cursor(HWND hwnd, base_view* view, LPARAM lparam, cursor_tracking state)
      {
         float pos_x = GET_X_LPARAM(lparam);
         float pos_y = GET_Y_LPARAM(lparam);

         auto scale = get_scale_for_window(hwnd);
         pos_x /= scale;
         pos_y /= scale;

         view->cursor({pos_x, pos_y}, state);
      }

      void on_scroll(HWND hwnd, view_info* info, LPARAM lparam, point dir)
      {
         constexpr auto acceleration = 1.1;
         auto now = std::chrono::steady_clock::now();
         auto elapsed = now - info->_scroll_start;
         info->_scroll_start = now;

         std::chrono::duration<double, std::milli> fp_ms = elapsed;

         bool reset_accel =
            elapsed > std::chrono::milliseconds(250) ||
            (info->_scroll_dir.x > 0 != dir.x > 0) ||
            (info->_scroll_dir.y > 0 != dir.y > 0)
            ;
         info->_scroll_dir = dir;

         if (reset_accel)
            info->_velocity = 1.0;
         else
            info->_velocity *= acceleration;

         dir.x *= info->_velocity;
         dir.y *= info->_velocity;

         POINT pos;
         pos.x = GET_X_LPARAM(lparam);
         pos.y = GET_Y_LPARAM(lparam);
         ScreenToClient(hwnd, &pos);

         float scale = get_scale_for_window(hwnd);
         info->_vptr->scroll(dir, {pos.x / scale, pos.y / scale});
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
            return view.text({codepoint, get_mods()});
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
               info->_vptr->click(get_button(hwnd, info, message, wparam, lparam));
               break;

            case WM_MOUSEMOVE:
               if (info->_is_dragging)
               {
                  info->_vptr->drag(get_button(hwnd, info, message, wparam, lparam));
               }
               else
               {
                  if (!info->_mouse_in_window)
                  {
                     on_cursor(hwnd, info->_vptr, lparam, cursor_tracking::entering);
                     info->_mouse_in_window = true;
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
               info->_mouse_in_window = false;
               on_cursor(hwnd, info->_vptr, lparam, cursor_tracking::leaving);
               break;

            case WM_MOUSEHOVER:
               on_cursor(hwnd, info->_vptr, lparam, cursor_tracking::hovering);
               break;

            case WM_MOUSEWHEEL:
               {
                  float delta = GET_WHEEL_DELTA_WPARAM(wparam);
                  on_scroll(hwnd, info, lparam, {0, delta / mouse_wheel_line_delta});
               }
               break;

            case WM_MOUSEHWHEEL:
               {
                  float delta = -GET_WHEEL_DELTA_WPARAM(wparam);
                  on_scroll(hwnd, info, lparam, {delta / mouse_wheel_line_delta, 0});
               }
               break;

            case WM_SETCURSOR:
               if (LOWORD(lparam) == HTCLIENT && current_cursor != GetCursor())
                  SetCursor(current_cursor);
               break;

            case WM_TIMER:
               if (wparam == IDT_TIMER1)
                  info->_vptr->poll();
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
               return on_text(*info->_vptr, message, wparam);

            case WM_SETFOCUS:
               info->_vptr->begin_focus();
               break;

            case WM_KILLFOCUS:
               info->_vptr->end_focus();
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
            windowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); ;
            windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
            windowClass.hInstance = GetModuleHandleW(nullptr);
            windowClass.lpfnWndProc = WndProc;
            windowClass.lpszClassName = L"ElementsView";
            windowClass.style = CS_HREDRAW | CS_VREDRAW;
            if (!RegisterClassW(&windowClass))
               MessageBoxW(nullptr, L"Could not register class", L"Error", MB_OK);

            auto pwd = fs::current_path();
            auto resource_path = pwd / "resources";
            artist::add_search_path(resource_path);
         }
      };
   }

   namespace
   {
      HWND make_window(base_view* _this, host_window_handle parent, RECT bounds)
      {
         static init_view_class init;
         auto style = WS_CHILD | WS_VISIBLE;

         HWND hwnd = CreateWindowW(
            L"ElementsView",
            nullptr,
            style,
            CW_USEDEFAULT, CW_USEDEFAULT, // position x, y
            1, 1,                         // width, height
            parent, nullptr,              // parent window, menu
            nullptr, nullptr              // instance, param
         );

         auto dpi = GetDpiForWindow(hwnd);
         AdjustWindowRectExForDpi(&bounds, style, false, WS_EX_APPWINDOW, dpi);

         MoveWindow(
            hwnd, bounds.left, bounds.top,
            bounds.right - bounds.left,
            bounds.bottom - bounds.top,
            false
         );

         view_info* info = new view_info{_this};
         SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(info));

         info->_skia_context = sk_app::window_context_factory::MakeGLForWin(hwnd, sk_app::DisplayParams());

         // Create 1ms timer
         SetTimer(hwnd, IDT_TIMER1, 1, (TIMERPROC) nullptr);

         return hwnd;
      }
   }

   base_view::base_view(extent size_)
   {
	   RECT bounds = {0, 0, LONG(size_.x), LONG(size_.y)};
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
      return {float(pos.x) / scale, float(pos.y) / scale};
   }

   elements::extent base_view::size() const
   {
      float scale = get_scale_for_window(_view);
      RECT r;
      GetWindowRect(_view, &r);
      return {float(r.right-r.left) / scale, float(r.bottom-r.top) / scale};
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

   float base_view::hdpi_scale() const
   {
      return get_scale_for_window(_view);
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

      std::wstring source{buffer, std::char_traits<WCHAR>::length(buffer)};

      GlobalUnlock(object);
      CloseClipboard();

      return utf8_encode(source);
   }

   void clipboard(std::string_view text)
   {
      auto len = MultiByteToWideChar(CP_UTF8, 0, text.data(), text.size(), nullptr, 0);
      if (!len)
         return;
      ++len; // null terminator

      HANDLE object = GlobalAlloc(GMEM_MOVEABLE, len * sizeof(WCHAR));
      if (!object)
         return;

      WCHAR* buffer = static_cast<WCHAR*>(GlobalLock(object));
      if (!buffer)
      {
         GlobalFree(object);
         return;
      }

      MultiByteToWideChar(CP_UTF8, 0, text.data(), text.size(), buffer, len);
      buffer[text.size()] = 0;
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
      return {+1.0f, +1.0f};
   }
}}

