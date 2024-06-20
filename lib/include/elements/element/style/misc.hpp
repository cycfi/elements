/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_MISC_APRIL_11_2016)
#define ELEMENTS_MISC_APRIL_11_2016

#include <elements/element/element.hpp>
#include <elements/element/proxy.hpp>
#include <elements/element/text.hpp>
#include <elements/element/size.hpp>
#include <elements/support/theme.hpp>
#include <infra/support.hpp>

#include <functional>
#include <utility>

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // spacers: empty horizontal or vertical element with specified width or
   // height.
   ////////////////////////////////////////////////////////////////////////////
   [[deprecated("Use vspace(size) instead.")]]
   inline auto vspacer(float size)
   {
      return vsize(size, element{});
   }

   [[deprecated("Use hspace(size) instead.")]]
   inline auto hspacer(float size)
   {
      return hsize(size, element{});
   }

   /**
    * \struct box_element
    * \brief
    *    A simple colored box.
    */
   struct box_element : element
   {
               box_element(color color_);
      void     draw(context const& ctx) override;

      color    _color;
   };

   /**
    * \brief
    *    Constructor for creating a box element with a specific color.
    *
    * \param color_
    *    The color to fill the box with.
    */
   inline box_element::box_element(color color_)
    : _color(color_)
   {}

   inline void box_element::draw(context const& ctx)
   {
      auto& cnv = ctx.canvas;
      cnv.fill_style(_color);
      cnv.fill_rect(ctx.bounds);
   }

   /**
    * \brief
    *    Create a box element with the specified color.
    *
    * \param color_
    *    The color of the box to be created.
    *
    * \return
    *    An instance of box_element with the specified color.
    */
   inline auto box(color color_)
   {
      return box_element{color_};
   }

   /**
    * \struct rbox_element
    * \brief
    *    A simple colored rounded-box.
    */
   struct rbox_element : element
   {
               rbox_element(color color_, float radius = 4);
      void     draw(context const& ctx) override;

      color    _color;
      float    _radius;
   };

   /**
    * \brief
    *    Constructor for creating a rounded box element with a specific
    *    color.
    *
    * \param color_
    *    The color to fill the box with.
    */
   inline rbox_element::rbox_element(color color_, float radius)
      : _color(color_)
      , _radius(radius)
   {}

   inline void rbox_element::draw(context const& ctx)
   {
      auto& cnv = ctx.canvas;
      cnv.begin_path();
      cnv.add_round_rect(ctx.bounds, _radius);
      cnv.fill_style(_color);
      cnv.fill();
   }

   /**
    * \brief
    *    Create a rounded box element with the specified color.
    *
    * \param color_
    *    The color of the box to be created.
    *
    * \return
    *    An instance of rbox_element with the specified color.
    */
   inline auto rbox(color color_, float radius = 4)
   {
      return rbox_element{color_, radius};
   }

   /**
    * \class draw_element
    *
    * \brief
    *    A customizable drawing element that takes in a function that
    *    executes a custom drawing function.
    *
    * \tparam F
    *    The type of the drawing function. This function should match the
    *    signature `void(context const&)`.
    */

   template <typename F>
   class draw_element : public element
   {
   public:

            draw_element(F draw);
      void  draw(context const& ctx);

   private:

      F     _draw;
   };

   /**
    * \brief
    *    Constructor that initializes the draw_element with a drawing
    *    function.
    *
    *    This constructor takes a drawing function. The function will be
    *    called with a drawing context that provides the necessary
    *    information for rendering.
    *
    * \param draw
    *    The drawing function to be used for rendering the element.
    */
   template <typename F>
   inline draw_element<F>::draw_element(F draw)
      : _draw(draw)
   {}

   template <typename F>
   inline void draw_element<F>::draw(context const& ctx)
   {
      _draw(ctx);
   }

   template <typename F>
   [[deprecated("Use draw(F&& _draw) instead.")]]
   inline draw_element<F> basic(F&& _draw)
   {
      return {std::forward<remove_cvref_t<F>>(_draw)};
   }

   /**
    * \brief
    *    Create a draw_element with a custom drawing function.
    *
    *    This function template creates a `draw_element` by wrapping a custom
    *    drawing function. It is useful for creating elements that perform
    *    custom rendering without the need to define a new class. The
    *    provided drawing function should take a single parameter of type
    *    `context const&` and return void.
    *
    * \tparam F
    *    The type of the drawing function. Must be callable with `context
    *    const&`.
    *
    * \param _draw
    *    The custom drawing function to be used by the draw_element.
    *
    * \return
    *    An instance of `draw_element`.
    */
   template <typename F>
   inline draw_element<F> draw(F&& _draw)
   {
      using ftype = remove_cvref_t<F>;
      return {std::forward<ftype>(_draw)};
   }

   /**
    * \class draw_value_element
    * \brief
    *    A drawing element that renders based on a value.
    *
    *    This class template extends `basic_receiver` (see
    *    support/receiver.hpp) to receive values of type `T` and uses a
    *    drawing function of type `F` to render these values.
    *
    * \tparam T
    *    The type of the value to be received and rendered.
    *
    * \tparam F
    *    The type of the drawing function. This function should accept a
    *    `context const&` and a value of type `T` as parameters.
    */
   template <typename T, typename F>
   class draw_value_element : public basic_receiver<T>, public element
   {
   public:

            draw_value_element(F draw);
      void  draw(context const& ctx) override;

   private:

      F     _draw;
      T     _value;
   };

   /**
    * \brief
    *    Constructor that initializes the element with a drawing function.
    *
    *    Initializes a new instance of `draw_value_element` with the
    *    specified drawing function. The drawing function will be called with
    *    the current value and the drawing context whenever the element needs
    *    to be rendered.
    *
    * \param draw
    *    The drawing function to be used for rendering the value.
    */
   template <typename T, typename F>
   inline draw_value_element<T, F>::draw_value_element(F draw)
    : _draw(draw)
   {}

   template <typename T, typename F>
   inline void draw_value_element<T, F>::draw(context const& ctx)
   {
      _draw(ctx, this->value());
   }

   /**
    * \brief
    *    Creates a `draw_value_element` with the specified drawing function.
    *
    * \tparam T
    *    The type of the value to be rendered.
    *
    * \tparam F
    *    The type of the drawing function.
    *
    * \param f
    *    The drawing function to be used by the draw_value_element.
    *
    * \return
    *    An instance of `draw_value_element`.
    */
   template <typename T, typename F>
   inline draw_value_element<T, remove_cvref_t<F>> draw_value(F&& f)
   {
      using ftype = remove_cvref_t<F>;
      return {std::forward<ftype>(f)};
   }

   /**
    * \class panel
    * \brief
    *    A panel element with customizable opacity.
    *
    * \param opacity_
    *    The opacity level of the panel. A float value where 0.0 is fully
    *    transparent and 1.0 is fully opaque.
    */
   class panel : public element
   {
   public:
                     panel(float opacity_ = get_theme().panel_color.alpha);
      void           draw(context const& ctx) override;

   private:

      float          _opacity;
   };

   /**
    * \brief
    *    Constructor for creating a panel with a specific opacity.
    *
    * \param opacity_
    *    The opacity level for the panel. A float value where 0.0 is fully
    *    transparent and 1.0 is fully opaque.
    */
   inline panel::panel(float opacity_)
    : _opacity(opacity_)
   {}

   /**
    * \struct frame
    * \brief
    *    A frame element that renders a frame.
    */
   struct frame : public element
   {
      void           draw(context const& ctx) override;
   };

   /**
    * \class title_bar
    * \brief
    *    A title bar element, minus the actual title. This may be used as the
    *    backdrop for child-window's title bars.
    */
   class title_bar : public element
   {
   public:

      void           draw(context const& ctx) override;
   };

   inline void title_bar::draw(context const& ctx)
   {
      draw_box_vgradient(ctx.canvas, ctx.bounds, 4.0);
   }

   /**
    * \struct icon
    *
    * \brief
    *    An icon element designed to render an icon based on a code that
    *    represents a specific glyph in a font or icon set. The size of the
    *    icon can be adjusted, allowing for flexible presentation in various
    *    UI contexts.
    *
    * \param code_
    *    The unicode code point of the icon to be rendered corresponding to a
    *    specific glyph in a font or icon set.
    *
    * \param size_
    *    The size multiplier adjusts an icon's size relative to its default.
    *    A multiplier of 1.0 keeps the icon at its standard size, while
    *    values above or below 1.0 proportionally increase or decrease its
    *    size.
    */
   struct icon : element
   {
                     icon(std::uint32_t code_, float size_ = 1.0);

      view_limits    limits(basic_context const& ctx) const override;
      void           draw(context const& ctx) override;

      std::uint32_t  _code;
      float          _size;
   };
}

#endif
