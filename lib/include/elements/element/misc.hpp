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
    * \struct border
    * \brief
    *    A generic border element that renders a border around the bounds of
    *    the element.
    */
   struct border : public element
   {
      void           draw(context const& ctx) override;
   };

   /**
    * \struct border_left
    *
    * \brief
    *    A generic border element that renders a border along the left side
    *    of the element's bounds.
    */
   struct border_left : public element
   {
      void           draw(context const& ctx) override;
   };

   /**
    * \struct border_right
    *
    * \brief
    *    A generic border element that renders a border along the right side
    *    of the element's bounds.
    */
   struct border_right : public element
   {
      void           draw(context const& ctx) override;
   };

   /**
    * \struct border_top
    *
    * \brief
    *    A generic border element that renders a border along the top side of
    *    the element's bounds.
    */
   struct border_top : public element
   {
      void           draw(context const& ctx) override;
   };

   /**
    * \struct border_bottom
    *
    * \brief
    *    A generic border element that renders a border along the bottom side
    *    of the element's bounds.
    */
   struct border_bottom : public element
   {
      void           draw(context const& ctx) override;
   };

   /**
    * \struct border_bottom
    *
    * \brief
    *    A generic border element that renders a border along the top and
    *    bottom sides of the element's bounds.
    */
   struct border_top_bottom : public element
   {
      void           draw(context const& ctx) override;
   };

   /**
    * \struct border_left_right
    *
    * \brief
    *    A generic border element that renders a border along the left and
    *    right sides of the element's bounds.
    */
   struct border_left_right : public element
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

   ////////////////////////////////////////////////////////////////////////////
   // Grid Lines - Vertical
   ////////////////////////////////////////////////////////////////////////////
   class vgrid_lines : public element
   {
   public:

                     vgrid_lines(float major_divisions, float minor_divisions)
                      : _major_divisions(major_divisions)
                      , _minor_divisions(minor_divisions)
                     {}

      void           draw(context const& ctx) override;

   private:

      float          _major_divisions;
      float          _minor_divisions;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Grid Lines - Horizontal
   ////////////////////////////////////////////////////////////////////////////
   class hgrid_lines : public element
   {
   public:

                     hgrid_lines(float major_divisions, float minor_divisions)
                      : _major_divisions(major_divisions)
                      , _minor_divisions(minor_divisions)
                     {}

      void           draw(context const& ctx) override;

   private:

      float          _major_divisions;
      float          _minor_divisions;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Icons
   ////////////////////////////////////////////////////////////////////////////
   struct icon : element
   {
                     icon(std::uint32_t code_, float size_ = 1.0);

      view_limits    limits(basic_context const& ctx) const override;
      void           draw(context const& ctx) override;

      std::uint32_t  _code;
      float          _size;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Key Intercept
   ////////////////////////////////////////////////////////////////////////////
   template <concepts::Element Subject>
   struct key_intercept_element : public proxy<Subject>
   {
      using base_type = proxy<Subject>;

                     key_intercept_element(Subject subject)
                      : base_type(std::move(subject))
                     {}

      bool           key(context const& ctx, key_info k) override;
      bool           wants_control() const override { return true; }
      bool           wants_focus() const override { return true; }

      using key_function = std::function<bool(key_info k)>;

      key_function   on_key = [](auto){ return false; };
   };

   template <concepts::Element Subject>
   inline key_intercept_element<remove_cvref_t<Subject>>
   key_intercept(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }

   template <concepts::Element Subject>
   inline bool key_intercept_element<Subject>::key(context const& ctx, key_info k)
   {
      if (on_key(k))
         return true;
      return this->subject().key(ctx, k);
   }

   ////////////////////////////////////////////////////////////////////////////
   // Text Intercept
   ////////////////////////////////////////////////////////////////////////////
   template <concepts::Element Subject>
   struct text_intercept_element : public proxy<Subject>
   {
      using base_type = proxy<Subject>;

                     text_intercept_element(Subject subject)
                      : base_type(std::move(subject))
                     {}

      bool           text(context const& ctx, text_info info) override;
      bool           wants_control() const override { return true; }
      bool           wants_focus() const override { return true; }

      using text_function = std::function<bool(text_info info)>;

      text_function   on_text = [](auto){ return false; };
   };

   template <concepts::Element Subject>
   inline text_intercept_element<remove_cvref_t<Subject>>
   text_intercept(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }

   template <concepts::Element Subject>
   inline bool text_intercept_element<Subject>::text(context const& ctx, text_info info)
   {
      if (on_text(info))
         return true;
      return this->subject().text(ctx, info);
   }

   ////////////////////////////////////////////////////////////////////////////
   // Click Intercept
   ////////////////////////////////////////////////////////////////////////////
   template <concepts::Element Subject>
   struct click_intercept_element : public proxy<Subject>
   {
      using base_type = proxy<Subject>;

                     click_intercept_element(Subject subject)
                      : base_type(std::move(subject))
                     {}

      bool           click(context const& ctx, mouse_button btn) override;
      bool           wants_control() const override { return true; }
      bool           wants_focus() const override { return true; }

      using click_function = std::function<bool(mouse_button btn)>;

      click_function on_click = [](auto){ return false; };
   };

   template <concepts::Element Subject>
   inline click_intercept_element<remove_cvref_t<Subject>>
   click_intercept(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }

   template <concepts::Element Subject>
   inline bool click_intercept_element<Subject>::click(context const& ctx, mouse_button btn)
   {
      if (on_click(btn))
         return true;
      return this->subject().click(ctx, btn);
   }

   ////////////////////////////////////////////////////////////////////////////
   // Hidable
   ////////////////////////////////////////////////////////////////////////////
   template <concepts::Element Subject>
   class hidable_element : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                              hidable_element(Subject subject);
      void                    draw(context const& ctx) override;
      bool                    wants_control() const override;
      bool                    wants_focus() const override;
      bool                    is_hidden = false;
   };

   template <concepts::Element Subject>
   inline hidable_element<Subject>::hidable_element(Subject subject)
    : base_type(std::move(subject))
   {}

   template <concepts::Element Subject>
   inline void hidable_element<Subject>::draw(context const& ctx)
   {
      if (!is_hidden)
         this->subject().draw(ctx);
   }

   template <concepts::Element Subject>
   inline bool hidable_element<Subject>::wants_control() const
   {
      if (is_hidden)
         return false;
      return this->subject().wants_control();
   }

   template <concepts::Element Subject>
   inline bool hidable_element<Subject>::wants_focus() const
   {
      if (is_hidden)
         return false;
      return this->subject().wants_focus();
   }

   template <concepts::Element Subject>
   inline hidable_element<remove_cvref_t<Subject>>
   hidable(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }

   ////////////////////////////////////////////////////////////////////////////
   // Vertical collapsable
   ////////////////////////////////////////////////////////////////////////////
   template <concepts::Element Subject>
   class vcollapsable_element : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                              vcollapsable_element(Subject subject);
      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;
      bool                    wants_control() const override;
      bool                    wants_focus() const override;
      bool                    is_collapsed = false;
   };

   template <concepts::Element Subject>
   inline vcollapsable_element<Subject>::vcollapsable_element(Subject subject)
    : base_type(std::move(subject))
   {}

   template <concepts::Element Subject>
   inline view_limits
   vcollapsable_element<Subject>::limits(basic_context const& ctx) const
   {
      auto e_limits = this->subject().limits(ctx);
      return is_collapsed?
         view_limits{{e_limits.min.x, 0.0f}, {e_limits.max.x, 0.0f}} :
         e_limits;
   }

   template <concepts::Element Subject>
   inline void vcollapsable_element<Subject>::draw(context const& ctx)
   {
      if (!is_collapsed)
         this->subject().draw(ctx);
   }

   template <concepts::Element Subject>
   inline bool vcollapsable_element<Subject>::wants_control() const
   {
      if (is_collapsed)
         return false;
      return this->subject().wants_control();
   }

   template <concepts::Element Subject>
   inline bool vcollapsable_element<Subject>::wants_focus() const
   {
      if (is_collapsed)
         return false;
      return this->subject().wants_focus();
   }

   template <concepts::Element Subject>
   inline vcollapsable_element<remove_cvref_t<Subject>>
   vcollapsable(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }

   ////////////////////////////////////////////////////////////////////////////
   // Modal element hugs the UI and prevents any event from passing through
   ////////////////////////////////////////////////////////////////////////////
   template <concepts::Element Subject>
   class modal_element : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                     modal_element(Subject subject);
      element*       hit_test(context const& ctx, point p, bool leaf, bool control) override;
      bool           click(context const& ctx, mouse_button btn) override;
      bool           key(context const& ctx, key_info k) override;
      bool           text(context const& ctx, text_info info) override;

      bool           wants_focus() const override { return true; }
      bool           wants_control() const override { return true; }
   };

   template <concepts::Element Subject>
   inline modal_element<remove_cvref_t<Subject>>
   modal(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }

   template <concepts::Element Subject>
   inline modal_element<Subject>::modal_element(Subject subject)
    : base_type(std::move(subject))
   {
   }

   template <concepts::Element Subject>
   inline element* modal_element<Subject>::hit_test(context const& ctx, point p, bool leaf, bool control)
   {
      if (auto e = this->subject().hit_test(ctx, p, leaf, control))
         return e;
      return this;
   }

   template <concepts::Element Subject>
   inline bool modal_element<Subject>::click(context const& ctx, mouse_button btn)
   {
      this->subject().click(ctx, btn);
      return true;
   }

   template <concepts::Element Subject>
   inline bool modal_element<Subject>::key(context const& ctx, key_info k)
   {
      base_type::key(ctx, k);
      return true;
   }

   template <concepts::Element Subject>
   inline bool modal_element<Subject>::text(context const& ctx, text_info info)
   {
      base_type::text(ctx, info);
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////
   // An element that prevents any event from passing through. Add this as a
   // topmost layer in a view to lock the UI.
   ////////////////////////////////////////////////////////////////////////////
   inline auto ui_block(color color_ = {0.0, 0.0, 0.0, 0.5})
   {
      return modal(box_element{color_});
   }
}

#endif
