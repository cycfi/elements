/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_STYLE_BUTTON_JUNE_5_2016)
#define ELEMENTS_STYLE_BUTTON_JUNE_5_2016

#include <elements/element/button.hpp>
#include <elements/element/label.hpp>
#include <elements/element/align.hpp>
#include <elements/element/margin.hpp>
#include <elements/element/tile.hpp>
#include <elements/support/theme.hpp>
#include <infra/support.hpp>
#include <infra/string_view.hpp>
#include <utility>

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Buttons
   ////////////////////////////////////////////////////////////////////////////
   struct button_styler_base : element, basic_receiver<button_state>
   {
      bool                    cursor(context const& ctx, point p, cursor_tracking status) override;
      bool                    wants_control() const override;
   };

   struct default_button_styler : button_styler_base, text_reader_u8
   {
   public:

      using base_type = default_button_styler;
      constexpr static bool has_default_corner_radius = true;

      enum icon_placement {icon_none, icon_left, icon_right};
      enum label_alignment {align_left, align_center, align_right};
      enum direction {up, left, down, right};
      enum corner {top_left, top_right, bottom_right, bottom_left};

      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;

      virtual float           get_size() const;
      virtual color           get_body_color() const;
      virtual color           get_active_body_color() const;
      virtual color           get_text_color() const;
      virtual std::uint32_t   get_icon() const;
      virtual icon_placement  get_icon_placement() const;
      virtual color           get_icon_color() const;
      virtual label_alignment get_label_alignment() const;
      virtual rect            get_margin() const;
      virtual float           get_corner_radius() const;

      virtual float           get_corner_radius_top_left() const;
      virtual float           get_corner_radius_top_right() const;
      virtual float           get_corner_radius_bottom_left() const;
      virtual float           get_corner_radius_bottom_right() const;
   };

   namespace concepts
   {
      template <typename T>
      concept ButtonStyler = std::is_base_of_v<default_button_styler, std::decay_t<T>>;
   }

   template <concepts::ButtonStyler Base>
   struct basic_button_styler_base : Base, text_writer_u8
   {
      using text_type = std::string_view;
      using base_type = basic_button_styler_base<typename Base::base_type>;

                              basic_button_styler_base(std::string text)
                               : _text(std::move(text))
                              {}

      text_type               get_text() const override;
      void                    set_text(std::string_view text) override;

   private:

      std::string             _text;
   };

   template <concepts::ButtonStyler Base>
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

   template <concepts::ButtonStyler Base>
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

   template <concepts::ButtonStyler Base>
   struct button_styler_with_active_body_color : Base
   {
      using base_type = button_styler_with_active_body_color<typename Base::base_type>;

                              button_styler_with_active_body_color(Base base, color color_)
                               : Base(std::move(base)), _color(color_)
                              {}

      color                   get_active_body_color() const override;
      void                    set_active_body_color(color color_);

   private:

      color                   _color;
   };

   template <concepts::ButtonStyler Base>
   struct button_styler_with_text_color : Base
   {
      using base_type = button_styler_with_text_color<typename Base::base_type>;

                              button_styler_with_text_color(Base base, color color_)
                               : Base(std::move(base)), _color(color_)
                              {}

      color                   get_text_color() const override;
      void                    set_text_color(color color_);

   private:

      color                   _color;
   };

   template <concepts::ButtonStyler Base>
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

   template <concepts::ButtonStyler Base>
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

   template <concepts::ButtonStyler Base>
   struct button_styler_with_icon_color : Base
   {
      using base_type = button_styler_with_icon_color<typename Base::base_type>;

                              button_styler_with_icon_color(Base base, color color_)
                               : Base(std::move(base)), _color(color_)
                              {}

      color                   get_icon_color() const override;
      void                    set_icon_color(color color_);

   private:

      color                   _color;
   };

   template <concepts::ButtonStyler Base>
   struct button_styler_with_label_alignment : Base
   {
      using label_alignment = default_button_styler::label_alignment;
      using base_type = button_styler_with_label_alignment<typename Base::base_type>;

                              button_styler_with_label_alignment(Base base, label_alignment alignment)
                               : Base(std::move(base)), _label_alignment(alignment)
                              {}

      label_alignment         get_label_alignment() const override;
      void                    set_label_alignment(label_alignment alignment);
      void                    align_left();
      void                    align_center();
      void                    align_right();

   private:

      label_alignment         _label_alignment;
   };

   template <concepts::ButtonStyler Base>
   struct button_styler_with_margin : Base
   {
      using base_type = button_styler_with_margin<typename Base::base_type>;

                              button_styler_with_margin(Base base, rect const& margin)
                               : Base(std::move(base)), _margin(margin)
                              {}

      rect                    get_margin() const override;
      void                    set_margin(rect const& margin);

   private:

      rect                    _margin;
   };

   template <concepts::ButtonStyler Base>
   struct button_styler_with_corner_radius : Base
   {
      using base_type = button_styler_with_corner_radius<typename Base::base_type>;
      constexpr static bool has_default_corner_radius = false;

                              button_styler_with_corner_radius(Base base, float r)
                               : Base(std::move(base)), _radius(r)
                              {}

      float                   get_corner_radius() const override;
      void                    set_corner_radius(float r);

   private:

      float                   _radius;
   };

   template <typename Base, default_button_styler::direction Direction>
   struct button_styler_rounded_half : Base
   {
      using base_type = button_styler_rounded_half<typename Base::base_type, Direction>;
      using direction_t = default_button_styler::direction;
      constexpr static bool has_default_corner_radius = false;

                              button_styler_rounded_half(Base base, float r)
                               : Base(std::move(base)), _radius(r)
                              {}

      virtual float           get_corner_radius_top_left() const override;
      virtual float           get_corner_radius_top_right() const override;
      virtual float           get_corner_radius_bottom_left() const override;
      virtual float           get_corner_radius_bottom_right() const override;
      void                    set_corner_radius(float r);

   private:

      float                   _radius;
   };

   template<typename Base, default_button_styler::direction Direction>
   struct button_styler_rounded_half_default : Base
   {
      using base_type = button_styler_rounded_half_default<typename Base::base_type, Direction>;
      using direction_t = default_button_styler::direction;
      constexpr static bool has_default_corner_radius = false;

                              button_styler_rounded_half_default(Base const& base)
                               : Base(base)
                              {}

      virtual float           get_corner_radius_top_left() const override;
      virtual float           get_corner_radius_top_right() const override;
      virtual float           get_corner_radius_bottom_left() const override;
      virtual float           get_corner_radius_bottom_right() const override;
   };

   template<typename Base, default_button_styler::corner Corner>
   struct button_styler_rounded_corner : Base
   {
      using base_type = button_styler_rounded_corner<typename Base::base_type, Corner>;
      using corner_t = default_button_styler::corner;
      constexpr static bool has_default_corner_radius = false;

                              button_styler_rounded_corner(Base base, float r)
                               : Base(std::move(base))
                               , _radius(r)
                              {}

      virtual float           get_corner_radius_top_left() const override;
      virtual float           get_corner_radius_top_right() const override;
      virtual float           get_corner_radius_bottom_left() const override;
      virtual float           get_corner_radius_bottom_right() const override;
      void                    set_corner_radius(float r);

   private:

      float                  _radius;
   };

   template <concepts::ButtonStyler Base>
   struct button_styler_with_individual_corner_radii : Base
   {
      using base_type = button_styler_with_individual_corner_radii<typename Base::base_type>;
      constexpr static bool has_default_corner_radius = false;

                              button_styler_with_individual_corner_radii(
                                 Base base,
                                 float top_left,
                                 float top_right,
                                 float bottom_right,
                                 float bottom_left
                              )
                               : Base(std::move(base)),
                                 _top_left(top_left),
                                 _top_right(top_right),
                                 _bottom_right(bottom_right),
                                 _bottom_left(bottom_left)
                              {}

      virtual float           get_corner_radius_top_left() const override;
      virtual float           get_corner_radius_top_right() const override;
      virtual float           get_corner_radius_bottom_left() const override;
      virtual float           get_corner_radius_bottom_right() const override;
      void                    set_corner_radius(
                                 float top_left,
                                 float top_right,
                                 float bottom_right,
                                 float bottom_left
                              );

   private:

      float                   _top_left;
      float                   _top_right;
      float                   _bottom_right;
      float                   _bottom_left;
   };

   template <concepts::ButtonStyler Base>
   struct button_styler_gen : Base
   {
      using Base::Base;
      using base_type = typename Base::base_type;
      using icon_placement_t = default_button_styler::icon_placement;
      using label_alignment_t = default_button_styler::label_alignment;
      using direction_t = default_button_styler::direction;
      using corner_t = default_button_styler::corner;

      template <typename OtherBase>
      using gen = button_styler_gen<OtherBase>;

      using gen_size                = gen<button_styler_with_size<base_type>>;
      using gen_body_color          = gen<button_styler_with_body_color<base_type>>;
      using gen_active_body_color   = gen<button_styler_with_active_body_color<base_type>>;
      using gen_text_color          = gen<button_styler_with_text_color<base_type>>;
      using gen_icon                = gen<button_styler_with_icon<base_type>>;
      using gen_icon_placement      = gen<button_styler_with_icon_placement<base_type>>;
      using gen_icon_color          = gen<button_styler_with_icon_color<base_type>>;
      using gen_label_alignment     = gen<button_styler_with_label_alignment<base_type>>;
      using gen_margin              = gen<button_styler_with_margin<base_type>>;
      using gen_corner_radius       = gen<button_styler_with_corner_radius<base_type>>;
      using gen_individual_corners  = gen<button_styler_with_individual_corner_radii<base_type>>;

      template<direction_t Dir>
      using gen_rounded_half        = button_styler_gen<button_styler_rounded_half<base_type, Dir>>;
      using gen_rounded_half_up     = gen_rounded_half<direction_t::up>;
      using gen_rounded_half_down   = gen_rounded_half<direction_t::down>;
      using gen_rounded_half_left   = gen_rounded_half<direction_t::left>;
      using gen_rounded_half_right  = gen_rounded_half<direction_t::right>;

      template<direction_t Dir>
      using gen_rounded_half_default         = button_styler_gen<button_styler_rounded_half_default<base_type, Dir>>;
      using gen_rounded_half_default_up      = gen_rounded_half_default<direction_t::up>;
      using gen_rounded_half_default_down    = gen_rounded_half_default<direction_t::down>;
      using gen_rounded_half_default_left    = gen_rounded_half_default<direction_t::left>;
      using gen_rounded_half_default_right   = gen_rounded_half_default<direction_t::right>;

      template<corner_t Corner>
      using gen_rounded_corner               = button_styler_gen<button_styler_rounded_corner<base_type, Corner>>;
      using gen_rounded_corner_top_left      = gen_rounded_corner<corner_t::top_left>;
      using gen_rounded_corner_top_right     = gen_rounded_corner<corner_t::top_right>;
      using gen_rounded_corner_bottom_left   = gen_rounded_corner<corner_t::bottom_left>;
      using gen_rounded_corner_bottom_right  = gen_rounded_corner<corner_t::bottom_right>;

      gen_size                         size(float size) const;
      gen_body_color                   body_color(color color_) const;
      gen_active_body_color            active_body_color(color color_) const;
      gen_text_color                   text_color(color color_) const;
      gen_icon                         icon(std::uint32_t code) const;
      gen_icon_placement               icon_left() const;
      gen_icon_placement               icon_right() const;
      gen_icon_color                   icon_color(color color_) const;
      gen_label_alignment              align_left() const;
      gen_label_alignment              align_center() const;
      gen_label_alignment              align_right() const;
      gen_margin                       margin(rect margin) const;
      gen_corner_radius                corner_radius(float r) const;
      gen_individual_corners           corner_radius(
                                          float top_left,
                                          float top_right,
                                          float bottom_right,
                                          float bottom_left
                                       ) const;

      gen_rounded_half_up              rounded_top(float r) const;
      gen_rounded_half_down            rounded_bottom(float r) const;
      gen_rounded_half_left            rounded_left(float r) const;
      gen_rounded_half_right           rounded_right(float r) const;

      gen_rounded_half_default_up      rounded_top() const;
      gen_rounded_half_default_down    rounded_bottom() const;
      gen_rounded_half_default_left    rounded_left() const;
      gen_rounded_half_default_right   rounded_right() const;

      gen_rounded_corner_top_left      rounded_corner_top_left(float r) const;
      gen_rounded_corner_top_right     rounded_corner_top_right(float r) const;
      gen_rounded_corner_bottom_left   rounded_corner_bottom_left(float r) const;
      gen_rounded_corner_bottom_right  rounded_corner_bottom_right(float r) const;
   };

   using basic_button_styler = basic_button_styler_base<default_button_styler>;
   using button_styler = button_styler_gen<basic_button_styler>;

   ////////////////////////////////////////////////////////////////////////////
   // Make a momentary button with label
   ////////////////////////////////////////////////////////////////////////////
   template <concepts::MomentaryButton Base = basic_button>
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
   template <concepts::MomentaryButton Base = basic_button>
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
   template <concepts::MomentaryButton Base = basic_button>
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
   template <concepts::ToggleButton Base = basic_toggle_button>
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
   template <concepts::ToggleButton Base = basic_toggle_button>
   inline auto toggle_button(
      std::uint32_t icon_code
    , std::string label
    , float size
    , color body_color = get_theme().default_button_color)
   {
      return toggle_button<Base>(
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
   template <concepts::ToggleButton Base = basic_toggle_button>
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
   template <concepts::LatchingButton Base = basic_latching_button>
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
   template <concepts::LatchingButton Base = basic_latching_button>
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
   template <concepts::LatchingButton Base = basic_latching_button>
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
      context const& ctx, rect bounds, color color_
    , bool enabled, corner_radii corner_radii
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

   inline color default_button_styler::get_active_body_color() const
   {
      return get_body_color();
   }

   inline color default_button_styler::get_text_color() const
   {
      return get_theme().label_font_color;
   }

   inline std::uint32_t default_button_styler::get_icon() const
   {
      return 0;
   }

   inline default_button_styler::icon_placement default_button_styler::get_icon_placement() const
   {
      return icon_none;
   }

   inline color default_button_styler::get_icon_color() const
   {
      return get_theme().label_font_color;
   }

   inline default_button_styler::label_alignment default_button_styler::get_label_alignment() const
   {
      return align_center;
   }

   inline rect default_button_styler::get_margin() const
   {
      return get_theme().button_margin;
   }

   inline float default_button_styler::get_corner_radius() const
   {
      return get_theme().button_corner_radius;
   }

   inline float default_button_styler::get_corner_radius_top_left() const
   {
      return get_corner_radius();
   }

   inline float default_button_styler::get_corner_radius_top_right() const
   {
      return get_corner_radius();
   }

   inline float default_button_styler::get_corner_radius_bottom_left() const
   {
      return get_corner_radius();
   }

   inline float default_button_styler::get_corner_radius_bottom_right() const
   {
      return get_corner_radius();
   }

   template <concepts::ButtonStyler Base>
   inline std::string_view basic_button_styler_base<Base>::get_text() const
   {
      return _text;
   }

   template <concepts::ButtonStyler Base>
   inline void basic_button_styler_base<Base>::set_text(std::string_view text)
   {
      _text = std::string(text);
   }

   template <concepts::ButtonStyler Base>
   inline float button_styler_with_size<Base>::get_size() const
   {
      return _size;
   }

   template <concepts::ButtonStyler Base>
   inline void button_styler_with_size<Base>::set_size(float size)
   {
      _size = size;
   }

   template <concepts::ButtonStyler Base>
   inline color button_styler_with_body_color<Base>::get_body_color() const
   {
      return _color;
   }

   template <concepts::ButtonStyler Base>
   inline void button_styler_with_body_color<Base>::set_body_color(color color_)
   {
      _color = color_;
   }

   template <concepts::ButtonStyler Base>
   inline color button_styler_with_active_body_color<Base>::get_active_body_color() const
   {
      return _color;
   }

   template <concepts::ButtonStyler Base>
   inline void button_styler_with_active_body_color<Base>::set_active_body_color(color color_)
   {
      _color = color_;
   }

   template <concepts::ButtonStyler Base>
   inline color button_styler_with_text_color<Base>::get_text_color() const
   {
      return _color;
   }

   template <concepts::ButtonStyler Base>
   inline void button_styler_with_text_color<Base>::set_text_color(color color_)
   {
      _color = color_;
   }

   template <concepts::ButtonStyler Base>
   inline std::uint32_t button_styler_with_icon<Base>::get_icon() const
   {
      return _code;
   }

   template <concepts::ButtonStyler Base>
   inline void button_styler_with_icon<Base>:: set_icon(std::uint32_t code)
   {
      _code = code;
   }

   template <concepts::ButtonStyler Base>
   inline button_styler::icon_placement
   button_styler_with_icon<Base>::get_icon_placement() const
   {
      return icon_placement::icon_right;
   }

   template <concepts::ButtonStyler Base>
   inline default_button_styler::icon_placement
   button_styler_with_icon_placement<Base>::get_icon_placement() const
   {
      return _icon_placement;
   }

   template <concepts::ButtonStyler Base>
   inline color button_styler_with_icon_color<Base>::get_icon_color() const
   {
      return _color;
   }

   template <concepts::ButtonStyler Base>
   inline void button_styler_with_icon_color<Base>::set_icon_color(color color_)
   {
      _color = color_;
   }

   template <concepts::ButtonStyler Base>
   inline void button_styler_with_icon_placement<Base>::set_icon_placement(icon_placement placement)
   {
      _icon_placement = placement;
   }

   template <concepts::ButtonStyler Base>
   inline void button_styler_with_icon_placement<Base>::icon_left()
   {
      _icon_placement = icon_placement::icon_left;
   }

   template <concepts::ButtonStyler Base>
   inline void button_styler_with_icon_placement<Base>::icon_right()
   {
      _icon_placement = icon_placement::icon_right;
   }

   template <concepts::ButtonStyler Base>
   inline default_button_styler::label_alignment
   button_styler_with_label_alignment<Base>::get_label_alignment() const
   {
      return _label_alignment;
   }

   template <concepts::ButtonStyler Base>
   inline void button_styler_with_label_alignment<Base>::set_label_alignment(label_alignment alignment)
   {
      _label_alignment = alignment;
   }

   template <concepts::ButtonStyler Base>
   inline void button_styler_with_label_alignment<Base>::align_left()
   {
      _label_alignment = label_alignment::align_left;
   }

   template <concepts::ButtonStyler Base>
   inline void button_styler_with_label_alignment<Base>::align_center()
   {
      _label_alignment = label_alignment::align_center;
   }

   template <concepts::ButtonStyler Base>
   inline void button_styler_with_label_alignment<Base>::align_right()
   {
      _label_alignment = label_alignment::align_right;
   }

   template <concepts::ButtonStyler Base>
   inline rect button_styler_with_margin<Base>::get_margin() const
   {
      return _margin;
   }

   template <concepts::ButtonStyler Base>
   inline void button_styler_with_margin<Base>::set_margin(rect const& margin)
   {
      _margin = margin;
   }

   template <concepts::ButtonStyler Base>
   inline float button_styler_with_corner_radius<Base>::get_corner_radius() const
   {
      return _radius;
   }

   template <concepts::ButtonStyler Base>
   inline void button_styler_with_corner_radius<Base>::set_corner_radius(float r)
   {
      _radius = r;
   }

   template <typename Base, default_button_styler::direction Direction>
   inline float button_styler_rounded_half<Base, Direction>::get_corner_radius_top_left() const
   {
      if constexpr (Direction == direction_t::up || Direction == direction_t::left)
         return _radius;
      else if constexpr (!Base::has_default_corner_radius)
         return Base::get_corner_radius_top_left();
      else
         return 0.0f;
   }

   template <typename Base, default_button_styler::direction Direction>
   inline float button_styler_rounded_half<Base, Direction>::get_corner_radius_top_right() const
   {
      if constexpr (Direction == direction_t::up || Direction == direction_t::right)
         return _radius;
      else if constexpr (!Base::has_default_corner_radius)
         return Base::get_corner_radius_top_right();
      else
         return 0.0f;
   }

   template <typename Base, default_button_styler::direction Direction>
   inline float button_styler_rounded_half<Base, Direction>::get_corner_radius_bottom_left() const
   {
      if constexpr (Direction == direction_t::down || Direction == direction_t::left)
         return _radius;
      else if constexpr (!Base::has_default_corner_radius)
         return Base::get_corner_radius_bottom_left();
      else
         return 0.0f;
   }

   template <typename Base, default_button_styler::direction Direction>
   inline float button_styler_rounded_half<Base, Direction>::get_corner_radius_bottom_right() const
   {
      if constexpr (Direction == direction_t::down || Direction == direction_t::right)
         return _radius;
      else if constexpr (!Base::has_default_corner_radius)
         return Base::get_corner_radius_bottom_right();
      else
         return 0.0f;
   }

   template <typename Base, default_button_styler::direction Direction>
   inline void button_styler_rounded_half<Base, Direction>::set_corner_radius(float r)
   {
      _radius = r;
   }

   template <typename Base, default_button_styler::direction Direction>
   inline float button_styler_rounded_half_default<Base, Direction>::get_corner_radius_top_left() const
   {
      if constexpr (Direction == direction_t::up || Direction == direction_t::left)
         return get_theme().button_corner_radius;
      else if constexpr (!Base::has_default_corner_radius)
         return Base::get_corner_radius_top_left();
      else
         return 0.0f;
   }

   template <typename Base, default_button_styler::direction Direction>
   inline float button_styler_rounded_half_default<Base, Direction>::get_corner_radius_top_right() const
   {
      if constexpr (Direction == direction_t::up || Direction == direction_t::right)
         return get_theme().button_corner_radius;
      else if constexpr (!Base::has_default_corner_radius)
         return Base::get_corner_radius_top_right();
      else
         return 0.0f;
   }

   template <typename Base, default_button_styler::direction Direction>
   inline float button_styler_rounded_half_default<Base, Direction>::get_corner_radius_bottom_left() const
   {
      if constexpr (Direction == direction_t::down || Direction == direction_t::left)
         return get_theme().button_corner_radius;
      else if constexpr (!Base::has_default_corner_radius)
         return Base::get_corner_radius_bottom_left();
      else
         return 0.0f;
   }

   template <typename Base, default_button_styler::direction Direction>
   inline float button_styler_rounded_half_default<Base, Direction>::get_corner_radius_bottom_right() const
   {
      if constexpr (Direction == direction_t::down || Direction == direction_t::right)
         return get_theme().button_corner_radius;
      else if constexpr (!Base::has_default_corner_radius)
         return Base::get_corner_radius_bottom_right();
      else
         return 0.0f;
   }

   template <typename Base, default_button_styler::corner Corner>
   inline float button_styler_rounded_corner<Base, Corner>::get_corner_radius_top_left() const
   {
      if constexpr (Corner == corner_t::top_left)
         return _radius;
      else if constexpr (!Base::has_default_corner_radius)
         return Base::get_corner_radius_top_left();
      else
         return 0.0f;
   }

   template <typename Base, default_button_styler::corner Corner>
   inline float button_styler_rounded_corner<Base, Corner>::get_corner_radius_top_right() const
   {
      if constexpr (Corner == corner_t::top_right)
         return _radius;
      else if constexpr (!Base::has_default_corner_radius)
         return Base::get_corner_radius_top_right();
      else
         return 0.0f;
   }

   template <typename Base, default_button_styler::corner Corner>
   inline float button_styler_rounded_corner<Base, Corner>::get_corner_radius_bottom_left() const
   {
      if constexpr (Corner == corner_t::bottom_left)
         return _radius;
      else if constexpr (!Base::has_default_corner_radius)
         return Base::get_corner_radius_bottom_left();
      else
         return 0.0f;
   }

   template <typename Base, default_button_styler::corner Corner>
   inline float button_styler_rounded_corner<Base, Corner>::get_corner_radius_bottom_right() const
   {
      if constexpr (Corner == corner_t::bottom_right)
         return _radius;
      else if constexpr (!Base::has_default_corner_radius)
         return Base::get_corner_radius_bottom_right();
      else
         return 0.0f;
   }

   template <typename Base, default_button_styler::corner Corner>
   inline void button_styler_rounded_corner<Base, Corner>::set_corner_radius(float r)
   {
      _radius = r;
   }

   template <concepts::ButtonStyler Base>
   inline float button_styler_with_individual_corner_radii<Base>::get_corner_radius_top_left() const
   {
      return _top_left;
   }

   template <concepts::ButtonStyler Base>
   inline float button_styler_with_individual_corner_radii<Base>::get_corner_radius_top_right() const
   {
      return _top_right;
   }

   template <concepts::ButtonStyler Base>
   inline float button_styler_with_individual_corner_radii<Base>::get_corner_radius_bottom_left() const
   {
      return _bottom_left;
   }

   template <concepts::ButtonStyler Base>
   inline float button_styler_with_individual_corner_radii<Base>::get_corner_radius_bottom_right() const
   {
      return _bottom_right;
   }

   template <concepts::ButtonStyler Base>
   inline void button_styler_with_individual_corner_radii<Base>::set_corner_radius(
      float top_left,
      float top_right,
      float bottom_right,
      float bottom_left
   )
   {
      _top_left = top_left;
      _top_right = top_right;
      _bottom_right = bottom_right;
      _bottom_left = bottom_left;
   }

   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::gen_size
   button_styler_gen<Base>::size(float size) const
   {
      return {*this, size};
   }

   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::gen_body_color
   button_styler_gen<Base>::body_color(color color_) const
   {
      return {*this, color_};
   }

   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::gen_active_body_color
   button_styler_gen<Base>::active_body_color(color color_) const
   {
      return {*this, color_};
   }

   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::gen_icon
   button_styler_gen<Base>::icon(std::uint32_t code) const
   {
      return {*this, code}; }

   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::gen_icon_placement
   button_styler_gen<Base>::icon_left() const
   {
      return {*this, icon_placement_t::icon_left};
   }

   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::gen_icon_placement
   button_styler_gen<Base>::icon_right() const
   {
      return {*this, icon_placement_t::icon_right};
   }

   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::gen_label_alignment
   button_styler_gen<Base>::align_left() const
   {
      return {*this, label_alignment_t::align_left};
   }

   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::gen_label_alignment
   button_styler_gen<Base>::align_center() const
   {
      return {*this, label_alignment_t::align_center};
   }

   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::gen_label_alignment
   button_styler_gen<Base>::align_right() const
   {
      return {*this, label_alignment_t::align_right};
   }

   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::gen_margin
   button_styler_gen<Base>::margin(rect margin) const
   {
      return {*this, margin};
   }

   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::gen_corner_radius
   button_styler_gen<Base>::corner_radius(float r) const
   {
      return {*this, r};
   }

   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::gen_text_color
   button_styler_gen<Base>::text_color(color color_) const
   {
      return {*this, color_};
   }

   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::gen_rounded_half_up
   button_styler_gen<Base>::rounded_top(float r) const
   {
      return {*this, r};
   }

   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::gen_rounded_half_down
   button_styler_gen<Base>::rounded_bottom(float r) const
   {
      return {*this, r};
   }

   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::gen_rounded_half_left
   button_styler_gen<Base>::rounded_left(float r) const
   {
      return {*this, r};
   }

   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::gen_rounded_half_right
   button_styler_gen<Base>::rounded_right(float r) const
   {
      return {*this, r};
   }

   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::gen_rounded_half_default_up
   button_styler_gen<Base>::rounded_top() const
   {
      return {*this};
   }

   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::gen_rounded_half_default_down
   button_styler_gen<Base>::rounded_bottom() const
   {
      return {*this};
   }

   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::gen_rounded_half_default_left
   button_styler_gen<Base>::rounded_left() const
   {
      return {*this};
   }

   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::gen_rounded_half_default_right
   button_styler_gen<Base>::rounded_right() const
   {
      return {*this};
   }

   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::template gen_rounded_corner<default_button_styler::top_left>
   button_styler_gen<Base>::rounded_corner_top_left(float r) const
   {
      return {*this, r};
   }

   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::template gen_rounded_corner<default_button_styler::top_right>
   button_styler_gen<Base>::rounded_corner_top_right(float r) const
   {
      return {*this, r};
   }

   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::template gen_rounded_corner<default_button_styler::bottom_left>
   button_styler_gen<Base>::rounded_corner_bottom_left(float r) const
   {
      return {*this, r};
   }

   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::template gen_rounded_corner<default_button_styler::bottom_right>
   button_styler_gen<Base>::rounded_corner_bottom_right(float r) const
   {
      return {*this, r};
   }

   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::gen_individual_corners
   button_styler_gen<Base>::corner_radius(float top_left, float top_right, float bottom_right, float bottom_left) const
   {
      return {*this, top_left, top_right, bottom_right, bottom_left};
   }
}

#endif
