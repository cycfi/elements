/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_GALLERY_BUTTON_JUNE_5_2016)
#define ELEMENTS_GALLERY_BUTTON_JUNE_5_2016

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
   struct button_config
   {
      enum icon_placement_enum { icon_none, icon_left, icon_right };
      enum label_align_enum { align_left, align_center, align_right };

      std::string          label;
      float                size = 1.0f;
      color                body_color = get_theme().default_button_color;
      std::uint32_t        icon_code = 0;
      icon_placement_enum  icon_placement = icon_none;
      label_align_enum     align = align_center;
   };

   struct button_element_base : element, basic_receiver<button_state>
   {
      bool                    cursor(context const& ctx, point p, cursor_tracking status) override;
      bool                    wants_control() const override;

   };

   class button_element : public button_element_base
   {
   public:

      using icon_placement_enum = button_config::icon_placement_enum;
      static constexpr icon_placement_enum icon_none = button_config::icon_none;
      static constexpr icon_placement_enum icon_left = button_config::icon_left;
      static constexpr icon_placement_enum icon_right = button_config::icon_right;

      using label_align_enum = button_config::label_align_enum;
      static constexpr label_align_enum align_left = button_config::align_left;
      static constexpr label_align_enum align_center = button_config::align_center;
      static constexpr label_align_enum align_right = button_config::align_right;

                              template <typename Config>
                              button_element(Config const& cfg)
                               : _label{std::move(cfg.label)}
                               , _size{cfg.size}
                               , _body_color{cfg.body_color}
                               , _icon_code{cfg.icon_code}
                               , _icon_placement{cfg.icon_placement}
                               , _align{cfg.align}
                              {}

      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;

      void                    label(std::string text_);
      std::string const&      label() const;
      void                    size(float size_);
      float                   size() const;
      void                    body_color(color body_color_);
      color                   body_color() const;
      void                    icon(std::uint32_t icon_code);
      std::uint32_t           icon() const;
      void                    icon_placement(icon_placement_enum icon_placement_);
      icon_placement_enum     icon_placement() const;
      void                    align(label_align_enum align_);
      label_align_enum        align() const;

   private:

      std::string             _label;
      float                   _size;
      color                   _body_color;
      std::uint32_t           _icon_code;
      icon_placement_enum     _icon_placement;
      label_align_enum        _align;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Inlines
   ////////////////////////////////////////////////////////////////////////////
   inline void button_element::label(std::string text_)
   {
      std::swap(_label, text_);
   }

   inline std::string const& button_element::label() const
   {
      return _label;
   }

   inline void button_element::size(float size_)
   {
      _size = size_;
   }

   inline float button_element::size() const
   {
      return _size;
   }

   inline void button_element::body_color(color body_color_)
   {
      _body_color = body_color_;
   }

   inline color button_element::body_color() const
   {
      return _body_color;
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

   inline void button_element::align(label_align_enum align_)
   {
      _align = align_;
   }

   inline button_element::label_align_enum button_element::align() const
   {
      return _align;
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make a generic basic momentary button
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base = basic_button, typename Subject>
   inline proxy<remove_cvref_t<Subject>, Base>
   momentary_button(Subject&& subject)
   {
      return { std::forward<Subject>(subject) };
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make a generic basic toggle button
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base = basic_button, typename Subject>
   inline basic_toggle_button<proxy<remove_cvref_t<Subject>, Base>>
   toggle_button(Subject&& subject)
   {
      return { std::forward<Subject>(subject) };
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make a generic basic latching button
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base = basic_button, typename Subject>
   inline basic_latching_button<proxy<remove_cvref_t<Subject>, Base>>
   latching_button(Subject&& subject)
   {
      return { std::forward<Subject>(subject) };
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make a momentary button using button_config
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base = basic_button>
   inline auto button(button_config const& cfg)
   {
      return momentary_button<Base>(button_element{cfg});
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make a momentary button with label
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base = basic_button>
   inline auto button(
      std::string label
    , float size = 1.0
    , color body_color = get_theme().default_button_color)
   {
      auto cfg = button_config{std::move(label), size, body_color};
      return momentary_button<Base>(button_element{cfg});
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make a momentary button with icon (left) and label (right)
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base = basic_button>
   inline auto button(
      std::uint32_t icon_code
    , std::string label
    , float size
    , color body_color = get_theme().default_button_color)
   {
      auto cfg = button_config{
         std::move(label), size, body_color
       , icon_code, button_config::icon_left};
      return momentary_button<Base>(button_element{cfg});
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make a momentary button with label (left) and icon (right)
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base = basic_button>
   inline auto button(
      std::string label
    , std::uint32_t icon_code
    , float size
    , color body_color = get_theme().default_button_color)
   {
      auto cfg = button_config{
         std::move(label), size, body_color
       , icon_code, button_config::icon_right};
      return momentary_button<Base>(button_element{cfg});
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make a toggle button using button_config
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base = basic_button>
   inline auto toggle_button(button_config const& cfg)
   {
      return toggle_button<Base>(button_element{cfg});
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make a toggle button with label
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base = basic_button>
   inline auto toggle_button(
      std::string label
    , float size = 1.0
    , color body_color = get_theme().default_button_color)
   {
      auto cfg = button_config{std::move(label), size, body_color};
      return toggle_button<Base>(button_element{cfg});
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make a toggle button with icon (left) and label (right)
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base = basic_button>
   inline auto toggle_button(
      std::uint32_t icon_code
    , std::string label
    , float size
    , color body_color = get_theme().default_button_color)
   {
      auto cfg = button_config{
         std::move(label), size, body_color
       , icon_code, button_config::icon_left};
      return toggle_button<Base>(button_element{cfg});
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make a toggle button with label (left) and icon (right)
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base = basic_button>
   inline auto toggle_button(
      std::string label
    , std::uint32_t icon_code
    , float size
    , color body_color = get_theme().default_button_color)
   {
      auto cfg = button_config{
         std::move(label), size, body_color
       , icon_code, button_config::icon_right};
      return toggle_button<Base>(button_element{cfg});
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make a latching button using button_config
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base = basic_button>
   inline auto latching_button(button_config const& cfg)
   {
      return latching_button<Base>(button_element{cfg});
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make a latching button with label
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base = basic_button>
   inline auto latching_button(
      std::string label
    , float size = 1.0
    , color body_color = get_theme().default_button_color)
   {
      auto cfg = button_config{std::move(label), size, body_color};
      return latching_button<Base>(button_element{cfg});
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make a latching button with icon (left) and label (right)
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base = basic_button>
   inline auto latching_button(
      std::uint32_t icon_code
    , std::string label
    , float size
    , color body_color = get_theme().default_button_color)
   {
      auto cfg = button_config{
         std::move(label), size, body_color
       , icon_code, button_config::icon_left};
      return latching_button<Base>(button_element{cfg});
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make a latching button with label (left) and icon (right)
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base = basic_button>
   inline auto latching_button(
      std::string label
    , std::uint32_t icon_code
    , float size
    , color body_color = get_theme().default_button_color)
   {
      auto cfg = button_config{
         std::move(label), size, body_color
       , icon_code, button_config::icon_right};
      return latching_button<Base>(button_element{cfg});
   }

   ////////////////////////////////////////////////////////////////////////////
   // Drawing button utility
   ////////////////////////////////////////////////////////////////////////////
   void draw_button_base(
      context const& ctx, rect bounds, color color_, bool enabled, float corner_radius
   );
}}

#endif
