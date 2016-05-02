/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_BUTTON_APRIL_21_2016)
#define PHOTON_GUI_LIB_WIDGET_BUTTON_APRIL_21_2016

#include <photon/widget/layer.hpp>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Buttons
   ////////////////////////////////////////////////////////////////////////////////////////////////

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Momentary Button
   class button_widget : public deck_widget
   {
   public:

      button_widget(std::shared_ptr<widget> on, std::shared_ptr<widget> off)
       : _state(false)
      {
         deck_widget::elements().push_back(on);
         deck_widget::elements().push_back(off);
      }

      virtual widget*   click(context const& ctx, mouse_button btn);
      virtual void      drag(context const& ctx, mouse_button btn);
      virtual bool      is_control() const;

      bool              state() const;
      bool              state(bool new_state);

   private:

      bool              _state;
   };

   inline widget_ptr button(std::shared_ptr<widget> on, std::shared_ptr<widget> off)
   {
      return widget_ptr{ new button_widget{ on, off } };
   }

   inline bool button_widget::state() const
   {
      return _state;
   }

   inline bool button_widget::state(bool new_state)
   {
      if (new_state != _state)
      {
         _state = new_state;
         deck_widget::select(_state);
         return true;
      }
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Button Body
   class botton_body_widget : public widget
   {
   public:

      botton_body_widget(color const& body_color)
       : body_color(body_color)
      {}

      virtual void   draw(context const& ctx);

   private:

      color          body_color;
   };

   inline widget_ptr botton_body(color const& body_color)
   {
      return widget_ptr{ new botton_body_widget{ body_color } };
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Toggle Button
   class toggle_button_widget : public button_widget
   {
   public:

      toggle_button_widget(std::shared_ptr<widget> on, std::shared_ptr<widget> off)
       : button_widget(on, off)
      {}

      virtual widget*   click(context const& ctx, mouse_button btn);
      virtual void      drag(context const& ctx, mouse_button btn);
   };

   inline widget_ptr toggle_button(std::shared_ptr<widget> on, std::shared_ptr<widget> off)
   {
      return widget_ptr{ new toggle_button_widget{ on, off } };
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Latching Button
   class latching_button_widget : public button_widget
   {
   public:

      latching_button_widget(std::shared_ptr<widget> on, std::shared_ptr<widget> off)
       : button_widget(on, off)
      {}

      virtual widget*   click(context const& ctx, mouse_button btn);
   };

   inline widget_ptr latching_button(std::shared_ptr<widget> on, std::shared_ptr<widget> off)
   {
      return widget_ptr{ new latching_button_widget{ on, off } };
   }
}

#endif