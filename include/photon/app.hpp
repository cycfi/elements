/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_APP_APRIL_11_2016)
#define PHOTON_GUI_LIB_APP_APRIL_11_2016

#include <photon/support.hpp>
#include <photon/cursor.hpp>
#include <photon/key.hpp>

#include <type_traits>
#include <vector>
#include <stack>
#include <functional>
#include <chrono>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // The Application
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class app
   {
   public:
                           ~app();
                           app(app const&) = delete;
                           app& operator=(app const&) = delete;

      virtual void         key(key_info const& k);
      virtual void         text(text_info const& info);
      virtual void         run();
      cursor&              get_cursor(cursor::type t);
      
      duration             multi_click_speed = duration{ 0.5 };
      duration             idle_time = duration{ 0.25 };

      struct undo_redo_task
      {
         std::function<void()> undo;
         std::function<void()> redo;
      };

      void                 add_undo(undo_redo_task t);
      bool                 has_undo() { return !_undo_stack.empty(); }
      bool                 has_redo() { return !_redo_stack.empty(); }
      bool                 undo();
      bool                 redo();

   protected:
                           app();
   private:

      template <typename App, typename... Args>
      friend typename std::enable_if<
         std::is_base_of<App, app>::value, App&
      >::type
      make_app(Args const&... args);

      using undo_stack_type = std::stack<undo_redo_task>;

      std::vector<cursor>  _cursors;
      undo_stack_type      _undo_stack;
      undo_stack_type      _redo_stack;
   };

   template <typename App, typename... Args>
   inline typename std::enable_if<
         std::is_base_of<App, app>::value, App&
      >::type
   make_app(Args const&... args)
   {
      extern app* app_ptr;
      static App app_obj{ args... };
      app_ptr = &app_obj;
      return app_obj;
   }
}

#endif