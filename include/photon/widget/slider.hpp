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

                        slider_widget() : pos(0.5) {}

      virtual rect      limits() const;
      virtual void      draw(layout_info const& l);
      virtual widget*   click(layout_info const& l, mouse_button btn);
      virtual void      drag(layout_info const& l, mouse_button btn);

   private:

      void              reposition(layout_info const& l);
      double            pos;
   };

   inline widget_ptr slider()
   {
      return widget_ptr{ new slider_widget{} };
   }
}

#endif