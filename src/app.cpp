/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <GLFW/glfw3.h>
#include <photon/app.hpp>
#include <photon/window.hpp>
#include <map>

namespace photon
{
   void on_error(int error, const char* desc)
   {
      // $$$ printf("GLFW error %d: %s\n", error, desc);
   }

   std::map<GLFWwindow*, window*> windows;
   app* app_ptr = 0; // singleton

   app::app()
    : _idle_time(0)
   {
      if (!glfwInit())
      {
         // $$$ printf("Failed to init GLFW.");
         return; // $$$ throw $$
      }

      glfwSetErrorCallback(on_error);

#ifndef _WIN32 // don't require this on win32, and works with more cards
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
      glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
      glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);

      _cursors.push_back(cursor{ cursor::arrow });
      _cursors.push_back(cursor{ cursor::ibeam });
      _cursors.push_back(cursor{ cursor::cross_hair });
      _cursors.push_back(cursor{ cursor::hand });
      _cursors.push_back(cursor{ cursor::h_resize });
      _cursors.push_back(cursor{ cursor::v_resize });
   }

   app::~app()
   {
      glfwTerminate();
   }

   cursor& app::get_cursor(cursor::type t)
   {
      return _cursors[t];
   }

   void app::key(key_info const& k)
   {
   }

   void app::text(text_info const& info)
   {
   }

   namespace
   {
      bool windows_open()
      {
         for (auto i = windows.cbegin(); i != windows.cend(); /***/)
         {
            if (glfwWindowShouldClose(i->first))
            {
               i->second->close();
               windows.erase(i++);
            }
            else
            {
               ++i;
            }
         }

         return !windows.empty();
      }

      void windows_idle()
      {
         for (auto i = windows.cbegin(); i != windows.cend(); ++i)
            i->second->idle();
      }
   }

   void app::run()
   {
      while (windows_open())
      {
         glfwWaitEvents();
         double time = glfwGetTime();
         if (time - _idle_time > idle_time)
         {
            windows_idle();
            _idle_time = time;
         }
      }
   }

   void text_entry(GLFWwindow* window_ptr, unsigned int codepoint, int mods)
   {
      if (app_ptr)
      {
         text_info info{ codepoint, mods };
         app_ptr->text(info);
         auto wp = static_cast<window*>(glfwGetWindowUserPointer(window_ptr));
         wp->text(info);
      }
   }

   void key_press(GLFWwindow* window_ptr, int key, int scancode, int action, int mods)
   {
      if (app_ptr)
      {
         key_info k = { key_code(key), key_action(action), mods };
         app_ptr->key(k);
         auto wp = static_cast<window*>(glfwGetWindowUserPointer(window_ptr));
         wp->key(k);
      }
   }
}
