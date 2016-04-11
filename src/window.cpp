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

   // $$$ testing $$$
   void drawWindow(NVGcontext* vg, float x, float y, float w, float h)
   {
      float cornerRadius = 3.0f;
      NVGpaint shadowPaint;

      nvgSave(vg);

      // Window
      nvgBeginPath(vg);
      nvgRoundedRect(vg, x,y, w,h, cornerRadius);
      nvgFillColor(vg, nvgRGBA(28,30,34,192));
      nvgFill(vg);

      // Drop shadow
      shadowPaint = nvgBoxGradient(vg, x,y+2, w,h, cornerRadius*2, 10, nvgRGBA(0,0,0,128), nvgRGBA(0,0,0,0));
      nvgBeginPath(vg);
      nvgRect(vg, x-10,y-10, w+20,h+30);
      nvgRoundedRect(vg, x,y, w,h, cornerRadius);
      nvgPathWinding(vg, NVG_HOLE);
      nvgFillPaint(vg, shadowPaint);
      nvgFill(vg);

      nvgRestore(vg);
   }

   window::window(
      char const* title
    , double width
    , double height
    , color bkd_color)
    : bkd_color(bkd_color)
   {
      window_ptr = glfwCreateWindow(width, height, title, 0, 0);
      if (window_ptr == 0)
      {
         glfwTerminate();
         return; // $$$ throw $$
      }

      glfwSetWindowUserPointer(window_ptr, this);
      glfwSetKeyCallback(window_ptr, key_press);
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

      int winWidth, winHeight;
      glfwGetWindowSize(window_ptr, &winWidth, &winHeight);

      int fbWidth, fbHeight;
      glfwGetFramebufferSize(window_ptr, &fbWidth, &fbHeight);

      // Update and render
      glViewport(0, 0, fbWidth, fbHeight);
      glClearColor(bkd_color.red, bkd_color.green, bkd_color.blue, bkd_color.alpha);
      glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

      // Calculate pixel ration for hi-dpi devices.
      float pxRatio = (float)fbWidth / (float)winWidth;

      nvgBeginFrame(vg_context_ptr, winWidth, winHeight, pxRatio);

      // $$$ testing $$$
      drawWindow(vg_context_ptr, 50, 50, 300, 400);

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
