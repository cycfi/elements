/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_PANEL_APRIL_11_2016)
#define PHOTON_GUI_LIB_WIDGET_PANEL_APRIL_11_2016

#include <photon/widget/widget.hpp>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Panels
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class panel_widget : public widget
   {
   public:

      virtual void draw(context const& ctx);
   };

   inline widget_ptr panel()
   {
      return widget_ptr{ new panel_widget{} };
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Frames
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class frame_widget : public widget
   {
   public:

      virtual void draw(context const& ctx);
   };

   inline widget_ptr frame()
   {
      return widget_ptr{ new frame_widget{} };
   }
}

#endif