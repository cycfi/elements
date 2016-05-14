/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/window.hpp>
#include <photon/app.hpp>
#include <photon/exception.hpp>

#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg_gl.h>
#include <map>
#include <string>

namespace photon
{
   // Internal API
   extern std::map<GLFWwindow*, window*> windows;

   // defined in app.cpp
   void key_press(GLFWwindow* window, int key, int scancode, int action, int mods);
   void text_entry(GLFWwindow* window, unsigned int codepoint, int mods);

   void window_refresh(GLFWwindow* window_ptr)
   {
      auto wp = static_cast<window*>(glfwGetWindowUserPointer(window_ptr));
      wp->draw();
   }

   void mouse_button(GLFWwindow* window_ptr, int button, int action, int mods)
   {
      struct mouse_button btn;
      btn.is_pressed = (action == GLFW_PRESS);
      switch (mods)
      {
         default:
         case GLFW_MOUSE_BUTTON_LEFT:
            btn.state = mouse_button::left;
            break;

         case GLFW_MOUSE_BUTTON_MIDDLE:
            btn.state = mouse_button::middle;
            break;

         case GLFW_MOUSE_BUTTON_RIGHT:
            btn.state = mouse_button::right;
            break;
      };

      auto wp = static_cast<window*>(glfwGetWindowUserPointer(window_ptr));
      wp->click(btn);
   }

   void cursor_position(GLFWwindow* window_ptr, double xpos, double ypos)
   {
      auto wp = static_cast<window*>(glfwGetWindowUserPointer(window_ptr));
      wp->mouse(point(xpos, ypos));
   }

   void window_focus(GLFWwindow* window_ptr, int is_focus)
   {
      auto wp = static_cast<window*>(glfwGetWindowUserPointer(window_ptr));
      wp->focus(is_focus);
   }

   void window_scroll(GLFWwindow* window_ptr, double x, double y)
   {
      auto wp = static_cast<window*>(glfwGetWindowUserPointer(window_ptr));
      wp->scroll(point{ float(x), float(y) });
   }

   window::window(
      char const*    title
    , point   size
    , color          bkd_color
    , class app&     app_
    , widget_ptr     subject
    , theme_ptr      theme
   )
    : _bkd_color(bkd_color)
    , _app(app_)
    , _subject(subject)
    , _canvas(0)
    , _theme(theme)
    , _click_time(0)
    , _num_clicks(0)
    , _refresh(false)
    , _current_key({ key_code::key_unknown, key_action::unknown, 0 })
   {
      _window = glfwCreateWindow(size.x, size.y, title, 0, 0);
      if (_window == 0)
      {
         glfwTerminate();
         throw glfw_exception(-1, "Failed to create window.");
      }

      glfwSetWindowUserPointer(_window, this);
      glfwSetKeyCallback(_window, key_press);
      glfwSetCharModsCallback(_window, text_entry);
      glfwSetWindowRefreshCallback(_window, window_refresh);
      glfwSetMouseButtonCallback(_window, mouse_button);
      glfwSetWindowFocusCallback(_window, window_focus);
      glfwSetCursorPosCallback(_window, cursor_position);
      glfwSetScrollCallback(_window, window_scroll);
      glfwMakeContextCurrent(_window);

      _context = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
      if (_context == 0)
         throw nanovg_exception("Could not init nanovg.");
      _canvas = { _context };

      glfwSwapInterval(0);
      glfwSetTime(0);

      _theme->canvas(_context);
      _theme->load_fonts();

      windows[_window] = this;
   }

   window::~window()
   {
      windows.erase(_window);
      glfwDestroyWindow(_window);
      nvgDeleteGL3(_context);
   }

   point window::size() const
   {
      int width, height;
      glfwGetWindowSize(_window, &width, &height);
      return { float(width), float(height) };
   }

   void window::size(point s)
   {
      glfwSetWindowSize(_window, s.x, s.y);
   }

   point window::cursor_pos() const
   {
      double mx, my;
      glfwGetCursorPos(_window, &mx, &my);
      return { float(mx), float(my) };
   }

   void window::draw()
   {
      int w_width, w_height;
      glfwGetWindowSize(_window, &w_width, &w_height);

      int fb_width, fb_height;
      glfwGetFramebufferSize(_window, &fb_width, &fb_height);

      // Update and render
      glViewport(0, 0, fb_width, fb_height);
      glClearColor(_bkd_color.red, _bkd_color.green, _bkd_color.blue, _bkd_color.alpha);
      glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

      // Calculate pixel ration for hi-dpi devices.
      float px_ratio = float(fb_width) / float(w_width);

      nvgBeginFrame(_context, w_width, w_height, px_ratio);
      {
         basic_context bctx{ *this };
         rect limits = _subject->limits(bctx);
         if (limits != _current_limits)
         {
            _current_limits = limits;
            glfwSetWindowSizeLimits(_window, limits.left, limits.top, limits.right, limits.bottom);

            int w = w_width;
            int h = w_height;
            limit(w, limits.left, limits.right);
            limit(h, limits.top, limits.bottom);

            if ((w_width != w) || (w_height != h))
            {
               glfwSetWindowSize(_window, w, h);
               return; // return early to refresh/redraw window
            }
         }

         rect subj_bounds = { 0, 0, float(w_width), float(w_height) };

         // layout the subject only if the window bounds changes
         context ctx{ *this, _subject.get(), subj_bounds };
         if (subj_bounds != _current_bounds)
         {
            _current_bounds = subj_bounds;
            _subject->layout(ctx);
         }

         // draw the subject
         _subject->draw(ctx);
      }
      nvgEndFrame(_context);
      glfwSwapBuffers(_window);
   }

   void window::key(key_info const& k)
   {
      _current_key = k;
      context ctx { *this, _subject.get(), _current_bounds };
      _subject->key(ctx, k);
   }

   void window::text(text_info const& info)
   {
      context ctx { *this, _subject.get(), _current_bounds };
      _subject->text(ctx, info);
   }

   void window::click(struct mouse_button const& btn_)
   {
      struct mouse_button btn = btn_;
      if (btn.is_pressed) // mouse down
      {
         double time = glfwGetTime();
         if (time - _click_time < _app.multi_click_speed)
            btn.num_clicks = ++_num_clicks;
         else
            btn.num_clicks = _num_clicks = 1;
         _click_time = time;
      }
      _btn = btn;
      context ctx { *this, _subject.get(), _current_bounds };
      _subject->click(ctx, btn);
   }

   void window::mouse(point p)
   {
      if (!_current_bounds.includes(p))
         return;

      context ctx { *this, _subject.get(), _current_bounds };
      if (_btn.is_pressed)
      {
         _subject->drag(ctx, _btn);
      }
      else
      {
         if (!_subject->cursor(ctx, cursor_pos()))
            reset_cursor();
         draw();
      }
   }

   void window::scroll(point p)
   {
      context ctx { *this, _subject.get(), _current_bounds };
      _subject->scroll(ctx, p);
   }

   void window::close()
   {
   }

   void window::set_cursor(cursor::type t) const
   {
      _app.get_cursor(t).set(_window);
   }

   void window::reset_cursor() const
   {
      glfwSetCursor(_window, 0);
   }

   bool window::is_focus() const
   {
      return glfwGetWindowAttrib(_window, GLFW_FOCUSED);
   }

   void window::focus(bool is_focus)
   {
      if (_subject->focus(focus_request::wants_focus))
         _subject->focus(is_focus? focus_request::begin_focus : focus_request::end_focus);
      draw();
   }

   void window::idle()
   {
      //if (_refresh)
      //   draw();
   }

   void window::refresh()
   {
      _refresh = true;
      glfwPostEmptyEvent();
   }

   std::string window::clipboard() const
   {
      return glfwGetClipboardString(_window);
   }

   void window::clipboard(std::string const& text) const
   {
      glfwSetClipboardString(_window, text.c_str());
   }
}
