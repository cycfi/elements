/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/window.hpp>
#include <photon/app.hpp>

#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg_gl.h>
#include <map>

namespace photon
{
   // Internal API
   extern std::map<GLFWwindow*, window*> windows;
   void key_press(GLFWwindow* window, int key, int scancode, int action, int mods);

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
      wp->cursor(point(xpos, ypos));
   }

   window::window(
      char const*    title
    , point const&   size
    , color const&   bkd_color
    , class app&     app_
    , widget_ptr     subject
    , theme_ptr      theme
   )
    : _bkd_color(bkd_color)
    , _app(app_)
    , _subject(subject)
    , _theme(theme)
   {
      _window = glfwCreateWindow(size.x, size.y, title, 0, 0);
      if (_window == 0)
      {
         glfwTerminate();
         return; // $$$ throw $$
      }

      glfwSetWindowUserPointer(_window, this);
      glfwSetKeyCallback(_window, key_press);
      glfwSetWindowRefreshCallback(_window, window_refresh);
      glfwSetMouseButtonCallback(_window, mouse_button);
      glfwSetCursorPosCallback(_window, cursor_position);
      glfwMakeContextCurrent(_window);

      _context = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
      if (_context == 0)
      {
         printf("Could not init nanovg.\n");
         return; // $$$ throw $$
      }

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
      return { double(width), double(height) };
   }

   void window::size(point const& s)
   {
      glfwSetWindowSize(_window, s.x, s.y);
   }

   point window::cursor_pos() const
   {
      double mx, my;
      glfwGetCursorPos(_window, &mx, &my);
      return { mx, my };
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

         rect subj_bounds = { 0, 0, double(w_width), double(w_height) };

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
   }

   void window::click(struct mouse_button btn)
   {
      _btn = btn;
      context ctx { *this, _subject.get(), _current_bounds };
      _subject->click(ctx, btn);
   }

   void window::cursor(point const& p)
   {
      if (_btn.is_pressed)
      {
         context ctx { *this, _subject.get(), _current_bounds };
         _subject->drag(ctx, _btn);
      }
   }

   void window::close()
   {
   }
}
