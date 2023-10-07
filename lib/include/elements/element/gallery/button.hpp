/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_GALLERY_BUTTON_JUNE_5_2016)
#define ELEMENTS_GALLERY_BUTTON_JUNE_5_2016

#include <elements/element/activator.hpp>
#include <elements/element/button.hpp>
#include <elements/element/misc.hpp>
#include <elements/element/label.hpp>
#include <elements/element/align.hpp>
#include <elements/element/margin.hpp>
#include <elements/element/tile.hpp>
#include <elements/support/theme.hpp>
#include <infra/support.hpp>
#include <infra/string_view.hpp>
#include <utility>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Buttons
   ////////////////////////////////////////////////////////////////////////////
   struct button_element : element, basic_receiver<button_state>
   {
      enum icon_placement { icon_none, icon_left, icon_right };

                              button_element(
                                 std::string text
                               , float size
                               , color body_color = get_theme().default_button_color
                               , std::uint32_t icon_code = 0
                               , icon_placement icon_placement_ = icon_none
                              )
                               : _body_color{body_color}
                               , _text{std::move(text)}
                               , _size{size}
                               , _icon_code{icon_code}
                               , _icon_placement{icon_placement_}
                              {}

      view_limits             limits(basic_context const& ctx) const override;
      bool                    cursor(context const& ctx, point p, cursor_tracking status) override;
      bool                    wants_control() const override;
      void                    draw(context const& ctx) override;

      color                   _body_color;
      std::string             _text;
      float                   _size;
      std::uint32_t           _icon_code;
      icon_placement          _icon_placement;
   };

   struct basic_button_body : public activator
   {
      constexpr static float corner_radius = 4.0;

                              basic_button_body(color body_color);
      void                    draw(context const& ctx) override;

      color                   body_color;
   };

   inline basic_button_body::basic_button_body(color body_color)
    : body_color(body_color)
   {}

   ////////////////////////////////////////////////////////////////////////////
   // Make a generic layered button
   ////////////////////////////////////////////////////////////////////////////
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

   ////////////////////////////////////////////////////////////////////////////
   // Make a layered button with text label
   ////////////////////////////////////////////////////////////////////////////
   template <typename Button>
   inline Button make_button(
      std::string text
    , float size = 1.0
    , color body_color = get_theme().default_button_color
   )
   {
      return make_button<Button>(
         margin(
            get_theme().button_margin,
            align_center(label(std::move(text)).relative_font_size(size))
         ),
         body_color
      );
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make a layered button with icon (left) and text (right) labels
   ////////////////////////////////////////////////////////////////////////////
   template <typename Button>
   inline Button make_button(
      std::uint32_t icon_code
    , std::string text
    , float size = 1.0
    , color body_color = get_theme().default_button_color
   )
   {
      return make_button<Button>(
         margin(
            get_theme().button_margin,
            align_center(
               htile(
                  right_margin(8, icon(icon_code, size)),
                  label(std::move(text)).relative_font_size(size)
               )
            )
         ),
         body_color
      );
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make a layered button with text (left) and icon (right) labels
   ////////////////////////////////////////////////////////////////////////////
   template <typename Button>
   inline Button make_button(
      std::string text
    , std::uint32_t icon_code
    , float size = 1.0
    , color body_color = get_theme().default_button_color
   )
   {
      return make_button<Button>(
         margin(
            get_theme().button_margin,
            align_center(
               htile(
                  label(std::move(text)).relative_font_size(size),
                  left_margin(12, icon(icon_code, size))
               )
            )
         ),
         body_color
      );
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make a generic layered text button with label and optional frame
   ////////////////////////////////////////////////////////////////////////////
   template <typename Button, typename Label>
   inline Button text_button(Label&& label, bool no_frame = false)
   {
      if (no_frame)
      {
         auto btn_img_off = label;
         auto btn_img_on = left_top_margin({1, 1}, label);
         return Button(std::move(btn_img_off), std::move(btn_img_on));
      }
      else
      {
         auto btn_img_off = layer(label, frame{});
         auto btn_img_on = left_top_margin({1, 1}, layer(label, frame{}));
         return Button(std::move(btn_img_off), std::move(btn_img_on));
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make a layered text button with text label and optional frame
   ////////////////////////////////////////////////////////////////////////////
   template <typename Button>
   inline Button text_button(
      std::string text
    , float size = 1.0
    , bool no_frame = false
   )
   {
      return text_button<Button>(
         margin(
            get_theme().button_margin,
            align_left(label(std::move(text)).relative_font_size(size))
         ),
         no_frame
      );
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make a layered text button with icon label and optional frame
   ////////////////////////////////////////////////////////////////////////////
   // template <typename Button>
   // inline Button text_button(
   //    std::uint32_t icon_code
   //  , float size = 1.0
   //  , bool no_frame = false
   // )
   // {
   //    return text_button<Button>(
   //       margin(
   //          get_theme().button_margin,
   //          align_center(icon(icon_code, size))
   //       ),
   //       no_frame
   //    );
   // }

   ////////////////////////////////////////////////////////////////////////////
   // Make a layered text button with icon (left) and text (right) labels
   // and optional frame
   ////////////////////////////////////////////////////////////////////////////
   template <typename Button>
   inline Button text_button(
      std::uint32_t icon_code
    , std::string text
    , float size = 1.0
    , bool no_frame = false
   )
   {
      return text_button<Button>(
         margin(
            get_theme().button_margin,
            htile(
               align_left(right_margin(8, icon(icon_code, size))),
               align_right(label(std::move(text)).relative_font_size(size))
            )
         ),
         no_frame
      );
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make a layered text button with text (left) and icon (right) labels
   // and optional frame
   ////////////////////////////////////////////////////////////////////////////
   template <typename Button>
   inline Button text_button(
      std::string text
    , std::uint32_t icon_code
    , float size = 1.0
    , bool no_frame = false
   )
   {
      return text_button<Button>(
         margin(
            get_theme().button_margin,
            htile(
               align_left(label(std::move(text)).relative_font_size(size)),
               align_right(left_margin(12, icon(icon_code, size)))
            )
         ),
         no_frame
      );
   }

   // ////////////////////////////////////////////////////////////////////////////
   // // Make a momentary button with icon label
   // ////////////////////////////////////////////////////////////////////////////
   // layered_button
   // button(
   //    std::uint32_t icon_code
   //  , float size = 1.0
   //  , color body_color = get_theme().default_button_color
   // );

   // ////////////////////////////////////////////////////////////////////////////
   // // Make a toggle button with icon label
   // ////////////////////////////////////////////////////////////////////////////
   // basic_toggle_button<>
   // toggle_button(
   //    std::uint32_t icon_code
   //  , float size = 1.0
   //  , color body_color = get_theme().default_button_color
   // );


   // ////////////////////////////////////////////////////////////////////////////
   // // Make a latching button with icon label
   // ////////////////////////////////////////////////////////////////////////////
   // basic_latching_button<>
   // latching_button(
   //    std::uint32_t icon_code
   //  , float size = 1.0
   //  , color body_color = get_theme().default_button_color
   // );

   ////////////////////////////////////////////////////////////////////////////
   // Basic buttons
   ////////////////////////////////////////////////////////////////////////////

   ////////////////////////////////////////////////////////////////////////////
   // Make a generic (non-layered) basic momentary button
   ////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   inline proxy<remove_cvref_t<Subject>, basic_button>
   momentary_button(Subject&& subject)
   {
      return { std::forward<Subject>(subject) };
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make a generic (non-layered) basic toggle button
   ////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   inline basic_toggle_button<proxy<remove_cvref_t<Subject>, basic_button>>
   toggle_button(Subject&& subject)
   {
      return { std::forward<Subject>(subject) };
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make a generic (non-layered) basic latching button
   ////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   inline basic_latching_button<proxy<remove_cvref_t<Subject>, basic_button>>
   latching_button(Subject&& subject)
   {
      return { std::forward<Subject>(subject) };
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make a momentary button with text label
   ////////////////////////////////////////////////////////////////////////////
   inline auto button(
      std::string text
    , float size = 1.0
    , color body_color = get_theme().default_button_color)
   {
      return momentary_button(button_element{text, size, body_color});
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make a momentary button with icon (left) and text (right) labels
   ////////////////////////////////////////////////////////////////////////////
   inline auto button(
      std::uint32_t icon_code
    , std::string text
    , float size = 1.0
    , color body_color = get_theme().default_button_color)
   {
      return momentary_button(
         button_element{text, size, body_color, icon_code, button_element::icon_left});
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make a momentary button with text (left) and icon (right) labels
   ////////////////////////////////////////////////////////////////////////////
   inline auto button(
      std::string text
    , std::uint32_t icon_code
    , float size
    , color body_color = get_theme().default_button_color)
   {
      return momentary_button(
         button_element{text, size, body_color, icon_code, button_element::icon_right});
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make a toggle button with text label
   ////////////////////////////////////////////////////////////////////////////
   inline auto toggle_button(
      std::string text
    , float size = 1.0
    , color body_color = get_theme().default_button_color)
   {
      return toggle_button(button_element{text, size, body_color});
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make a toggle button with icon (left) and text (right) labels
   ////////////////////////////////////////////////////////////////////////////
   inline auto toggle_button(
      std::uint32_t icon_code
    , std::string text
    , float size = 1.0
    , color body_color = get_theme().default_button_color)
   {
      return toggle_button(
         button_element{text, size, body_color, icon_code, button_element::icon_left});
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make a toggle button with text (left) and icon (right) labels
   ////////////////////////////////////////////////////////////////////////////
   inline auto toggle_button(
      std::string text
    , std::uint32_t icon_code
    , float size = 1.0
    , color body_color = get_theme().default_button_color)
   {
      return toggle_button(
         button_element{text, size, body_color, icon_code, button_element::icon_right});
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make a latching button with text label
   ////////////////////////////////////////////////////////////////////////////
   inline auto latching_button(
      std::string text
    , float size = 1.0
    , color body_color = get_theme().default_button_color)
   {
      return latching_button(button_element{text, size, body_color});
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make a latching button with icon (left) and text (right) labels
   ////////////////////////////////////////////////////////////////////////////
   inline auto latching_button(
      std::uint32_t icon_code
    , std::string text
    , float size = 1.0
    , color body_color = get_theme().default_button_color)
   {
      return latching_button(
         button_element{text, size, body_color, icon_code, button_element::icon_left});
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make a latching button with text (left) and icon (right) labels
   ////////////////////////////////////////////////////////////////////////////
   inline auto latching_button(
      std::string text
    , std::uint32_t icon_code
    , float size = 1.0
    , color body_color = get_theme().default_button_color)
   {
      return latching_button(
         button_element{text, size, body_color, icon_code, button_element::icon_right});
   }

   ////////////////////////////////////////////////////////////////////////////
   // Drawing button utility
   ////////////////////////////////////////////////////////////////////////////
   void draw_button_base(
      context const& ctx, rect bounds, color color_, bool enabled, float corner_radius
   );
}}

#endif
