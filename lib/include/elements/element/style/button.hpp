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
   /**
    * \brief Base class for button styling.
    *
    * The `button_styler_base` class is responsible for providing the basic
    * styling and behavior for buttons. It inherits from `element` and
    * `basic_receiver<button_state>`, allowing it to handle button states
    * and interact with the user interface.
    *
    * This class provides the foundational functionality for button styling,
    * including cursor handling and control requirements.
    */
   struct button_styler_base : element, basic_receiver<button_state>
   {
      bool                    cursor(context const& ctx, point p, cursor_tracking status) override;
      bool                    wants_control() const override;
   };

   /**
    * \brief
    *    Default button styler class.
    *
    *    The `default_button_styler` class provides the default styling and
    *    behavior for buttons. It inherits from `button_styler_base` and
    *    `text_reader_u8`, allowing it to handle button states and retrieve text
    *    via the `get_text()` member function.
    *
    *    This class includes various customization options for button
    *    appearance, such as icon placement, label alignment, and corner radius.
    *    It also provides functions to retrieve colors, scales, and margins for
    *    the button.
    */
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

   /**
    * \brief
    *    Basic button styler base class.
    *
    *    The `basic_button_styler_base` class provides a base implementation
    *    for button stylers.
    *
    *    This class allows setting and retrieving the button text via the
    *    `set_text` and `get_text` member functions. It is designed to be
    *    extended or used as a base class for creating button stylers with
    *    text handling functionality.
    *
    * \tparam Base
    *    The base button styler class.
    */
   template <concepts::ButtonStyler Base>
   struct basic_button_styler_base : Base, text_writer_u8
   {
      using text_type = std::string_view;
      using base_type = basic_button_styler_base<typename Base::base_type>;

                              basic_button_styler_base(std::string text);

      text_type               get_text() const override;
      void                    set_text(std::string_view text) override;

   private:

      std::string             _text;
   };

   /**
    * \brief
    *    Button styler with size.
    *
    *    The `button_styler_with_size` class provides the ability to set the
    *    size of a button. The size is relative to the default size of the
    *    button. A size of 1.0 represents the default size, values greater
    *    than 1.0 scale the button up, and values less than 1.0 scale the
    *    button down.
    *
    * \tparam Base
    *    The base button styler class. Must conform to the `ButtonStyler`
    *    concept.
    */
   template <concepts::ButtonStyler Base>
   struct button_styler_with_size : Base
   {
      using base_type = button_styler_with_size<typename Base::base_type>;

                              button_styler_with_size(Base base, float size);

      float                   get_size() const override;
      void                    set_size(float size);

   private:

      float                   _size;
   };

   /**
    * \brief
    *    Button styler with body color.
    *
    *    The `button_styler_with_body_color` class provides the ability to
    *    set the body color of a button.
    *
    * \tparam Base
    *    The base button styler class. Must conform to the `ButtonStyler`
    *    concept.
    */
   template <concepts::ButtonStyler Base>
   struct button_styler_with_body_color : Base
   {
      using base_type = button_styler_with_body_color<typename Base::base_type>;

                              button_styler_with_body_color(Base base, color color_);

      color                   get_body_color() const override;
      void                    set_body_color(color color_);

   private:

      color                   _color;
   };

   /**
    * \brief
    *    Button styler with active body color.
    *
    *    The `button_styler_with_active_body_color` class provides the
    *    ability to set the active body color of a button.
    *
    * \tparam Base
    *    The base button styler class. Must conform to the `ButtonStyler`
    *    concept.
    */
   template <concepts::ButtonStyler Base>
   struct button_styler_with_active_body_color : Base
   {
      using base_type = button_styler_with_active_body_color<typename Base::base_type>;

                              button_styler_with_active_body_color(Base base, color color_);

      color                   get_active_body_color() const override;
      void                    set_active_body_color(color color_);

   private:

      color                   _color;
   };

   /**
    * \brief
    *   Button styler with text color.
    *
    *    The `button_styler_with_text_color` class provides the ability to
    *    set the text color of a button.
    *
    * \tparam Base
    *    The base button styler class. Must conform to the `ButtonStyler`
    *    concept.
    */
   template <concepts::ButtonStyler Base>
   struct button_styler_with_text_color : Base
   {
      using base_type = button_styler_with_text_color<typename Base::base_type>;

                              button_styler_with_text_color(Base base, color color_);

      color                   get_text_color() const override;
      void                    set_text_color(color color_);

   private:

      color                   _color;
   };

   /**
    * \brief
    *    Button styler with icon.
    *
    *    The `button_styler_with_icon` class provides the ability to set the
    *    icon of a button.
    *
    * \tparam Base
    *    The base button styler class. Must conform to the `ButtonStyler`
    *    concept.
    */
   template <concepts::ButtonStyler Base>
   struct button_styler_with_icon : Base
   {
      using icon_placement = default_button_styler::icon_placement;
      using base_type = button_styler_with_icon<typename Base::base_type>;

                              button_styler_with_icon(Base base, std::uint32_t code);

      std::uint32_t           get_icon() const override;
      void                    set_icon(std::uint32_t code);
      icon_placement          get_icon_placement() const override;

   private:

      std::uint32_t           _code;
      icon_placement          _icon_placement;
   };

   /**
    * \brief
    *    Button styler with icon placement.
    *
    *    The `button_styler_with_icon_placement` class provides the ability
    *    to set the icon placement of a button.
    *
    * \tparam Base
    *    The base button styler class. Must conform to the `ButtonStyler`
    *    concept.
    */
   template <concepts::ButtonStyler Base>
   struct button_styler_with_icon_placement : Base
   {
      using icon_placement = default_button_styler::icon_placement;
      using base_type = button_styler_with_icon_placement<typename Base::base_type>;

                              button_styler_with_icon_placement(Base base, icon_placement placement);

      icon_placement          get_icon_placement() const override;
      void                    set_icon_placement(icon_placement placement);
      void                    icon_left();
      void                    icon_right();

   private:

      icon_placement          _icon_placement;
   };

   /**
    * \brief
    *    Button styler with icon color.
    *
    *    The `button_styler_with_icon_color` class provides the ability to
    *    set the icon color of a button.
    *
    * \tparam Base
    *    The base button styler class. Must conform to the `ButtonStyler`
    *    concept.
    */
   template <concepts::ButtonStyler Base>
   struct button_styler_with_icon_color : Base
   {
      using base_type = button_styler_with_icon_color<typename Base::base_type>;

                              button_styler_with_icon_color(Base base, color color_);

      color                   get_icon_color() const override;
      void                    set_icon_color(color color_);

   private:

      color                   _color;
   };

   /**
    * \brief
    *    Button styler with label alignment.
    *
    *    The `button_styler_with_label_alignment` class provides the ability
    *    to set the label alignment of a button.
    *
    * \tparam Base
    *    The base button styler class. Must conform to the `ButtonStyler`
    *    concept.
    */
   template <concepts::ButtonStyler Base>
   struct button_styler_with_label_alignment : Base
   {
      using label_alignment = default_button_styler::label_alignment;
      using base_type = button_styler_with_label_alignment<typename Base::base_type>;

                              button_styler_with_label_alignment(Base base, label_alignment alignment);

      label_alignment         get_label_alignment() const override;
      void                    set_label_alignment(label_alignment alignment);
      void                    align_left();
      void                    align_center();
      void                    align_right();

   private:

      label_alignment         _label_alignment;
   };

   /**
    * \brief
    *    Button styler with margin.
    *
    *    The `button_styler_with_margin` class provides the ability to set
    *    the margin of a button.
    *
    * \tparam Base
    *    The base button styler class. Must conform to the `ButtonStyler`
    *    concept.
    */
   template <concepts::ButtonStyler Base>
   struct button_styler_with_margin : Base
   {
      using base_type = button_styler_with_margin<typename Base::base_type>;

                              button_styler_with_margin(Base base, rect const& margin);

      rect                    get_margin() const override;
      void                    set_margin(rect const& margin);

   private:

      rect                    _margin;
   };

   /**
    * \brief
    *    Button styler with corner radius.
    *
    *    The `button_styler_with_corner_radius` class provides the ability to
    *    set the corner radius of a button.
    *
    * \tparam Base
    *    The base button styler class. Must conform to the `ButtonStyler`
    *    concept.
    */
   template <concepts::ButtonStyler Base>
   struct button_styler_with_corner_radius : Base
   {
      using base_type = button_styler_with_corner_radius<typename Base::base_type>;
      constexpr static bool has_default_corner_radius = false;

                              button_styler_with_corner_radius(Base base, float r);

      float                   get_corner_radius() const override;
      void                    set_corner_radius(float r);

   private:

      float                   _radius;
   };

   /**
    * \brief
    *    Button styler with rounded corners on one half.
    *
    *    The `button_styler_rounded_half` class provides styling for buttons
    *    with rounded corners on one half, based on the specified direction.
    *    This class allows customization of the corner radius for two corners
    *    on the specified side of the button, as determined by the
    *    `Direction`.
    *
    * \tparam Base
    *    The base button styler class. Must conform to the `ButtonStyler`
    *    concept.
    *
    * \tparam Direction
    *    The direction specifying which half of the button has rounded corners.
    */
   template <concepts::ButtonStyler Base, default_button_styler::direction Direction>
   struct button_styler_rounded_half : Base
   {
      using base_type = button_styler_rounded_half<typename Base::base_type, Direction>;
      using direction_t = default_button_styler::direction;
      constexpr static bool has_default_corner_radius = false;

                              button_styler_rounded_half(Base base, float r);

      virtual float           get_corner_radius_top_left() const override;
      virtual float           get_corner_radius_top_right() const override;
      virtual float           get_corner_radius_bottom_left() const override;
      virtual float           get_corner_radius_bottom_right() const override;
      void                    set_corner_radius(float r);

   private:

      float                   _radius;
   };

   /**
    * \brief
    *    Button styler with rounded corners on one half.
    *
    *    The `button_styler_rounded_half_default` class provides the default
    *    implementation for buttons with rounded corners on one half, based
    *    on the specified direction. It provides default values for the
    *    corner radius via the member functions.
    *
    * \tparam Base
    *    The base button styler class.
    *
    * \tparam Direction
    *    The direction specifying which half of the button has rounded corners.
    */
   template <concepts::ButtonStyler Base, default_button_styler::direction Direction>
   struct button_styler_rounded_half_default : Base
   {
      using base_type = button_styler_rounded_half_default<typename Base::base_type, Direction>;
      using direction_t = default_button_styler::direction;
      constexpr static bool has_default_corner_radius = false;

                              button_styler_rounded_half_default(Base const& base);

      virtual float           get_corner_radius_top_left() const override;
      virtual float           get_corner_radius_top_right() const override;
      virtual float           get_corner_radius_bottom_left() const override;
      virtual float           get_corner_radius_bottom_right() const override;
   };

   /**
    * \brief
    *    Button styler with a rounded corner.
    *
    *    The `button_styler_rounded_corner` class provides styling for
    *    buttons with a single rounded corner, based on the specified corner.
    *    This class allows customization of the corner radius for one
    *    specific corner of the button, as determined by the `Corner`
    *    template parameter.
    *
    * \tparam Base
    *    The base button styler class.
    *
    * \tparam Corner
    *    The corner specifying which corner of the button has a rounded radius.
    */
   template <concepts::ButtonStyler Base, default_button_styler::corner Corner>
   struct button_styler_rounded_corner : Base
   {
      using base_type = button_styler_rounded_corner<typename Base::base_type, Corner>;
      using corner_t = default_button_styler::corner;
      constexpr static bool has_default_corner_radius = false;

                              button_styler_rounded_corner(Base base, float r);

      virtual float           get_corner_radius_top_left() const override;
      virtual float           get_corner_radius_top_right() const override;
      virtual float           get_corner_radius_bottom_left() const override;
      virtual float           get_corner_radius_bottom_right() const override;
      void                    set_corner_radius(float r);

   private:

      float                  _radius;
   };

   /**
    * \brief
    *    Button styler with individual corner radii.
    *
    *    The `button_styler_with_individual_corner_radii` class provides
    *    styling for buttons with customizable corner radii for each
    *    individual corner.
    *
    * \tparam Base
    *    The base button styler class.
    */
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
                              );

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

   /**
    * \brief
    *    Button styler generator and styler template class.
    *
    *    The `button_styler_gen` class allows customization of button size,
    *    body color, active body color, text color, icon, icon placement,
    *    icon color, label alignment, margin, and corner radius. It also
    *    supports rounded corners for individual corners and rounded halves
    *    in different directions.
    *
    *    Each member function of this class generates a new button styler
    *    with the specified styling attribute by extending the base styler
    *    with additional properties. This allows for the flexible composition
    *    of various styling attributes. At the same time, it is also a button
    *    styler in and of itself.
    *
    * \tparam Base
    *    The base button styler class.
    */
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

   template <concepts::MomentaryButton Base = basic_button>
   inline auto button(
      std::string label
    , float size = 1.0
    , color body_color = get_theme().default_button_color);

   template <concepts::MomentaryButton Base = basic_button>
   inline auto button(
      std::uint32_t icon_code
    , std::string label
    , float size
    , color body_color = get_theme().default_button_color);

   template <concepts::MomentaryButton Base = basic_button>
   inline auto button(
      std::string label
    , std::uint32_t icon_code
    , float size
    , color body_color = get_theme().default_button_color);

   template <concepts::ToggleButton Base = basic_toggle_button>
   inline auto toggle_button(
      std::string label
    , float size = 1.0
    , color body_color = get_theme().default_button_color);

   template <concepts::ToggleButton Base = basic_toggle_button>
   inline auto toggle_button(
      std::uint32_t icon_code
    , std::string label
    , float size
    , color body_color = get_theme().default_button_color);

   template <concepts::ToggleButton Base = basic_toggle_button>
   inline auto toggle_button(
      std::string label
    , std::uint32_t icon_code
    , float size
    , color body_color = get_theme().default_button_color);

   template <concepts::LatchingButton Base = basic_latching_button>
   inline auto latching_button(
      std::string label
    , float size = 1.0
    , color body_color = get_theme().default_button_color);

   template <concepts::LatchingButton Base = basic_latching_button>
   inline auto latching_button(
      std::uint32_t icon_code
    , std::string label
    , float size
    , color body_color = get_theme().default_button_color);

   template <concepts::LatchingButton Base = basic_latching_button>
   inline auto latching_button(
      std::string label
    , std::uint32_t icon_code
    , float size
    , color body_color = get_theme().default_button_color);

   ////////////////////////////////////////////////////////////////////////////
   // Drawing button utility (private)
   ////////////////////////////////////////////////////////////////////////////
   void draw_button_base(
      context const& ctx, rect bounds, color color_
    , bool enabled, corner_radii corner_radii
   );

   //--------------------------------------------------------------------------
   // Inlines
   //--------------------------------------------------------------------------

   /**
    * \brief
    *    Creates a momentary button with a specified label, size, and body
    *    color.
    *
    * \tparam Base
    *    The base type for the momentary button, which must satisfy the
    *    `concepts::MomentaryButton` concept.
    *
    * \param label
    *    The label text to be displayed on the button.
    *
    * \param size
    *    The size of the button.
    *
    * \param body_color
    *    The color of the button's body.
    *
    * \return
    *    A momentary button styled with the specified label, size, and body
    *    color.
    */
   template <concepts::MomentaryButton Base>
   inline auto button(
      std::string label
    , float size
    , color body_color)
   {
      return momentary_button<Base>(
         button_styler{std::move(label)}
            .size(size)
            .body_color(body_color)
      );
   }

   /**
    * \brief
    *    Creates a momentary button with a specified icon (on the left),
    *    label (on the right), size, and body color.
    *
    * \tparam Base
    *    The base type for the momentary button, which must satisfy the
    *    `concepts::MomentaryButton` concept.
    *
    * \param icon_code
    *    The Unicode code point of the icon to be displayed on the right side
    *    of the button.
    *
    * \param label
    *    The label text to be displayed on the right side of the button.
    *
    * \param size
    *    The size of the button.
    *
    * \param body_color
    *    The color of the button's body.
    *
    * \return
    *    A momentary button styled with the specified icon, label, size, and
    *    body color.
    */
   template <concepts::MomentaryButton Base>
   inline auto button(
      std::uint32_t icon_code
    , std::string label
    , float size
    , color body_color)
   {
      return momentary_button<Base>(
         button_styler{std::move(label)}
            .icon(icon_code)
            .icon_left()
            .size(size)
            .body_color(body_color)
      );
   }

   /**
    * \brief
    *    Creates a momentary button with a specified label (on the left),
    *    icon (on the right), size, and body color.
    *
    * \tparam Base
    *    The base type for the momentary button, which must satisfy the
    *    `concepts::MomentaryButton` concept.
    *
    * \param label
    *    The label text to be displayed on the left side of the button.
    *
    * \param icon_code
    *    The Unicode code point of the icon to be displayed on the right side
    *    of the button.
    *
    * \param size
    *    The size of the button.
    *
    * \param body_color
    *    The color of the button's body.
    *
    * \return
    *    A momentary button styled with the specified label, icon, size, and
    *    body color.
    */
   template <concepts::MomentaryButton Base>
   inline auto button(
      std::string label
    , std::uint32_t icon_code
    , float size
    , color body_color)
   {
      return momentary_button<Base>(
         button_styler{std::move(label)}
            .icon(icon_code)
            .icon_right()
            .size(size)
            .body_color(body_color)
      );
   }

   /**
    * \brief
    *    Creates a toggle button with a specified label, size, and body
    *    color.
    *
    * \tparam Base
    *    The base type for the toggle button, which must satisfy the
    *    `concepts::ToggleButton` concept.
    *
    * \param label
    *    The label text to be displayed on the button.
    *
    * \param size
    *    The size of the button.
    *
    * \param body_color
    *    The color of the button's body.
    *
    * \return
    *    A toggle button styled with the specified label, size, and body
    *    color.
    */
   template <concepts::ToggleButton Base>
   inline auto toggle_button(
      std::string label
    , float size
    , color body_color)
   {
      return toggle_button<Base>(
         button_styler{std::move(label)}
            .size(size)
            .body_color(body_color)
      );
   }

   /**
    * \brief
    *    Creates a toggle button with a specified icon (on the left), label
    *    (on the right), size, and body color.
    *
    * \tparam Base
    *    The base type for the toggle button, which must satisfy the
    *    `concepts::ToggleButton` concept.
    *
    * \param icon_code
    *    The Unicode code point of the icon to be displayed on the right side
    *    of the button.
    *
    * \param label
    *    The label text to be displayed on the right side of the button.
    *
    * \param size
    *    The size of the button.
    *
    * \param body_color
    *    The color of the button's body.
    *
    * \return
    *    A toggle button styled with the specified icon, label, size, and
    *    body color.
    */
   template <concepts::ToggleButton Base>
   inline auto toggle_button(
      std::uint32_t icon_code
    , std::string label
    , float size
    , color body_color)
   {
      return toggle_button<Base>(
         button_styler{std::move(label)}
            .icon(icon_code)
            .icon_left()
            .size(size)
            .body_color(body_color)
      );
   }

   /**
    * \brief
    *    Creates a toggle button with a specified label (on the left), icon
    *    (on the right), size, and body color.
    *
    * \tparam Base
    *    The base type for the toggle button, which must satisfy the
    *    `concepts::ToggleButton` concept.
    *
    * \param label
    *    The label text to be displayed on the left side of the button.
    *
    * \param icon_code
    *    The Unicode code point of the icon to be displayed on the right side
    *    of the button.
    *
    * \param size
    *    The size of the button.
    *
    * \param body_color
    *    The color of the button's body.
    *
    * \return
    *    A toggle button styled with the specified label, icon, size, and
    *    body color.
    */
   template <concepts::ToggleButton Base>
   inline auto toggle_button(
      std::string label
    , std::uint32_t icon_code
    , float size
    , color body_color)
   {
      return toggle_button<Base>(
         button_styler{std::move(label)}
            .icon(icon_code)
            .icon_right()
            .size(size)
            .body_color(body_color)
      );
   }

   /**
    * \brief
    *    Creates a latching button with a specified label, size, and body
    *    color.
    *
    * \tparam Base
    *    The base type for the latching button, which must satisfy the
    *    `concepts::LatchingButton` concept.
    *
    * \param label
    *    The label text to be displayed on the button.
    *
    * \param size
    *    The size of the button.
    *
    * \param body_color
    *    The color of the button's body.
    *
    * \return
    *    A latching button styled with the specified label, size, and body
    *    color.
    */
   template <concepts::LatchingButton Base>
   inline auto latching_button(
      std::string label
    , float size
    , color body_color)
   {
      return latching_button<Base>(
         button_styler{std::move(label)}
            .size(size)
            .body_color(body_color)
      );
   }

   /**
    * \brief
    *    Creates a latching button with a specified icon (on the left), label
    *    (on the right), size, and body color.
    *
    * \tparam Base
    *    The base type for the latching button, which must satisfy the
    *    `concepts::LatchingButton` concept.
    *
    * \param icon_code
    *    The Unicode code point of the icon to be displayed on the left side
    *    of the button.
    *
    * \param label
    *    The label text to be displayed on the right side of the button.
    *
    * \param size
    *    The size of the button.
    *
    * \param body_color
    *    The color of the button's body.
    *
    * \return
    *    A latching button styled with the specified icon, label, size, and
    *    body color.
    */
   template <concepts::LatchingButton Base>
   inline auto latching_button(
      std::uint32_t icon_code
    , std::string label
    , float size
    , color body_color)
   {
      return latching_button<Base>(
         button_styler{std::move(label)}
            .icon(icon_code)
            .icon_left()
            .size(size)
            .body_color(body_color)
      );
   }

   /**
    * \brief
    *    Creates a latching button with a specified label (on the left), icon
    *    (on the right), size, and body color.
    *
    * \tparam Base
    *    The base type for the latching button, which must satisfy the
    *    `concepts::LatchingButton` concept.
    *
    * \param label
    *    The label text to be displayed on the left side of the button.
    *
    * \param icon_code
    *    The Unicode code point of the icon to be displayed on the right side
    *    of the button.
    *
    * \param size
    *    The size of the button.
    *
    * \param body_color
    *    The color of the button's body.
    *
    * \return
    *    A latching button styled with the specified label, icon, size, and
    *    body color.
    */
   template <concepts::LatchingButton Base>
   inline auto latching_button(
      std::string label
    , std::uint32_t icon_code
    , float size
    , color body_color)
   {
      return latching_button<Base>(
         button_styler{std::move(label)}
            .icon(icon_code)
            .icon_right()
            .size(size)
            .body_color(body_color)
      );
   }

   /**
    * \brief
    *    Get the default relative size of the button.
    *
    *    A size of 1.0 represents the default size, values greater than 1.0
    *    scale the button up, and values less than 1.0 scale the button down.
    *
    * \return
    *   The default relative size of the button.
    */
   inline float default_button_styler::get_size() const
   {
      return 1.0f;
   }

   /**
    * \brief
    *   Get the default body color of the button.
    *
    * \return
    *    The default body color of the button.
    */
   inline color default_button_styler::get_body_color() const
   {
      return get_theme().default_button_color;
   }

   /**
    * \brief
    *    Get the default active body color of the button.
    *
    * \return
    *    The default active body color of the button.
    */
   inline color default_button_styler::get_active_body_color() const
   {
      return get_body_color();
   }

   /**
    * \brief
    *    Get the default text color of the button.
    *
    * \return
    *    The default text color of the button.
    */
   inline color default_button_styler::get_text_color() const
   {
      return get_theme().label_font_color;
   }

   /**
    * \brief
    *    Get the default icon code of the button.
    *
    * \return
    *    The default icon code of the button: A Unicode code point.
    */
   inline std::uint32_t default_button_styler::get_icon() const
   {
      return 0;
   }

   /**
    * \brief
    *    Get the default icon placement of the button.
    *
    * \return
    *    The default icon placement of the button.
    */
   inline default_button_styler::icon_placement default_button_styler::get_icon_placement() const
   {
      return icon_none;
   }

   /**
    * \brief
    *    Get the default icon color of the button.
    *
    * \return
    *    The default icon color of the button.
    */
   inline color default_button_styler::get_icon_color() const
   {
      return get_theme().label_font_color;
   }

   /**
    * \brief
    *    Get the default label alignment of the button.
    *
    * \return
    *    The default label alignment of the button.
    */
   inline default_button_styler::label_alignment default_button_styler::get_label_alignment() const
   {
      return align_center;
   }

   /**
    * \brief
    *    Get the default margin of the button.
    *
    * \return
    *    The default margin of the button.
    */
   inline rect default_button_styler::get_margin() const
   {
      return get_theme().button_margin;
   }

   /**
    * \brief
    *    Get the default corner radius of the button.
    *
    * \return
    *    The default corner radius of the button.
    */
   inline float default_button_styler::get_corner_radius() const
   {
      return get_theme().button_corner_radius;
   }

   /**
    * \brief
    *    Get the default top-left corner radius of the button.
    *
    * \return
    *    The default top-left corner radius of the button.
    */
   inline float default_button_styler::get_corner_radius_top_left() const
   {
      return get_corner_radius();
   }

   /**
    * \brief
    *    Get the default top-right corner radius of the button.
    *
    * \return
    *    The default top-right corner radius of the button.
    */
   inline float default_button_styler::get_corner_radius_top_right() const
   {
      return get_corner_radius();
   }

   /**
    * \brief
    *    Get the default bottom-left corner radius of the button.
    *
    * \return
    *    The default bottom-left corner radius of the button.
    */
   inline float default_button_styler::get_corner_radius_bottom_left() const
   {
      return get_corner_radius();
   }

   /**
    * \brief
    *    Get the default bottom-right corner radius of the button.
    *
    * \return
    *    The default bottom-right corner radius of the button.
    */
   inline float default_button_styler::get_corner_radius_bottom_right() const
   {
      return get_corner_radius();
   }

   /**
    * \brief
    *    Constructs a basic_button_styler_base styler with the specified
    *    text.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \param text
    *    The text to be displayed on the button.
    */
   template <concepts::ButtonStyler Base>
   basic_button_styler_base<Base>::basic_button_styler_base(std::string text)
    : _text(std::move(text))
   {}

   /**
    * \brief
    *    Get the text displayed on the button.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \return
    *    A `std::string_view` of the text displayed on the button.
    */
   template <concepts::ButtonStyler Base>
   inline std::string_view basic_button_styler_base<Base>::get_text() const
   {
      return _text;
   }

   /**
    * \brief
    *    Set the text to be displayed on the button.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \param text
    *    A `std::string_view` of the new text to be displayed on
    *    the button.
    */
   template <concepts::ButtonStyler Base>
   inline void basic_button_styler_base<Base>::set_text(std::string_view text)
   {
      _text = std::string(text);
   }

   /**
    * \brief
    *    Constructs a button_styler_with_size styler with a specified
    *    relative size.
    *
    *    A size of 1.0 represents the default size, values greater than 1.0
    *    scale the button up, and values less than 1.0 scale the button down.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \param base
    *    The base button styler object.
    *
    * \param size
    *    The relative size of the button.
    */
   template <concepts::ButtonStyler Base>
   inline button_styler_with_size<Base>::button_styler_with_size(Base base, float size)
    : Base(std::move(base)), _size(size)
   {}

   /**
    * \brief
    *    Get the relative size of the button.
    *
    *    A size of 1.0 represents the default size, values greater than 1.0
    *    scale the button up, and values less than 1.0 scale the button down.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \return
    *    A `float` representing the relative size of the button.
    */
   template <concepts::ButtonStyler Base>
   inline float button_styler_with_size<Base>::get_size() const
   {
      return _size;
   }

   /**
    * \brief
    *    Set the relative size of the button.
    *
    *    A size of 1.0 represents the default size, values greater than 1.0
    *    scale the button up, and values less than 1.0 scale the button down.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \param size
    *    A `float` representing the new relative size of the button.
    */
   template <concepts::ButtonStyler Base>
   inline void button_styler_with_size<Base>::set_size(float size)
   {
      _size = size;
   }

   /**
    * \brief
    *    Constructs a button_styler_with_body_color styler with a specified
    *    body color.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \param base
    *    The base button styler object.
    *
    * \param color_
    *    The color of the button's body.
    */
   template <concepts::ButtonStyler Base>
   inline button_styler_with_body_color<Base>::button_styler_with_body_color(Base base, color color_)
    : Base(std::move(base)), _color(color_)
   {}

   /**
    * \brief
    *    Get the body color of the button.
    *
    *    This function returns the body color that is currently set for the button.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \return
    *    A `color` representing the body color of the button.
    */
   template <concepts::ButtonStyler Base>
   inline color button_styler_with_body_color<Base>::get_body_color() const
   {
      return _color;
   }

   /**
    * \brief
    *    Set the body color of the button.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \param color_
    *    A `color` representing the new body color of the button.
    */
   template <concepts::ButtonStyler Base>
   inline void button_styler_with_body_color<Base>::set_body_color(color color_)
   {
      _color = color_;
   }

   /**
    * \brief
    *    Constructs a button_styler_with_active_body_color styler with a
    *    specified active body color.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \param base
    *    The base button styler object.
    *
    * \param color_
    *    The active color of the button's body.
    */
   template <concepts::ButtonStyler Base>
   inline button_styler_with_active_body_color<Base>::button_styler_with_active_body_color(Base base, color color_)
    : Base(std::move(base)), _color(color_)
   {}

   /**
    * \brief
    *    Get the active body color of the button.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \return
    *    A `color` representing the active body color of the button.
    */
   template <concepts::ButtonStyler Base>
   inline color button_styler_with_active_body_color<Base>::get_active_body_color() const
   {
      return _color;
   }

   /**
    * \brief
    *    Set the active body color of the button.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \param color_
    *    A `color` representing the new active body color of the button.
    */
   template <concepts::ButtonStyler Base>
   inline void button_styler_with_active_body_color<Base>::set_active_body_color(color color_)
   {
      _color = color_;
   }

   /**
    * \brief
    *    Constructs a button_styler_with_text_color styler with a specified
    *    text color.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \param base
    *    The base button styler object.
    *
    * \param color_
    *    The color of the button's text.
    */
   template <concepts::ButtonStyler Base>
   inline button_styler_with_text_color<Base>::button_styler_with_text_color(Base base, color color_)
    : Base(std::move(base)), _color(color_)
   {}

   /**
    * \brief
    *    Get the text color of the button.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \return
    *    A `color` representing the text color of the button.
    */
   template <concepts::ButtonStyler Base>
   inline color button_styler_with_text_color<Base>::get_text_color() const
   {
      return _color;
   }

   /**
    * \brief
    *    Set the text color of the button.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \param color_
    *    A `color` representing the new text color of the button.
    */
   template <concepts::ButtonStyler Base>
   inline void button_styler_with_text_color<Base>::set_text_color(color color_)
   {
      _color = color_;
   }

   /**
    * \brief
    *    Constructs a button_styler_with_icon styler with a specified icon
    *    (Unicode code point).
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \param base
    *    The base button styler object.
    *
    * \param code
    *    The Unicode code point of the icon.
    */
   template <concepts::ButtonStyler Base>
   inline button_styler_with_icon<Base>::button_styler_with_icon(Base base, std::uint32_t code)
    : Base(std::move(base)), _code(code)
   {}

   /**
    * \brief
    *    Get the icon of the button (Unicode code point).
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \return
    *    A `std::uint32_t` representing the Unicode code point of the icon.
    */
   template <concepts::ButtonStyler Base>
   inline std::uint32_t button_styler_with_icon<Base>::get_icon() const
   {
      return _code;
   }

   /**
    * \brief
    *    Set the icon of the button (Unicode code point).
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \param code
    *    A `std::uint32_t` representing the Unicode code point of the new
    *    icon.
    */
   template <concepts::ButtonStyler Base>
   inline void button_styler_with_icon<Base>:: set_icon(std::uint32_t code)
   {
      _code = code;
   }

   /**
    * \brief
    *    Get the icon placement of the button.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \return
    *    An `icon_placement` enum value representing the icon placement of
    *    the button.
    */
   template <concepts::ButtonStyler Base>
   inline button_styler::icon_placement
   button_styler_with_icon<Base>::get_icon_placement() const
   {
      return icon_placement::icon_right;
   }

   /**
    * \brief
    *    Constructs a button_styler_with_icon_color styler with a specified
    *    icon color.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \param base
    *    The base button styler object.
    *
    * \param color_
    *    The color of the button's icon.
    */
   template <concepts::ButtonStyler Base>
   inline button_styler_with_icon_color<Base>::button_styler_with_icon_color(Base base, color color_)
    : Base(std::move(base)), _color(color_)
   {}

   /**
    * \brief
    *    Get the icon color of the button.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \return
    *    A `color` representing the icon color of the button.
    */
   template <concepts::ButtonStyler Base>
   inline color button_styler_with_icon_color<Base>::get_icon_color() const
   {
      return _color;
   }

   /**
    * \brief
    *    Set the icon color of the button.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \param color_
    *    A `color` representing the new icon color of the button.
    */
   template <concepts::ButtonStyler Base>
   inline void button_styler_with_icon_color<Base>::set_icon_color(color color_)
   {
      _color = color_;
   }

   /**
    * \brief
    *    Constructs a button_styler_with_icon_placement styler with a
    *    specified icon placement.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \param base
    *    The base button styler object.
    *
    * \param placement
    *    The placement of the button's icon.
    */
   template <concepts::ButtonStyler Base>
   inline button_styler_with_icon_placement<Base>::button_styler_with_icon_placement(Base base, icon_placement placement)
    : Base(std::move(base)), _icon_placement(placement)
   {}

   /**
    * \brief
    *    Get the icon placement of the button.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \return
    *    An `icon_placement` enum value representing the icon placement of the button.
    */
   template <concepts::ButtonStyler Base>
   inline default_button_styler::icon_placement
   button_styler_with_icon_placement<Base>::get_icon_placement() const
   {
      return _icon_placement;
   }

   /**
    * \brief
    *    Set the icon placement of the button.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \param placement
    *    An `icon_placement` enum value representing the new icon placement of the button.
    */
   template <concepts::ButtonStyler Base>
   inline void button_styler_with_icon_placement<Base>::set_icon_placement(icon_placement placement)
   {
      _icon_placement = placement;
   }

   /**
    * \brief
    *    Set the icon placement of the button to the left.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    */
   template <concepts::ButtonStyler Base>
   inline void button_styler_with_icon_placement<Base>::icon_left()
   {
      _icon_placement = icon_placement::icon_left;
   }

   /**
    * \brief
    *    Set the icon placement of the button to the right.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    */
   template <concepts::ButtonStyler Base>
   inline void button_styler_with_icon_placement<Base>::icon_right()
   {
      _icon_placement = icon_placement::icon_right;
   }

   /**
    * \brief
    *    Constructs a button_styler_with_label_alignment styler with a
    *    specified label alignment.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \param base
    *    The base button styler object.
    *
    * \param alignment
    *    The alignment of the button's label.
    */
   template <concepts::ButtonStyler Base>
   inline button_styler_with_label_alignment<Base>::button_styler_with_label_alignment(Base base, label_alignment alignment)
    : Base(std::move(base)), _label_alignment(alignment)
   {}

   /**
    * \brief
    *    Get the label alignment of the button.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \return
    *    A `label_alignment` enum value representing the label alignment of
    *    the button.
    */
   template <concepts::ButtonStyler Base>
   inline default_button_styler::label_alignment
   button_styler_with_label_alignment<Base>::get_label_alignment() const
   {
      return _label_alignment;
   }

   /**
    * \brief
    *    Set the label alignment of the button.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \param alignment
    *    A `label_alignment` enum value representing the new label alignment
    *    of the button.
    */
   template <concepts::ButtonStyler Base>
   inline void button_styler_with_label_alignment<Base>::set_label_alignment(label_alignment alignment)
   {
      _label_alignment = alignment;
   }

   /**
    * \brief
    *    Set the label alignment of the button to the left.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    */
   template <concepts::ButtonStyler Base>
   inline void button_styler_with_label_alignment<Base>::align_left()
   {
      _label_alignment = label_alignment::align_left;
   }

   /**
    * \brief
    *    Set the label alignment of the button to the center.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    */
   template <concepts::ButtonStyler Base>
   inline void button_styler_with_label_alignment<Base>::align_center()
   {
      _label_alignment = label_alignment::align_center;
   }

   /**
    * \brief
    *    Set the label alignment of the button to the right.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    */
   template <concepts::ButtonStyler Base>
   inline void button_styler_with_label_alignment<Base>::align_right()
   {
      _label_alignment = label_alignment::align_right;
   }

   /**
    * \brief
    *    Constructs a button_styler_with_margin styler with a specified
    *    margin.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \param base
    *    The base button styler object.
    *
    * \param margin
    *    The margin of the button.
    */
   template <concepts::ButtonStyler Base>
   inline button_styler_with_margin<Base>::button_styler_with_margin(Base base, rect const& margin)
    : Base(std::move(base)), _margin(margin)
   {}

   /**
    * \brief
    *    Get the margin of the button.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \return
    *    A `rect` representing the margin of the button.
    */
   template <concepts::ButtonStyler Base>
   inline rect button_styler_with_margin<Base>::get_margin() const
   {
      return _margin;
   }

   /**
    * \brief
    *    Set the margin of the button.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \param margin
    *    A `rect` representing the new margin of the button.
    */
   template <concepts::ButtonStyler Base>
   inline void button_styler_with_margin<Base>::set_margin(rect const& margin)
   {
      _margin = margin;
   }

   /**
    * \brief
    *    Constructs a button_styler_with_corner_radius styler with a
    *    specified corner radius.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \param base
    *    The base button styler object.
    *
    * \param r
    *    The corner radius of the button.
    */
   template <concepts::ButtonStyler Base>
   inline button_styler_with_corner_radius<Base>::button_styler_with_corner_radius(Base base, float r)
    : Base(std::move(base)), _radius(r)
   {}

   /**
    * \brief
    *    Get the corner radius of the button.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \return
    *    A `float` representing the corner radius of the button.
    */
   template <concepts::ButtonStyler Base>
   inline float button_styler_with_corner_radius<Base>::get_corner_radius() const
   {
      return _radius;
   }

   /**
    * \brief
    *    Set the corner radius of the button.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \param r
    *    A `float` representing the new corner radius of the button.
    */
   template <concepts::ButtonStyler Base>
   inline void button_styler_with_corner_radius<Base>::set_corner_radius(float r)
   {
      _radius = r;
   }

   /**
    * \brief
    *    Constructs a button styler with the specified radius.
    *
    * \param base
    *    The base button styler.
    *
    * \param r
    *    The radius for the rounded corners.
    */
   template <concepts::ButtonStyler Base, default_button_styler::direction Direction>
   inline button_styler_rounded_half<Base, Direction>::button_styler_rounded_half(Base base, float r)
    : Base(std::move(base)), _radius(r)
   {}

   /**
    * \brief
    *    Get the top-left corner radius.
    *
    * \return
    *    The top-left corner radius.
    */
   template <concepts::ButtonStyler Base, default_button_styler::direction Direction>
   inline float button_styler_rounded_half<Base, Direction>::get_corner_radius_top_left() const
   {
      if constexpr (Direction == direction_t::up || Direction == direction_t::left)
         return _radius;
      else if constexpr (!Base::has_default_corner_radius)
         return Base::get_corner_radius_top_left();
      else
         return 0.0f;
   }

   /**
    * \brief
    *    Get the top-right corner radius.
    *
    * \return
    *    The top-right corner radius.
    */
   template <concepts::ButtonStyler Base, default_button_styler::direction Direction>
   inline float button_styler_rounded_half<Base, Direction>::get_corner_radius_top_right() const
   {
      if constexpr (Direction == direction_t::up || Direction == direction_t::right)
         return _radius;
      else if constexpr (!Base::has_default_corner_radius)
         return Base::get_corner_radius_top_right();
      else
         return 0.0f;
   }

   /**
    * \brief
    *    Get the bottom-left corner radius.
    *
    * \return
    *    The bottom-left corner radius.
    */
   template <concepts::ButtonStyler Base, default_button_styler::direction Direction>
   inline float button_styler_rounded_half<Base, Direction>::get_corner_radius_bottom_left() const
   {
      if constexpr (Direction == direction_t::down || Direction == direction_t::left)
         return _radius;
      else if constexpr (!Base::has_default_corner_radius)
         return Base::get_corner_radius_bottom_left();
      else
         return 0.0f;
   }

   /**
    * \brief Get the bottom-right corner radius.
    *
    * \return The bottom-right corner radius.
    */
   template <concepts::ButtonStyler Base, default_button_styler::direction Direction>
   inline float button_styler_rounded_half<Base, Direction>::get_corner_radius_bottom_right() const
   {
      if constexpr (Direction == direction_t::down || Direction == direction_t::right)
         return _radius;
      else if constexpr (!Base::has_default_corner_radius)
         return Base::get_corner_radius_bottom_right();
      else
         return 0.0f;
   }

   /**
    * \brief
    *    Constructs a button_styler_rounded_half styler with a specified
    *    corner radius.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \tparam Direction
    *    The direction type for the button styler, which must be a value of
    *    `default_button_styler::direction`.
    *
    * \param base
    *    The base button styler object.
    *
    * \param r
    *    The corner radius of the button.
    */
   template <concepts::ButtonStyler Base, default_button_styler::direction Direction>
   inline void button_styler_rounded_half<Base, Direction>::set_corner_radius(float r)
   {
      _radius = r;
   }

   /**
    * \brief
    *    Constructs a button_styler_rounded_half_default styler.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \tparam Direction
    *    The direction type for the button styler, which must be a value of
    *    `default_button_styler::direction`.
    *
    * \param base
    *    The base button styler object.
    */
   template <concepts::ButtonStyler Base, default_button_styler::direction Direction>
   inline button_styler_rounded_half_default<Base, Direction>::button_styler_rounded_half_default(Base const& base)
    : Base(base)
   {}

   /**
    * \brief
    *    Get the top-left corner radius of the button.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \tparam Direction
    *    The direction type for the button styler, which must be a value of
    *    `default_button_styler::direction`.
    *
    * \return
    *    A `float` representing the top-left corner radius of the button.
    */
   template <concepts::ButtonStyler Base, default_button_styler::direction Direction>
   inline float button_styler_rounded_half_default<Base, Direction>::get_corner_radius_top_left() const
   {
      if constexpr (Direction == direction_t::up || Direction == direction_t::left)
         return get_theme().button_corner_radius;
      else if constexpr (!Base::has_default_corner_radius)
         return Base::get_corner_radius_top_left();
      else
         return 0.0f;
   }

   /**
    * \brief
    *    Get the top-right corner radius of the button.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \tparam Direction
    *    The direction type for the button styler, which must be a value of
    *    `default_button_styler::direction`.
    *
    * \return
    *    A `float` representing the top-right corner radius of the button.
    */
   template <concepts::ButtonStyler Base, default_button_styler::direction Direction>
   inline float button_styler_rounded_half_default<Base, Direction>::get_corner_radius_top_right() const
   {
      if constexpr (Direction == direction_t::up || Direction == direction_t::right)
         return get_theme().button_corner_radius;
      else if constexpr (!Base::has_default_corner_radius)
         return Base::get_corner_radius_top_right();
      else
         return 0.0f;
   }

   /**
    * \brief
    *    Get the bottom-left corner radius of the button.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \tparam Direction
    *    The direction type for the button styler, which must be a value of
    *    `default_button_styler::direction`.
    *
    * \return
    *    A `float` representing the bottom-left corner radius of the button.
    */
   template <concepts::ButtonStyler Base, default_button_styler::direction Direction>
   inline float button_styler_rounded_half_default<Base, Direction>::get_corner_radius_bottom_left() const
   {
      if constexpr (Direction == direction_t::down || Direction == direction_t::left)
         return get_theme().button_corner_radius;
      else if constexpr (!Base::has_default_corner_radius)
         return Base::get_corner_radius_bottom_left();
      else
         return 0.0f;
   }

  /**
    * \brief
    *    Get the bottom-right corner radius of the button.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \tparam Direction
    *    The direction type for the button styler, which must be a value of
    *    `default_button_styler::direction`.
    *
    * \return
    *    A `float` representing the bottom-right corner radius of the button.
    */
   template <concepts::ButtonStyler Base, default_button_styler::direction Direction>
   inline float button_styler_rounded_half_default<Base, Direction>::get_corner_radius_bottom_right() const
   {
      if constexpr (Direction == direction_t::down || Direction == direction_t::right)
         return get_theme().button_corner_radius;
      else if constexpr (!Base::has_default_corner_radius)
         return Base::get_corner_radius_bottom_right();
      else
         return 0.0f;
   }

   /**
    * \brief
    *    Constructs a button_styler_rounded_corner styler with a specified
    *    corner radius.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \tparam Corner
    *    The corner type for the button styler, which must be a value of
    *    `default_button_styler::corner`.
    *
    * \param base
    *    The base button styler object.
    *
    * \param r
    *    The corner radius of the button.
    */
   template <concepts::ButtonStyler Base, default_button_styler::corner Corner>
   inline button_styler_rounded_corner<Base, Corner>::button_styler_rounded_corner(Base base, float r)
    : Base(std::move(base))
    , _radius(r)
   {}

   /**
    * \brief
    *    Get the top-left corner radius of the button.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \tparam Corner
    *    The corner type for the button styler, which must be a value of
    *    `default_button_styler::corner`.
    *
    * \return
    *    A `float` representing the top-left corner radius of the button.
    */
   template <concepts::ButtonStyler Base, default_button_styler::corner Corner>
   inline float button_styler_rounded_corner<Base, Corner>::get_corner_radius_top_left() const
   {
      if constexpr (Corner == corner_t::top_left)
         return _radius;
      else if constexpr (!Base::has_default_corner_radius)
         return Base::get_corner_radius_top_left();
      else
         return 0.0f;
   }

   /**
    * \brief
    *    Get the top-right corner radius of the button.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \tparam Corner
    *    The corner type for the button styler, which must be a value of
    *    `default_button_styler::corner`.
    *
    * \return
    *    A `float` representing the top-right corner radius of the button.
    */
   template <concepts::ButtonStyler Base, default_button_styler::corner Corner>
   inline float button_styler_rounded_corner<Base, Corner>::get_corner_radius_top_right() const
   {
      if constexpr (Corner == corner_t::top_right)
         return _radius;
      else if constexpr (!Base::has_default_corner_radius)
         return Base::get_corner_radius_top_right();
      else
         return 0.0f;
   }

   /**
    * \brief
    *    Get the bottom-left corner radius of the button.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \tparam Corner
    *    The corner type for the button styler, which must be a value of
    *    `default_button_styler::corner`.
    *
    * \return
    *    A `float` representing the bottom-left corner radius of the button.
    */
   template <concepts::ButtonStyler Base, default_button_styler::corner Corner>
   inline float button_styler_rounded_corner<Base, Corner>::get_corner_radius_bottom_left() const
   {
      if constexpr (Corner == corner_t::bottom_left)
         return _radius;
      else if constexpr (!Base::has_default_corner_radius)
         return Base::get_corner_radius_bottom_left();
      else
         return 0.0f;
   }

   /**
    * \brief
    *    Get the bottom-right corner radius of the button.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \tparam Corner
    *    The corner type for the button styler, which must be a value of
    *    `default_button_styler::corner`.
    *
    * \return
    *    A `float` representing the bottom-right corner radius of the button.
    */
   template <concepts::ButtonStyler Base, default_button_styler::corner Corner>
   inline float button_styler_rounded_corner<Base, Corner>::get_corner_radius_bottom_right() const
   {
      if constexpr (Corner == corner_t::bottom_right)
         return _radius;
      else if constexpr (!Base::has_default_corner_radius)
         return Base::get_corner_radius_bottom_right();
      else
         return 0.0f;
   }

   /**
    * \brief
    *    Set the corner radius of the button.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \tparam Corner
    *    The corner type for the button styler, which must be a value of
    *    `default_button_styler::corner`.
    *
    * \param r
    *    A `float` representing the new corner radius of the button.
    */
   template <concepts::ButtonStyler Base, default_button_styler::corner Corner>
   inline void button_styler_rounded_corner<Base, Corner>::set_corner_radius(float r)
   {
      _radius = r;
   }

   /**
    * \brief
    *    Constructs a button_styler_with_individual_corner_radii styler with
    *    individual corner radii.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \param base
    *    The base button styler object.
    *
    * \param top_left
    *    The top-left corner radius of the button.
    *
    * \param top_right
    *    The top-right corner radius of the button.
    *
    * \param bottom_right
    *    The bottom-right corner radius of the button.
    *
    * \param bottom_left
    *    The bottom-left corner radius of the button.
    */
   template <concepts::ButtonStyler Base>
   inline button_styler_with_individual_corner_radii<Base>::button_styler_with_individual_corner_radii(
      Base base
    , float top_left
    , float top_right
    , float bottom_right
    , float bottom_left
   )
      : Base(std::move(base)),
      _top_left(top_left),
      _top_right(top_right),
      _bottom_right(bottom_right),
      _bottom_left(bottom_left)
   {}

   /**
    * \brief
    *    Get the top-left corner radius of the button.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \return
    *    A `float` representing the top-left corner radius of the button.
    */
   template <concepts::ButtonStyler Base>
   inline float button_styler_with_individual_corner_radii<Base>::get_corner_radius_top_left() const
   {
      return _top_left;
   }

   /**
    * \brief
    *    Get the top-right corner radius of the button.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \return
    *    A `float` representing the top-right corner radius of the button.
    */
   template <concepts::ButtonStyler Base>
   inline float button_styler_with_individual_corner_radii<Base>::get_corner_radius_top_right() const
   {
      return _top_right;
   }

   /**
    * \brief
    *    Get the bottom-left corner radius of the button.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \return
    *    A `float` representing the bottom-left corner radius of the button.
    */
   template <concepts::ButtonStyler Base>
   inline float button_styler_with_individual_corner_radii<Base>::get_corner_radius_bottom_left() const
   {
      return _bottom_left;
   }

   /**
    * \brief
    *    Get the bottom-right corner radius of the button.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \return
    *    A `float` representing the bottom-right corner radius of the button.
    */
   template <concepts::ButtonStyler Base>
   inline float button_styler_with_individual_corner_radii<Base>::get_corner_radius_bottom_right() const
   {
      return _bottom_right;
   }

   /**
    * \brief
    *    Set the corner radii of the button.
    *
    * \tparam Base
    *    The base type for the button styler, which must satisfy the
    *    `concepts::ButtonStyler` concept.
    *
    * \param top_left
    *    A `float` representing the new top-left corner radius of the button.
    *
    * \param top_right
    *    A `float` representing the new top-right corner radius of the button.
    *
    * \param bottom_right
    *    A `float` representing the new bottom-right corner radius of the button.
    *
    * \param bottom_left
    *    A `float` representing the new bottom-left corner radius of the button.
    */
   template <concepts::ButtonStyler Base>
   inline void button_styler_with_individual_corner_radii<Base>::set_corner_radius(
      float top_left
    , float top_right
    , float bottom_right
    , float bottom_left
   )
   {
      _top_left = top_left;
      _top_right = top_right;
      _bottom_right = bottom_right;
      _bottom_left = bottom_left;
   }

   /**
    * \brief
    *    Returns a button styler with the specified size.
    *
    * \param size
    *    A float representing the size of the button.
    *
    * \return
    *    A button styler with the specified size.
    */
   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::gen_size
   button_styler_gen<Base>::size(float size) const
   {
      return {*this, size};
   }

   /**
    * \brief
    *    Returns a button styler with the specified body color.
    *
    * \param color_
    *    A color representing the body color of the button.
    *
    * \return
    *    A button styler with the specified body color.
    */
   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::gen_body_color
   button_styler_gen<Base>::body_color(color color_) const
   {
      return {*this, color_};
   }

   /**
    * \brief
    *    Returns a button styler with the specified active body color.
    *
    * \param color_
    *    A color representing the active body color of the button.
    *
    * \return
    *    A button styler with the specified active body color.
    */
   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::gen_active_body_color
   button_styler_gen<Base>::active_body_color(color color_) const
   {
      return {*this, color_};
   }

   /**
    * \brief
    *    Returns a button styler with the specified icon.
    *
    * \param code
    *    A std::uint32_t representing the icon code.
    *
    * \return
    *    A button styler with the specified icon.
    */
   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::gen_icon
   button_styler_gen<Base>::icon(std::uint32_t code) const
   {
      return {*this, code};
   }

   /**
    * \brief
    *    Returns a button styler with the icon placed on the left.
    *
    * \return
    *    A button styler with the icon placed on the left.
    */
   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::gen_icon_placement
   button_styler_gen<Base>::icon_left() const
   {
      return {*this, icon_placement_t::icon_left};
   }

   /**
    * \brief
    *    Returns a button styler with the icon placed on the right.
    *
    * \return
    *    A button styler with the icon placed on the right.
    */
   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::gen_icon_placement
   button_styler_gen<Base>::icon_right() const
   {
      return {*this, icon_placement_t::icon_right};
   }

   /**
    * \brief
    *    Returns a button styler with the label aligned to the left.
    *
    * \return
    *    A button styler with the label aligned to the left.
    */
   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::gen_label_alignment
   button_styler_gen<Base>::align_left() const
   {
      return {*this, label_alignment_t::align_left};
   }

   /**
    * \brief
    *    Returns a button styler with the label aligned to the center.
    *
    * \return
    *    A button styler with the label aligned to the center.
    */
   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::gen_label_alignment
   button_styler_gen<Base>::align_center() const
   {
      return {*this, label_alignment_t::align_center};
   }

   /**
    * \brief
    *    Returns a button styler with the label aligned to the right.
    *
    * \return
    *    A button styler with the label aligned to the right.
    */
   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::gen_label_alignment
   button_styler_gen<Base>::align_right() const
   {
      return {*this, label_alignment_t::align_right};
   }

   /**
    * \brief
    *    Returns a button styler with the specified margin.
    *
    * \param margin
    *    A rect representing the margin of the button.
    *
    * \return
    *    A button styler with the specified margin.
    */
   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::gen_margin
   button_styler_gen<Base>::margin(rect margin) const
   {
      return {*this, margin};
   }

   /**
    * \brief
    *    Returns a button styler with the specified corner radius.
    *
    * \param r
    *    A float representing the corner radius of the button.
    *
    * \return
    *    A button styler with the specified corner radius.
    */
   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::gen_corner_radius
   button_styler_gen<Base>::corner_radius(float r) const
   {
      return {*this, r};
   }

   /**
    * \brief
    *    Returns a button styler with the specified text color.
    *
    * \param color_
    *    A color representing the text color of the button.
    *
    * \return
    *    A button styler with the specified text color.
    */
   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::gen_text_color
   button_styler_gen<Base>::text_color(color color_) const
   {
      return {*this, color_};
   }

   /**
    * \brief
    *    Returns a button styler with the specified top rounded corner
    *    radius.
    *
    * \param r
    *    A float representing the top rounded corner radius of the button.
    *
    * \return
    *    A button styler with the specified top rounded corner radius.
    */
   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::gen_rounded_half_up
   button_styler_gen<Base>::rounded_top(float r) const
   {
      return {*this, r};
   }

   /**
    * \brief
    *    Returns a button styler with the specified bottom rounded corner radius.
    *
    * \param r
    *    A float representing the bottom rounded corner radius of the button.
    *
    * \return
    *    A button styler with the specified bottom rounded corner radius.
    */
   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::gen_rounded_half_down
   button_styler_gen<Base>::rounded_bottom(float r) const
   {
      return {*this, r};
   }

   /**
    * \brief
    *    Returns a button styler with the specified left rounded corner radius.
    *
    * \param r
    *    A float representing the left rounded corner radius of the button.
    *
    * \return
    *    A button styler with the specified left rounded corner radius.
    */
   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::gen_rounded_half_left
   button_styler_gen<Base>::rounded_left(float r) const
   {
      return {*this, r};
   }

   /**
    * \brief
    *    Returns a button styler with the specified right rounded corner radius.
    *
    * \param r
    *    A float representing the right rounded corner radius of the button.
    *
    * \return
    *    A button styler with the specified right rounded corner radius.
    */
   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::gen_rounded_half_right
   button_styler_gen<Base>::rounded_right(float r) const
   {
      return {*this, r};
   }

   /**
    * \brief
    *    Returns a button styler with the default top rounded corner radius.
    *
    * \return
    *    A button styler with the default top rounded corner radius.
    */
   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::gen_rounded_half_default_up
   button_styler_gen<Base>::rounded_top() const
   {
      return {*this};
   }

   /**
    * \brief
    *    Returns a button styler with the default bottom rounded corner radius.
    *
    * \return
    *    A button styler with the default bottom rounded corner radius.
    */
   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::gen_rounded_half_default_down
   button_styler_gen<Base>::rounded_bottom() const
   {
      return {*this};
   }

   /**
    * \brief
    *    Returns a button styler with the default left rounded corner radius.
    *
    * \return
    *    A button styler with the default left rounded corner radius.
    */
   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::gen_rounded_half_default_left
   button_styler_gen<Base>::rounded_left() const
   {
      return {*this};
   }

   /**
    * \brief
    *    Returns a button styler with the default right rounded corner radius.
    *
    * \return
    *    A button styler with the default right rounded corner radius.
    */
   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::gen_rounded_half_default_right
   button_styler_gen<Base>::rounded_right() const
   {
      return {*this};
   }

   /**
    * \brief
    *    Returns a button styler with the specified top-left rounded corner radius.
    *
    * \param r
    *    A float representing the top-left rounded corner radius of the button.
    *
    * \return
    *    A button styler with the specified top-left rounded corner radius.
    */
   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::template gen_rounded_corner<default_button_styler::top_left>
   button_styler_gen<Base>::rounded_corner_top_left(float r) const
   {
      return {*this, r};
   }

   /**
    * \brief
    *    Returns a button styler with the specified top-right rounded corner radius.
    *
    * \param r
    *    A float representing the top-right rounded corner radius of the button.
    *
    * \return
    *    A button styler with the specified top-right rounded corner radius.
    */
   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::template gen_rounded_corner<default_button_styler::top_right>
   button_styler_gen<Base>::rounded_corner_top_right(float r) const
   {
      return {*this, r};
   }

   /**
    * \brief
    *    Returns a button styler with the specified bottom-left rounded corner radius.
    *
    * \param r
    *    A float representing the bottom-left rounded corner radius of the button.
    *
    * \return
    *    A button styler with the specified bottom-left rounded corner radius.
    */
   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::template gen_rounded_corner<default_button_styler::bottom_left>
   button_styler_gen<Base>::rounded_corner_bottom_left(float r) const
   {
      return {*this, r};
   }

   /**
    * \brief
    *    Returns a button styler with the specified bottom-right rounded corner radius.
    *
    * \param r
    *    A float representing the bottom-right rounded corner radius of the button.
    *
    * \return
    *    A button styler with the specified bottom-right rounded corner radius.
    */
   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::template gen_rounded_corner<default_button_styler::bottom_right>
   button_styler_gen<Base>::rounded_corner_bottom_right(float r) const
   {
      return {*this, r};
   }

   /**
    * \brief Returns a button styler with the specified individual corner radii.
    *
    * \param top_left
    *    A float representing the top-left corner radius of the button.
    *
    * \param top_right
    *    A float representing the top-right corner radius of the button.
    *
    * \param bottom_right
    *    A float representing the bottom-right corner radius of the button.
    *
    * \param bottom_left
    *    A float representing the bottom-left corner radius of the button.
    *
    * \return A button styler with the specified individual corner radii.
    */
   template <concepts::ButtonStyler Base>
   inline typename button_styler_gen<Base>::gen_individual_corners
   button_styler_gen<Base>::corner_radius(
         float top_left
       , float top_right
       , float bottom_right
       , float bottom_left
      ) const
   {
      return {*this, top_left, top_right, bottom_right, bottom_left};
   }
}

#endif
