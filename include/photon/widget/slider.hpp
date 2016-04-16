/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_SLIDER_APRIL_16_2016)
#define PHOTON_GUI_LIB_WIDGET_SLIDER_APRIL_16_2016

#include <photon/widget/widget.hpp>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Sliders
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class slider_widget : public widget
   {
   public:

                     slider_widget(double pos) : pos(pos) {}

      virtual rect   limits() const;
      virtual void   draw(layout_info const& l);

   private:

      double         pos;
   };

   inline widget_ptr slider(double pos)
   {
      return widget_ptr{ new slider_widget{ pos } };
   }
}

#endif