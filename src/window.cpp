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
      char const* title
    , point const& size
    , color const& bkd_color
    , app& app_
    , widget_ptr subject
   )
    : bkd_color(bkd_color)
    , app_(app_)
    , subject(subject)
   {
      window_ptr = glfwCreateWindow(size.x, size.y, title, 0, 0);
      if (window_ptr == 0)
      {
         glfwTerminate();
         return; // $$$ throw $$
      }

      glfwSetWindowUserPointer(window_ptr, this);
      glfwSetKeyCallback(window_ptr, key_press);
      glfwSetWindowRefreshCallback(window_ptr, window_refresh);
      glfwMakeContextCurrent(window_ptr);

      vg_context_ptr = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
      if (vg_context_ptr == 0)
      {
         printf("Could not init nanovg.\n");
         return; // $$$ throw $$
      }

      glfwSwapInterval(0);
      glfwSetTime(0);

      windows[window_ptr] = this;
   }

   window::~window()
   {
      windows.erase(window_ptr);
      glfwDestroyWindow(window_ptr);
      nvgDeleteGL3(vg_context_ptr);
   }

   void window::draw()
   {
      double mx, my;
      glfwGetCursorPos(window_ptr, &mx, &my);

      int w_width, w_height;
      glfwGetWindowSize(window_ptr, &w_width, &w_height);

      int fb_width, fb_height;
      glfwGetFramebufferSize(window_ptr, &fb_width, &fb_height);

      // Update and render
      glViewport(0, 0, fb_width, fb_height);
      glClearColor(bkd_color.red, bkd_color.green, bkd_color.blue, bkd_color.alpha);
      glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

      // Calculate pixel ration for hi-dpi devices.
      float px_ratio = float(fb_width) / float(w_width);

      nvgBeginFrame(vg_context_ptr, w_width, w_height, px_ratio);

      rect subj_bounds = { 0, 0, double(w_height), double(w_width) };
      subject->draw(layout_info{ app_, *this, 0, subj_bounds });

      nvgEndFrame(vg_context_ptr);
      glfwSwapBuffers(window_ptr);
   }

   void window::key(key_info const& k)
   {
   }

   void window::close()
   {
   }
}
