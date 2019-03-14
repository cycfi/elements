/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#if !defined(CYCFI_PHOTON_GUI_LIB_WINDOW_MARCH_6_2019)
#define CYCFI_PHOTON_GUI_LIB_WINDOW_MARCH_6_2019

#include <string>
#include <functional>

#if defined(_WIN32)
# include <Windows.h>
#endif

#if defined(__linux__)
# include <gtk/gtk.h>
#endif

#include <infra/support.hpp>
#include <photon/support/rect.hpp>

namespace cycfi { namespace photon
{
   ////////////////////////////////////////////////////////////////////////////
   // Host window type

#if defined(__APPLE__)
   using host_window = void*;
#elif defined(_WIN32)
   using host_window = HWND;
#elif defined(__linux__)
   using host_window = GtkWidget*;
#endif

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
                           std::string const& name = ""
                         , int style_ = standard
                         , rect const& bounds = rect{ 20, 20, 660, 500 }
                        );

                        ~window();

      point             size() const;
      void              size(point const& p);
      point             position() const;
      void              position(point const& p);
      host_window       host() const { return _window; }

      /////////////////////////////////////////////////////////////////////////
      // Notifications
      using callback_fn = std::function<void()>;

      callback_fn       on_close;

   private:

      host_window       _window;
   };
}}

#endif

