/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_GALLERY_BUTTON_JUNE_5_2016)
#define ELEMENTS_GALLERY_BUTTON_JUNE_5_2016

#include <elements/element/button.hpp>
#include <elements/element/misc.hpp>
#include <elements/element/align.hpp>
#include <elements/element/margin.hpp>
#include <elements/element/tile.hpp>
#include <elements/support/theme.hpp>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Buttons
   ////////////////////////////////////////////////////////////////////////////
   auto constexpr button_margin = rect{ 10, 5, 10, 5 };

   struct basic_button_body : public element
   {
      constexpr static float corner_radius = 4.0;

                              basic_button_body(color body_color);
      virtual void            draw(context const& ctx);

      color                   body_color;
   };

   inline basic_button_body::basic_button_body(color body_color)
    : body_color(body_color)
   {}

   template <typename Button, typename Label>
   inline Button make_button(
      Label&& label
    , color body_color = get_theme().default_button_color)
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
    , float size = 1.0
    , color body_color = get_theme().default_button_color
   )
   {
      return make_button<Button>(
         margin(
            button_margin,
            align_center(label(text, size))
         ),
         body_color
      );
   }

   template <typename Button>
   inline Button make_button(
      std::uint32_t icon_code
    , float size = 1.0
    , color body_color = get_theme().default_button_color
   )
   {
      return make_button<Button>(
         margin(
            button_margin,
            align_center(
               icon(icon_code, size)
            )
         ),
         body_color
      );
   }

   template <typename Button>
   inline Button make_button(
      std::uint32_t icon_code
    , std::string const& text
    , float size = 1.0
    , color body_color = get_theme().default_button_color
   )
   {
      return make_button<Button>(
         margin(
            button_margin,
            align_center(
               htile(
                  right_margin(8, icon(icon_code, size)),
                  label(text, size)
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
    , float size = 1.0
    , color body_color = get_theme().default_button_color
   )
   {
      return make_button<Button>(
         margin(
            button_margin,
            align_center(
               htile(
                  label(text, size),
                  left_margin(12, icon(icon_code, size))
               )
            )
         ),
         body_color
      );
   }

   template <typename Button, typename Label>
   inline Button text_button(Label&& label)
   {
      auto btn_img_off = layer(label, frame{});
      auto btn_img_on = left_top_margin({1, 1}, layer(label, frame{}));

      return Button(std::move(btn_img_off), std::move(btn_img_on));
   }

   template <typename Button>
   inline Button text_button(
      std::string const& text
    , float size = 1.0
   )
   {
      return text_button<Button>(
         margin(
            button_margin,
            align_left(label(text, size))
         )
      );
   }

   template <typename Button>
   inline Button text_button(
      std::uint32_t icon_code
    , float size = 1.0
   )
   {
      return text_button<Button>(
         margin(
            button_margin,
            align_center(icon(icon_code, size))
         )
      );
   }

   template <typename Button>
   inline Button text_button(
      std::uint32_t icon_code
    , std::string const& text
    , float size = 1.0
   )
   {
      return text_button<Button>(
         margin(
            button_margin,
            htile(
               align_left(right_margin(8, icon(icon_code, size))),
               align_right(label(text, size))
            )
         )
      );
   }

   template <typename Button>
   inline Button text_button(
      std::string const& text
    , std::uint32_t icon_code
    , float size = 1.0
   )
   {
      return text_button<Button>(
         margin(
            button_margin,
            htile(
               align_left(label(text, size)),
               align_right(left_margin(12, icon(icon_code, size)))
            )
         )
      );
   }

   layered_button
   button(
      std::string const& text
    , float size = 1.0
    , color body_color = get_theme().default_button_color
   );

   layered_button
   button(
      std::uint32_t icon_code
    , float size = 1.0
    , color body_color = get_theme().default_button_color
   );

   layered_button
   button(
      std::uint32_t icon_code
    , std::string const& text
    , float size = 1.0
    , color body_color = get_theme().default_button_color
   );

   layered_button
   button(
      std::string const& text
    , std::uint32_t icon_code
    , float size
    , color body_color = get_theme().default_button_color
   );

   basic_toggle_button<>
   toggle_button(
      std::string const& text
    , float size = 1.0
    , color body_color = get_theme().default_button_color
   );

   basic_toggle_button<>
   toggle_button(
      std::uint32_t icon_code
    , float size = 1.0
    , color body_color = get_theme().default_button_color
   );

   basic_toggle_button<>
   toggle_button(
      std::uint32_t icon_code
    , std::string const& text
    , float size = 1.0
    , color body_color = get_theme().default_button_color
   );

   basic_toggle_button<>
   toggle_button(
      std::string const& text
    , std::uint32_t icon_code
    , float size = 1.0
    , color body_color = get_theme().default_button_color
   );

   basic_latching_button<>
   latching_button(
      std::string const& text
    , float size = 1.0
    , color body_color = get_theme().default_button_color
   );

   basic_latching_button<>
   latching_button(
      std::uint32_t icon_code
    , float size = 1.0
    , color body_color = get_theme().default_button_color
   );

   basic_latching_button<>
   latching_button(
      std::uint32_t icon_code
    , std::string const& text
    , float size = 1.0
    , color body_color = get_theme().default_button_color
   );

   basic_latching_button<>
   latching_button(
      std::string const& text
    , std::uint32_t icon_code
    , float size = 1.0
    , color body_color = get_theme().default_button_color
   );

   ////////////////////////////////////////////////////////////////////////////
   // Basic buttons
   ////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   inline basic_toggle_button<proxy<Subject, basic_button>>
   toggle_button(Subject&& subject)
   {
      return { std::forward<Subject>(subject) };
   }

   template <typename Subject>
   inline proxy<Subject, basic_button>
   momentary_button(Subject&& subject)
   {
      return { std::forward<Subject>(subject) };
   }

   ////////////////////////////////////////////////////////////////////////////
   // Drawing button utility
   ////////////////////////////////////////////////////////////////////////////
   void draw_button_base(context const& ctx, rect bounds, color color_, float corner_radius);
}}

#endif
