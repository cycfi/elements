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
   class basic_button : public array_composite<2, deck_widget>
   {
   public:
   
      using base_type = array_composite<2, deck_widget>;

      basic_button(std::shared_ptr<widget> off, std::shared_ptr<widget> on)
       : _state(false)
      {
         elements[0] = off;
         elements[1] = on;
      }

      virtual widget*   click(context const& ctx, mouse_button btn);
      virtual void      drag(context const& ctx, mouse_button btn);
      virtual bool      is_control() const;

      bool              state() const;
      bool              state(bool new_state);

   private:

      bool              _state;
   };

   inline bool basic_button::state() const
   {
      return _state;
   }

   inline bool basic_button::state(bool new_state)
   {
      if (new_state != _state)
      {
         _state = new_state;
         deck_widget::select(_state);
         return true;
      }
      return false;
   }

   basic_button button(color const& body_color, std::string const& text);

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Basic Button Body
   class basic_botton_body : public widget
   {
   public:

      basic_botton_body(color const& body_color)
       : body_color(body_color)
      {}

      virtual void   draw(context const& ctx);

   private:

      color          body_color;
   };

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Toggle Button
   class basic_toggle_button : public basic_button
   {
   public:

      basic_toggle_button(std::shared_ptr<widget> off, std::shared_ptr<widget> on)
       : basic_button(off, on)
      {}

      virtual widget*   click(context const& ctx, mouse_button btn);
      virtual void      drag(context const& ctx, mouse_button btn);
   };

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Latching Button
   class basic_latching_button : public basic_button
   {
   public:

      basic_latching_button(std::shared_ptr<widget> off, std::shared_ptr<widget> on)
       : basic_button(off, on)
      {}

      virtual widget*   click(context const& ctx, mouse_button btn);
   };
}

#endif