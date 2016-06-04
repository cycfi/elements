/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_BUTTON_APRIL_21_2016)
#define PHOTON_GUI_LIB_WIDGET_BUTTON_APRIL_21_2016

#include <photon/widget/layer.hpp>
#include <photon/widget/align.hpp>
#include <photon/widget/text.hpp>
#include <photon/widget/icon.hpp>
#include <photon/widget/tile.hpp>
#include <photon/widget/margin.hpp>
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

                        template <typename W1, typename W2>
                        basic_button(W1&& off, W2&& on);

      virtual widget*   hit_test(context const& ctx, point p);
      virtual widget*   click(context const& ctx, mouse_button btn);
      virtual void      drag(context const& ctx, mouse_button btn);
      virtual bool      is_control() const;

      bool              state() const;
      bool              state(bool new_state);

      button_function   on_click;

   private:

      bool              _state;
   };

   template <typename W1, typename W2>
   inline basic_button::basic_button(W1&& off, W2&& on)
    : _state(false)
   {
      elements[0] = new_(std::forward<W1>(off));
      elements[1] = new_(std::forward<W2>(on));
   }

   basic_button button(
      std::string const& text, color body_color = color{ 0, 0, 0, 0 }
   );

   basic_button button(
      std::uint32_t icon_code, std::string const& text, color body_color = color{ 0, 0, 0, 0 }
   );

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Basic Button Body
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class basic_botton_body : public widget
   {
   public:

      static float corner_radius;

                     basic_botton_body(color body_color);
      virtual void   draw(context const& ctx);

   private:

      color          body_color;
   };

   inline basic_botton_body::basic_botton_body(color body_color)
    : body_color(body_color)
   {}

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // make_button
   ////////////////////////////////////////////////////////////////////////////////////////////////
   template <typename Button, typename Label>
   inline Button make_button(Label&& label, color body_color = color{ 0, 0, 0, 0 })
   {
      auto btn_body_off = basic_botton_body(body_color.level(0.9));
      auto btn_body_on = basic_botton_body(body_color.opacity(0.5));

      auto btn_img_off = layer(label, btn_body_off);
      auto btn_img_on = left_top_margin({1, 1}, layer(label, btn_body_on));

      return Button(std::move(btn_img_off), std::move(btn_img_on));
   }

   template <typename Button>
   inline Button make_button(
      std::string const& text
    , color body_color = color{ 0, 0, 0, 0 }
   )
   {
      return make_button<Button>(
         margin(
            { 15, 8, 15, 8 },
            align_center(heading(text))
         ),
         body_color
      );
   }

   template <typename Button>
   inline Button make_button(
      std::uint32_t icon_code
    , std::string const& text
    , color body_color = color{ 0, 0, 0, 0 }
   )
   {
      return make_button<Button>(
         margin(
            { 15, 8, 15, 8 },
            align_center(
               htile(
                  right_margin(8, icon(icon_code)),
                  heading(text)
               )
            )
         ),
         body_color
      );
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
   };

   basic_toggle_button
   toggle_button(
      std::string const& text, color body_color = color{ 0, 0, 0, 0 }
   );

   basic_toggle_button
   toggle_button(
      std::uint32_t icon_code, std::string const& text, color body_color = color{ 0, 0, 0, 0 }
   );

   template <typename W1, typename W2>
   inline basic_toggle_button::basic_toggle_button(W1&& off, W2&& on)
    : basic_button(std::forward<W1>(off), std::forward<W2>(on))
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

   basic_latching_button
   latching_button(std::string const& text, color body_color = color{ 0, 0, 0, 0 });

   template <typename W1, typename W2>
   inline basic_latching_button::basic_latching_button(W1&& off, W2&& on)
    : basic_button(std::forward<W1>(off), std::forward<W2>(on))
   {}
}

#endif