/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#if !defined(ELEMENTS_WINDOW_MARCH_6_2019)
#define ELEMENTS_WINDOW_MARCH_6_2019

#include <string>
#include <functional>

#if defined(ELEMENTS_HOST_UI_LIBRARY_WIN32)
# include <Windows.h>
#endif

#include <infra/support.hpp>
#include <elements/support/rect.hpp>
#include <elements/base_view.hpp>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Window class
   ////////////////////////////////////////////////////////////////////////////
   class window : non_copyable
   {
   public:

      enum style
      {
         with_title     = 1
       , closable       = 2
       , miniaturizable = 4
       , resizable      = 8

       , bare           = 0
       , standard       = with_title | closable | miniaturizable | resizable
      };

                           window(
                              std::string const& name
                            , int style_ = standard
                            , rect const& bounds = rect{ 20, 20, 660, 500 }
                           );

                           window(
                              int style_ = standard
                            , rect const& bounds = rect{ 20, 20, 660, 500 }
                           )
                            : window("", style_, bounds)
                           {}

                           ~window();

      point                size() const;
      void                 size(point const& p);
      void                 limits(view_limits limits_);
      point                position() const;
      void                 position(point const& p);
      host_window_handle   host() const { return _window; }

      /////////////////////////////////////////////////////////////////////////
      // Notifications
      using callback_fn = std::function<void()>;

      callback_fn          on_close;

   private:

      host_window_handle   _window;
   };
}}

#endif

