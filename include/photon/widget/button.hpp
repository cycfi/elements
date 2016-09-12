/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_BUTTON_APRIL_21_2016)
#define PHOTON_GUI_LIB_WIDGET_BUTTON_APRIL_21_2016

#include <photon/widget/layer.hpp>
#include <functional>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Momentary Button
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class basic_button : public array_composite<2, deck_widget>
   {
   public:

      using base_type = array_composite<2, deck_widget>;
      using button_function = std::function<void(bool)>;
      using base_type::value;

                        template <typename W1, typename W2>
                        basic_button(W1&& off, W2&& on);

      virtual widget*   hit_test(context const& ctx, point p);
      virtual widget*   click(context const& ctx, mouse_button btn);
      virtual void      drag(context const& ctx, mouse_button btn);
      virtual bool      is_control() const;

      virtual void      value(bool new_state);
      bool              value() const;

      button_function   on_click;

   protected:

      bool              state(bool new_state);

   private:

      bool              _state;
   };

   template <typename W1, typename W2>
   inline basic_button::basic_button(W1&& off, W2&& on)
    : _state(false)
   {
      (*this)[0] = share(std::forward<W1>(off));
      (*this)[1] = share(std::forward<W2>(on));
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Toggle Button
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class basic_toggle_button : public basic_button
   {
   public:
                        template <typename W1, typename W2>
                        basic_toggle_button(W1&& off, W2&& on);

      virtual widget*   click(context const& ctx, mouse_button btn);
      virtual void      drag(context const& ctx, mouse_button btn);

   private:

      bool              _current_state;
   };

   template <typename W1, typename W2>
   inline basic_toggle_button::basic_toggle_button(W1&& off, W2&& on)
    : basic_button(std::forward<W1>(off), std::forward<W2>(on))
    , _current_state(false)
   {}

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Latching Button
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class basic_latching_button : public basic_button
   {
   public:
                        template <typename W1, typename W2>
                        basic_latching_button(W1&& off, W2&& on);

      virtual widget*   click(context const& ctx, mouse_button btn);
   };

   template <typename W1, typename W2>
   inline basic_latching_button::basic_latching_button(W1&& off, W2&& on)
    : basic_button(std::forward<W1>(off), std::forward<W2>(on))
   {}
}

#endif