/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_BASIC_APRIL_11_2016)
#define PHOTON_GUI_LIB_WIDGET_BASIC_APRIL_11_2016

#include <photon/widget/widget.hpp>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Panels
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class panel : public widget
   {
   public:

      static float   corner_radius;
      static rect    shadow_offset;

      virtual void draw(context const& ctx);
   };

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Frames
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class frame : public widget
   {
   public:

      virtual void draw(context const& ctx);
   };

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Title Bars
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class title_bar : public widget
   {
   public:

      virtual void draw(context const& ctx);
   };
}

#endif