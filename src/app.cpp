/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <GLFW/glfw3.h>
#include <photon/app.hpp>
#include <photon/window.hpp>
#include <photon/exception.hpp>

#include <map>
#include <chrono>
#include <thread>

namespace photon
{
   void on_error(int error, const char* desc)
   {
      throw glfw_exception(error, desc);
   }

   std::map<GLFWwindow*, window*> windows;
   app* app_ptr = 0; // singleton

   app::app()
   {
      if (!glfwInit())
         throw glfw_exception(-1, "Failed to init GLFW.");

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
      _cursors.clear();
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
      bool do_idle = false;
   
      std::thread idle_thread(
         [this, &do_idle]()
         {
            while (windows_open())
            {
               std::this_thread::sleep_for(idle_time);
               glfwPostEmptyEvent();
               do_idle = true;
            }
         }
      );
   
      while (windows_open())
      {
         glfwWaitEvents();
         if (do_idle)
         {
            do_idle = false;
            windows_idle();
         }
      }
      
      idle_thread.join();
   }

   void app::add_undo(undo_redo_task f)
   {
      undo_stack.push(f);
      if (has_redo())
      {
         // clear the redo stack
         undo_stack_type empty{};
         redo_stack.swap(empty);
      }
   }

   bool app::undo()
   {
      if (has_undo())
      {
         auto t = undo_stack.top();
         undo_stack.pop();
         redo_stack.push(t);
         t.undo();  // execute undo function
         return true;
      }
      return false;
   }

   bool app::redo()
   {
      if (has_redo())
      {
         auto t = redo_stack.top();
         undo_stack.push(t);
         redo_stack.pop();
         t.redo();  // execute redo function
         return true;
      }
      return false;
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
