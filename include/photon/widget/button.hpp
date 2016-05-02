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

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Momentary Button
   class button_widget : public proxy
   {
   public:

      button_widget(color const& color_, std::shared_ptr<widget> subject)
       : proxy(subject)
       , _color(color_)
       , _state(false)
      {}

      virtual void      draw(context const& ctx);
      virtual widget*   click(context const& ctx, mouse_button btn);
      virtual void      drag(context const& ctx, mouse_button btn);
      virtual bool      is_control() const;

      bool              state() const { return _state; }
      void              state(bool active) { _state = active; }

   private:

      color             _color;
      bool              _state;
   };

   inline widget_ptr button(color const& color_, std::shared_ptr<widget> subject)
   {
      return widget_ptr{ new button_widget{ color_, subject } };
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Toggle Button
   class toggle_button_widget : public button_widget
   {
   public:

      toggle_button_widget(color const& color, std::shared_ptr<widget> subject)
       : button_widget(color, subject)
      {}

      virtual widget*   click(context const& ctx, mouse_button btn);
      virtual void      drag(context const& ctx, mouse_button btn);
   };

   inline widget_ptr toggle_button(color const& color_, std::shared_ptr<widget> subject)
   {
      return widget_ptr{ new toggle_button_widget{ color_, subject } };
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Latching Button
   class latching_button_widget : public button_widget
   {
   public:

      latching_button_widget(color const& color, std::shared_ptr<widget> subject)
       : button_widget(color, subject)
      {}

      virtual widget*   click(context const& ctx, mouse_button btn);
   };

   inline widget_ptr latching_button(color const& color_, std::shared_ptr<widget> subject)
   {
      return widget_ptr{ new latching_button_widget{ color_, subject } };
   }
}

#endif