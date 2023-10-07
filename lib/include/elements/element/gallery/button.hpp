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
   class button_element : public element, public basic_receiver<button_state>
   {
   public:

      enum icon_placement_enum { icon_none, icon_left, icon_right };

                              button_element(
                                      std::string text
                               , float size
                               , color body_color = get_theme().default_button_color
                               , std::uint32_t icon_code = 0
                               , icon_placement_enum icon_placement_ = icon_none
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

      void                    body_color(color body_color_);
      color                   body_color() const;
      void                    label(std::string text_);
      std::string const&      label() const;
      void                    icon(std::uint32_t icon_code);
      std::uint32_t           icon() const;
      void                    icon_placement(icon_placement_enum icon_placement_);
      icon_placement_enum     icon_placement() const;

   private:

      color                   _body_color;
      std::string             _text;
      float                   _size;
      std::uint32_t           _icon_code;
      icon_placement_enum     _icon_placement;
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
   // Inlines
   ////////////////////////////////////////////////////////////////////////////
   inline void button_element::body_color(color body_color_)
   {
      _body_color = body_color_;
   }

   inline color button_element::body_color() const
   {
      return _body_color;
   }

   inline void button_element::label(std::string text_)
   {
      std::swap(_text, text_);
   }

   inline std::string const& button_element::label() const
   {
      return _text;
   }

   inline void button_element::icon(std::uint32_t icon_code)
   {
      _icon_code = icon_code;
   }

   inline std::uint32_t button_element::icon() const
   {
      return _icon_code;
   }

   inline void button_element::icon_placement(icon_placement_enum icon_placement_)
   {
      _icon_placement = icon_placement_;
   }

   inline button_element::icon_placement_enum button_element::icon_placement() const
   {
      return _icon_placement;
   }

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
   // template <typename Button>
   // inline Button make_button(
   //    std::string text
   //  , float size = 1.0
   //  , color body_color = get_theme().default_button_color
   // )
   // {
   //    return make_button<Button>(
   //       margin(
   //          get_theme().button_margin,
   //          align_center(label(std::move(text)).relative_font_size(size))
   //       ),
   //       body_color
   //    );
   // }

   ////////////////////////////////////////////////////////////////////////////
   // Make a layered button with icon (left) and text (right) labels
   ////////////////////////////////////////////////////////////////////////////
   // template <typename Button>
   // inline Button make_button(
   //    std::uint32_t icon_code
   //  , std::string text
   //  , float size = 1.0
   //  , color body_color = get_theme().default_button_color
   // )
   // {
   //    return make_button<Button>(
   //       margin(
   //          get_theme().button_margin,
   //          align_center(
   //             htile(
   //                right_margin(8, icon(icon_code, size)),
   //                label(std::move(text)).relative_font_size(size)
   //             )
   //          )
   //       ),
   //       body_color
   //    );
   // }

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
   // Make a generic (non-layered) basic momentary button
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base = basic_button, typename Subject>
   inline proxy<remove_cvref_t<Subject>, Base>
   momentary_button(Subject&& subject)
   {
      return { std::forward<Subject>(subject) };
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make a generic (non-layered) basic toggle button
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base = basic_button, typename Subject>
   inline basic_toggle_button<proxy<remove_cvref_t<Subject>, Base>>
   toggle_button(Subject&& subject)
   {
      return { std::forward<Subject>(subject) };
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make a generic (non-layered) basic latching button
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base = basic_button, typename Subject>
   inline basic_latching_button<proxy<remove_cvref_t<Subject>, Base>>
   latching_button(Subject&& subject)
   {
      return { std::forward<Subject>(subject) };
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make a momentary button with text label
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base = basic_button>
   inline auto button(
      std::string text
    , float size = 1.0
    , color body_color = get_theme().default_button_color)
   {
      return momentary_button<Base>(
         button_element{std::move(text), size, body_color});
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make a momentary button with icon (left) and text (right) labels
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base = basic_button>
   inline auto button(
      std::uint32_t icon_code
    , std::string text
    , float size = 1.0
    , color body_color = get_theme().default_button_color)
   {
      return momentary_button<Base>(
         button_element{std::move(text), size, body_color
       , icon_code, button_element::icon_left}
      );
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make a momentary button with text (left) and icon (right) labels
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base = basic_button>
   inline auto button(
      std::string text
    , std::uint32_t icon_code
    , float size
    , color body_color = get_theme().default_button_color)
   {
      return momentary_button<Base>(
         button_element{std::move(text), size, body_color
       , icon_code, button_element::icon_right}
      );
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make a toggle button with text label
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base = basic_button>
   inline auto toggle_button(
      std::string text
    , float size = 1.0
    , color body_color = get_theme().default_button_color)
   {
      return toggle_button<Base>(
         button_element{std::move(text), size, body_color});
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make a toggle button with icon (left) and text (right) labels
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base = basic_button>
   inline auto toggle_button(
      std::uint32_t icon_code
    , std::string text
    , float size = 1.0
    , color body_color = get_theme().default_button_color)
   {
      return toggle_button<Base>(
         button_element{std::move(text), size, body_color
       , icon_code, button_element::icon_left}
      );
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make a toggle button with text (left) and icon (right) labels
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base = basic_button>
   inline auto toggle_button(
      std::string text
    , std::uint32_t icon_code
    , float size = 1.0
    , color body_color = get_theme().default_button_color)
   {
      return toggle_button<Base>(
         button_element{std::move(text), size, body_color
       , icon_code, button_element::icon_right}
      );
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make a latching button with text label
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base = basic_button>
   inline auto latching_button(
      std::string text
    , float size = 1.0
    , color body_color = get_theme().default_button_color)
   {
      return latching_button<Base>(
         button_element{std::move(text), size, body_color});
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make a latching button with icon (left) and text (right) labels
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base = basic_button>
   inline auto latching_button(
      std::uint32_t icon_code
    , std::string text
    , float size = 1.0
    , color body_color = get_theme().default_button_color)
   {
      return latching_button<Base>(
         button_element{std::move(text), size, body_color
       , icon_code, button_element::icon_left}
      );
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make a latching button with text (left) and icon (right) labels
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base = basic_button>
   inline auto latching_button(
      std::string text
    , std::uint32_t icon_code
    , float size = 1.0
    , color body_color = get_theme().default_button_color)
   {
      return latching_button<Base>(
         button_element{std::move(text), size, body_color
       , icon_code, button_element::icon_right}
      );
   }

   ////////////////////////////////////////////////////////////////////////////
   // Drawing button utility
   ////////////////////////////////////////////////////////////////////////////
   void draw_button_base(
      context const& ctx, rect bounds, color color_, bool enabled, float corner_radius
   );
}}

#endif
