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
   struct button_styler_base : element, basic_receiver<button_state>
   {
      bool                    cursor(context const& ctx, point p, cursor_tracking status) override;
      bool                    wants_control() const override;
   };

   struct default_button_styler : button_styler_base, text_reader
   {
   public:

      using base_type = default_button_styler;

      enum icon_placement {icon_none, icon_left, icon_right};
      enum label_alignment {align_left, align_center, align_right};

      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;

      virtual float           get_size() const;
      virtual color           get_body_color() const;
      virtual std::uint32_t   get_icon() const;
      virtual icon_placement  get_icon_placement() const;
      virtual label_alignment get_label_alignment() const;
   };

   template <typename Base>
   struct basic_button_styler_base : Base, text_writer
   {
      using text_type = std::string const&;
      using base_type = basic_button_styler_base<typename Base::base_type>;

                              basic_button_styler_base(std::string text)
                               : _text(std::move(text))
                              {}

      text_type               get_text() const override;
      void                    set_text(string_view text) override;

   private:

      std::string             _text;
   };

   template <typename Base>
   struct button_styler_with_size : Base
   {
      using base_type = button_styler_with_size<typename Base::base_type>;

                              button_styler_with_size(Base base, float size)
                               : Base(std::move(base)), _size(size)
                              {}

      float                   get_size() const override;
      void                    set_size(float size);

   private:

      float                   _size;
   };

   template <typename Base>
   struct button_styler_with_body_color : Base
   {
      using base_type = button_styler_with_body_color<typename Base::base_type>;

                              button_styler_with_body_color(Base base, color color_)
                               : Base(std::move(base)), _color(color_)
                              {}

      color                   get_body_color() const override;
      void                    set_body_color(color color_);

   private:

      color                   _color;
   };

   template <typename Base>
   struct button_styler_with_icon : Base
   {
      using icon_placement = default_button_styler::icon_placement;
      using base_type = button_styler_with_icon<typename Base::base_type>;

                              button_styler_with_icon(Base base, std::uint32_t code)
                               : Base(std::move(base)), _code(code)
                              {}

      std::uint32_t           get_icon() const override;
      void                    set_icon(std::uint32_t code);
      icon_placement          get_icon_placement() const override;

   private:

      std::uint32_t           _code;
      icon_placement          _icon_placement;
   };

   template <typename Base>
   struct button_styler_with_icon_placement : Base
   {
      using icon_placement = default_button_styler::icon_placement;
      using base_type = button_styler_with_icon_placement<typename Base::base_type>;

                              button_styler_with_icon_placement(Base base, icon_placement placement)
                               : Base(std::move(base)), _icon_placement(placement)
                              {}

      icon_placement          get_icon_placement() const override;
      void                    set_icon_placement(icon_placement placement);
      void                    icon_left();
      void                    icon_right();

   private:

      icon_placement          _icon_placement;
   };

   template <typename Base>
   struct button_styler_with_label_alignment : Base
   {
      using label_alignment = default_button_styler::label_alignment;
      using base_type = button_styler_with_label_alignment<typename Base::base_type>;

                              button_styler_with_label_alignment(Base base, label_alignment alignment)
                               : Base(std::move(base)), _label_alignment(alignment)
                              {}

      label_alignment         get_label_alignment() const override;
      void                    set_label_alignment(label_alignment alignment);
      void                    align_right();
      void                    align_left();

   private:

      label_alignment         _label_alignment;
   };

   template <typename Base>
   struct button_styler_gen : Base
   {
      using Base::Base;
      using base_type = typename Base::base_type;
      using icon_placement_t = default_button_styler::icon_placement;
      using label_alignment_t = default_button_styler::label_alignment;

      using gen_size             = button_styler_gen<button_styler_with_size<base_type>>;
      using gen_body_color       = button_styler_gen<button_styler_with_body_color<base_type>>;
      using gen_icon             = button_styler_gen<button_styler_with_icon<base_type>>;
      using gen_icon_placement   = button_styler_gen<button_styler_with_icon_placement<base_type>>;
      using gen_label_alignment  = button_styler_gen<button_styler_with_label_alignment<base_type>>;

      gen_size                size(float size) const;
      gen_body_color          body_color(color color_) const;
      gen_icon                icon(std::uint32_t code) const;
      gen_icon_placement      icon_left() const;
      gen_icon_placement      icon_right() const;
      gen_label_alignment     align_left() const;
      gen_label_alignment     align_center() const;
      gen_label_alignment     align_right() const;
   };

   using basic_button_styler = basic_button_styler_base<default_button_styler>;
   using button_styler = button_styler_gen<basic_button_styler>;

   ////////////////////////////////////////////////////////////////////////////
   // Make a generic basic momentary button
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base = basic_button, typename Subject>
   inline proxy<remove_cvref_t<Subject>, Base>
   momentary_button(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make a generic basic toggle button
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base = basic_button, typename Subject>
   inline basic_toggle_button<proxy<remove_cvref_t<Subject>, Base>>
   toggle_button(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }

   ////////////////////////////////////////////////////////////////////////////
   // Make a generic basic latching button
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base = basic_button, typename Subject>
   inline basic_latching_button<proxy<remove_cvref_t<Subject>, Base>>
   latching_button(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
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
      return momentary_button<Base>(
         button_styler{std::move(label)}
            .size(size)
            .body_color(body_color)
      );
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
      return momentary_button<Base>(
         button_styler{std::move(label)}
            .icon(icon_code)
            .icon_left()
            .size(size)
            .body_color(body_color)
      );
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
      return momentary_button<Base>(
         button_styler{std::move(label)}
            .icon(icon_code)
            .icon_right()
            .size(size)
            .body_color(body_color)
      );
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
      return toggle_button<Base>(
         button_styler{std::move(label)}
            .size(size)
            .body_color(body_color)
      );
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
      return momentary_button<Base>(
         button_styler{std::move(label)}
            .icon(icon_code)
            .icon_left()
            .size(size)
            .body_color(body_color)
      );
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
      return toggle_button<Base>(
         button_styler{std::move(label)}
            .icon(icon_code)
            .icon_right()
            .size(size)
            .body_color(body_color)
      );
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
      return latching_button<Base>(
         button_styler{std::move(label)}
            .size(size)
            .body_color(body_color)
      );
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
      return latching_button<Base>(
         button_styler{std::move(label)}
            .icon(icon_code)
            .icon_left()
            .size(size)
            .body_color(body_color)
      );
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
      return latching_button<Base>(
         button_styler{std::move(label)}
            .icon(icon_code)
            .icon_right()
            .size(size)
            .body_color(body_color)
      );
   }

   ////////////////////////////////////////////////////////////////////////////
   // Drawing button utility
   ////////////////////////////////////////////////////////////////////////////
   void draw_button_base(
      context const& ctx, rect bounds, color color_, bool enabled, float corner_radius
   );

   ////////////////////////////////////////////////////////////////////////////
   // Inlines
   ////////////////////////////////////////////////////////////////////////////
   inline float default_button_styler::get_size() const
   {
      return 1.0f;
   }

   inline color default_button_styler::get_body_color() const
   {
      return get_theme().default_button_color;
   }

   inline std::uint32_t default_button_styler::get_icon() const
   {
      return 0;
   }

   inline default_button_styler::icon_placement default_button_styler::get_icon_placement() const
   {
      return icon_none;
   }

   inline default_button_styler::label_alignment default_button_styler::get_label_alignment() const
   {
      return align_center;
   }

   template <typename Base>
   inline std::string const& basic_button_styler_base<Base>::get_text() const
   {
      return _text;
   }

   template <typename Base>
   inline void basic_button_styler_base<Base>::set_text(string_view text)
   {
      _text = std::string(text);
   }

   template <typename Base>
   inline float button_styler_with_size<Base>::get_size() const
   {
      return _size;
   }

   template <typename Base>
   inline void button_styler_with_size<Base>::set_size(float size)
   {
      _size = size;
   }

   template <typename Base>
   inline color button_styler_with_body_color<Base>::get_body_color() const
   {
      return _color;
   }

   template <typename Base>
   inline void button_styler_with_body_color<Base>::set_body_color(color color_)
   {
      _color = color_;
   }

   template <typename Base>
   inline std::uint32_t button_styler_with_icon<Base>::get_icon() const
   {
      return _code;
   }

   template <typename Base>
   inline void button_styler_with_icon<Base>:: set_icon(std::uint32_t code)
   {
      _code = code;
   }

   template <typename Base>
   inline button_styler::icon_placement
   button_styler_with_icon<Base>::get_icon_placement() const
   {
      return icon_placement::icon_right;
   }

   template <typename Base>
   inline default_button_styler::icon_placement
   button_styler_with_icon_placement<Base>::get_icon_placement() const
   {
      return _icon_placement;
   }

   template <typename Base>
   inline void button_styler_with_icon_placement<Base>::set_icon_placement(icon_placement placement)
   {
      _icon_placement = placement;
   }

   template <typename Base>
   inline void button_styler_with_icon_placement<Base>::icon_left()
   {
      _icon_placement = icon_placement::icon_left;
   }

   template <typename Base>
   inline void button_styler_with_icon_placement<Base>::icon_right()
   {
      _icon_placement = icon_placement::icon_right;
   }

   template <typename Base>
   inline default_button_styler::label_alignment
   button_styler_with_label_alignment<Base>::get_label_alignment() const
   {
      return _label_alignment;
   }

   template <typename Base>
   inline void button_styler_with_label_alignment<Base>::set_label_alignment(label_alignment alignment)
   {
      _label_alignment = alignment;
   }

   template <typename Base>
   inline void button_styler_with_label_alignment<Base>::align_right()
   {
      _label_alignment = label_alignment::align_right;
   }

   template <typename Base>
   inline void button_styler_with_label_alignment<Base>::align_left()
   {
      _label_alignment = label_alignment::align_left;
   }

   template <typename Base>
   inline typename button_styler_gen<Base>::gen_size
   button_styler_gen<Base>::size(float size) const
   {
      return {*this, size};
   }

   template <typename Base>
   inline typename button_styler_gen<Base>::gen_body_color
   button_styler_gen<Base>::body_color(color color_) const
   {
      return {*this, color_};
   }

   template <typename Base>
   inline typename button_styler_gen<Base>::gen_icon
   button_styler_gen<Base>::icon(std::uint32_t code) const
   {
      return {*this, code};
   }

   template <typename Base>
   inline typename button_styler_gen<Base>::gen_icon_placement
   button_styler_gen<Base>::icon_left() const
   {
      return {*this, icon_placement_t::icon_left};
   }

   template <typename Base>
   inline typename button_styler_gen<Base>::gen_icon_placement
   button_styler_gen<Base>::icon_right() const
   {
      return {*this, icon_placement_t::icon_right};
   }

   template <typename Base>
   inline typename button_styler_gen<Base>::gen_label_alignment
   button_styler_gen<Base>::align_left() const
   {
      return {*this, label_alignment_t::align_left};
   }

   template <typename Base>
   inline typename button_styler_gen<Base>::gen_label_alignment
   button_styler_gen<Base>::align_center() const
   {
      return {*this, label_alignment_t::align_center};
   }

   template <typename Base>
   inline typename button_styler_gen<Base>::gen_label_alignment
   button_styler_gen<Base>::align_right() const
   {
      return {*this, label_alignment_t::align_right};
   }
}}

#endif
