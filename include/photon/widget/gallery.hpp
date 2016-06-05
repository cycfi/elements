/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_GALLERY_JUNE_5_2016)
#define PHOTON_GUI_LIB_WIDGET_GALLERY_JUNE_5_2016

#include <photon/widget.hpp>
#include <photon/widget/icon_ids.hpp>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Buttons
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class basic_button_body : public widget
   {
   public:

      static float corner_radius;

                     basic_button_body(color body_color);
      virtual void   draw(context const& ctx);

   private:

      color          body_color;
   };

   inline basic_button_body::basic_button_body(color body_color)
    : body_color(body_color)
   {}

   template <typename Button, typename Label>
   inline Button make_button(Label&& label, color body_color = color{ 0, 0, 0, 0 })
   {
      auto btn_body_off = basic_button_body(body_color.level(0.9));
      auto btn_body_on = basic_button_body(body_color.opacity(0.5));

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

   template <typename Button>
   inline Button make_button(
      std::string const& text
    , std::uint32_t icon_code
    , color body_color = color{ 0, 0, 0, 0 }
   )
   {
      return make_button<Button>(
         margin(
            { 15, 8, 15, 8 },
            align_center(
               htile(
                  heading(text),
                  left_margin(8, icon(icon_code))
               )
            )
         ),
         body_color
      );
   }

   basic_button
   button(
      std::string const& text
    , color body_color = color{ 0, 0, 0, 0 }
   );

   basic_button
   button(
      std::uint32_t icon_code
    , std::string const& text
    , color body_color = color{ 0, 0, 0, 0 }
   );

   basic_button
   button(
      std::string const& text
    , std::uint32_t icon_code
    , color body_color = color{ 0, 0, 0, 0 }
   );

   basic_toggle_button
   toggle_button(
      std::string const& text
    , color body_color = color{ 0, 0, 0, 0 }
   );

   basic_toggle_button
   toggle_button(
      std::uint32_t icon_code
    , std::string const& text
    , color body_color = color{ 0, 0, 0, 0 }
   );

   basic_toggle_button
   toggle_button(
      std::string const& text
    , std::uint32_t icon_code
    , color body_color = color{ 0, 0, 0, 0 }
   );

   basic_latching_button
   latching_button(
      std::string const& text
    , color body_color = color{ 0, 0, 0, 0 }
   );

   basic_latching_button
   latching_button(
      std::uint32_t icon_code
    , std::string const& text
    , color body_color = color{ 0, 0, 0, 0 }
   );

   basic_latching_button
   latching_button(
      std::string const& text
    , std::uint32_t icon_code
    , color body_color = color{ 0, 0, 0, 0 }
   );
   
   inline auto icon_text(std::uint32_t code, std::string const& text)
   {
      return
         xside_margin(
            { 15, 15 },
            align_left(
               layer(
                  icon(code),
                  left_margin(25, heading(text))
               )
            )
         )
      ;
   }

   inline basic_toggle_button check_box(std::string const& text)
   {
      return basic_toggle_button(
         icon_text(icons::squareo, text)
       , icon_text(icons::checksquareo, text)
      );
   }
   
   inline basic_latching_button radio_button(std::string const& text)
   {
      return basic_latching_button(
         icon_text(icons::circleo, text)
       , icon_text(icons::dotcircleo, text)
      );
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Menu Items
   ////////////////////////////////////////////////////////////////////////////////////////////////
   inline auto menu_item_text(std::string const& text)
   {
      return xside_margin({ 20, 20 },  align_left(heading(text)));
   }

   template <typename Subject>
   inline basic_menu_item<typename std::decay<Subject>::type>
   make_basic_menu_item(Subject&& subject)
   {
      return { std::forward<Subject>(subject) };
   }

   inline auto menu_item(std::string const& text)
   {
      return make_basic_menu_item(menu_item_text(text));
   }
}

#endif