/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_APP_APRIL_11_2016)
#define PHOTON_GUI_LIB_APP_APRIL_11_2016

#include <photon/support.hpp>
#include <photon/color.hpp>
#include <type_traits>

namespace photon
{
	////////////////////////////////////////////////////////////////////////////////////////////////
	// Theme
	////////////////////////////////////////////////////////////////////////////////////////////////
   struct app_theme
   {
      // Panels
      float panel_corner_radius = 3.0f;
      color panel_color = { 28, 30, 34, 192 };

      // Shadows
      color shadow_color = { 0, 0, 0, 0 };
   };

	////////////////////////////////////////////////////////////////////////////////////////////////
	// The Application
	////////////////////////////////////////////////////////////////////////////////////////////////
   class app
   {
   public:

      ~app();

      app(app const&) = delete;
      app& operator=(app const&) = delete;

      virtual void key(key_info const& k);
      virtual void run();

      template <typename App, typename... Args>
      friend App& make_app(Args const&... args);

      // The default theme
      app_theme theme = {};

   protected:

      app();
      bool is_running;
   };

   template <typename App, typename... Args>
   inline App& make_app(Args const&... args)
   {
      static_assert(std::is_base_of<App, app>::value, "Error. App is not a subclass of class app.");
      extern app* app_ptr;

      static App app_obj{ args... };
      app_ptr = &app_obj;
      return app_obj;
   }
}

#endif