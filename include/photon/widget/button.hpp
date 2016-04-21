/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_BUTTON_APRIL_21_2016)
#define PHOTON_GUI_LIB_WIDGET_BUTTON_APRIL_21_2016

#include <photon/widget/widget.hpp>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Buttons
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class button_widget : public proxy
   {
   public:

      button_widget(color const& color_, std::shared_ptr<widget> subject)
       : proxy(subject)
       , _color(color_)
       , _is_active(false)
      {}

      virtual void      draw(context const& ctx);
      virtual widget*   click(context const& ctx, mouse_button btn);
      virtual void      drag(context const& ctx, mouse_button btn);
      virtual bool      is_control() const;
   private:

      color             _color;
      bool              _is_active;
   };

   inline widget_ptr button(color const& color_, std::shared_ptr<widget> subject)
   {
      return widget_ptr{ new button_widget{ color_, subject } };
   }
}

#endif