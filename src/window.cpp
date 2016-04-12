/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/window.hpp>

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

   window::window(
      char const*    title
    , point const&   size
    , color const&   bkd_color
    , app&           app_
    , widget_ptr     subject
   )
    : _bkd_color(bkd_color)
    , _app(app_)
    , _subject(subject)
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
      glfwMakeContextCurrent(_window);

      _context = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
      if (_context == 0)
      {
         printf("Could not init nanovg.\n");
         return; // $$$ throw $$
      }

      glfwSwapInterval(0);
      glfwSetTime(0);

      rect limits = _subject->limits();
      glfwSetWindowSizeLimits(_window, limits.left, limits.top, limits.right, limits.bottom);

      windows[_window] = this;
   }

   window::~window()
   {
      windows.erase(_window);
      glfwDestroyWindow(_window);
      nvgDeleteGL3(_context);
   }

   void window::draw()
   {
      double mx, my;
      glfwGetCursorPos(_window, &mx, &my);

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

      rect subj_bounds = { 0, 0, double(w_height), double(w_width) };
      _subject->draw(layout_info{ _app, *this, 0, 0, subj_bounds });

      nvgEndFrame(_context);
      glfwSwapBuffers(_window);
   }

   void window::key(key_info const& k)
   {
   }

   void window::close()
   {
   }
}
