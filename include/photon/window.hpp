/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WINDOW_APRIL_10_2016)
#define PHOTON_GUI_LIB_WINDOW_APRIL_10_2016

#include <stdio.h>
#ifdef __APPLE__
# define GLFW_INCLUDE_GLCOREARB
#endif

#include <GLFW/glfw3.h>
#include "nanovg.h"

#include <photon/widget/widget.hpp>
#include <photon/color.hpp>
#include <photon/point.hpp>

#include <memory>

namespace photon
{
   class app;

	////////////////////////////////////////////////////////////////////////////////////////////////
	// Windows
	////////////////////////////////////////////////////////////////////////////////////////////////
   class window
   {
   public:

      window(
         char const* title
       , point const& size
       , color const& bkd_color
       , app& app_
       , widget_ptr subject
      );

      ~window();

      window(window const&) = delete;
      window& operator=(window const&) = delete;

      virtual void draw();
      virtual void key(key_info const& k);
      virtual void close();

      NVGcontext* context() const { return vg_context_ptr; }

   private:

      color bkd_color;
      app& app_;
      widget_ptr subject;
      GLFWwindow* window_ptr;
      NVGcontext* vg_context_ptr;
   };
}

#endif