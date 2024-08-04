/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_PORT_APRIL_24_2016)
#define ELEMENTS_PORT_APRIL_24_2016

#include <elements/element/proxy.hpp>
#include <infra/support.hpp>
#include <memory>

namespace cycfi::elements
{
   //--------------------------------------------------------------------------
   // Port base
   //--------------------------------------------------------------------------

   /**
    * @class port_base
    *
    * \brief
    *    Base class for port elements, representing windows over large areas.
    *
    *    `port_base` serves as an abstract base class for port elements
    *    within a UI framework, where a port is conceptualized as a window or
    *    viewport over a larger area.
    *
    *    Alignment determines which part of the underlying area is visible
    *    through the port, with both horizontal and vertical alignment values
    *    ranging from 0.0 to 1.0. An alignment of 0.0 positions the port to
    *    the left or top of the area, respectively, while an alignment of 1.0
    *    shifts the port to the right or bottom.
    */
   class port_base : public proxy_base
   {
   public:

      void                    draw(context const& ctx) override;

      virtual double          halign() const = 0;
      virtual void            halign(double val) = 0;
      virtual double          valign() const = 0;
      virtual void            valign(double val) = 0;
   };

   rect get_port_bounds(context const& ctx);

   //--------------------------------------------------------------------------
   // Port element
   //--------------------------------------------------------------------------

   /**
    * @class port_element
    *
    * \brief
    *    A port element with alignment capabilities, extending `port_base`.
    *    This class models supports horizontal and vertical alignment.
    */
   class port_element : public port_base
   {
   public:
                              port_element();

      view_limits             limits(basic_context const& ctx) const override;
      void                    prepare_subject(context& ctx) override;

      double                  halign() const override;
      void                    halign(double val) override;
      double                  valign() const override;
      void                    valign(double val) override;

   private:

      double                  _halign;
      double                  _valign;
   };

   template <concepts::Element Subject>
   inline proxy<remove_cvref_t<Subject>, port_element>
   port(Subject&& subject);

   //--------------------------------------------------------------------------
   // Vertical port
   //--------------------------------------------------------------------------

   /**
    * @class vport_element
    *
    * \brief
    *    A vertical port element that extends `port_base`, specifically
    *    designed for vertical alignment within a port.
    *
    *    This class represents a vertical viewport or window over a larger
    *    area, allowing content to be aligned vertically.
    */
   class vport_element : public port_base
   {
   public:
                              vport_element();

      view_limits             limits(basic_context const& ctx) const override;
      void                    prepare_subject(context& ctx) override;

      double                  halign() const override          { return 0; }
      void                    halign(double /*val*/) override  {}
      double                  valign() const override          { return _valign; }
      void                    valign(double val) override      { _valign = val; }

   private:

      double                  _valign;
   };

   template <concepts::Element Subject>
   inline proxy<remove_cvref_t<Subject>, vport_element>
   vport(Subject&& subject);

   //--------------------------------------------------------------------------
   // Horizontal port
   //--------------------------------------------------------------------------

   /**
    * @class hport_element
    *
    * \brief
    *    A horizontal port element that extends `port_base`, specifically
    *    designed for horizontal alignment within a port.
    *
    *    This class represents a horizontal viewport or window over a larger
    *    area, allowing content to be aligned horizontally.
    */
   class hport_element : public port_base
   {
   public:
                              hport_element();

      view_limits             limits(basic_context const& ctx) const override;
      void                    prepare_subject(context& ctx) override;

      double                  halign() const override             { return _halign; }
      void                    halign(double val) override         { _halign = val; }
      double                  valign() const override             { return 0; }
      void                    valign(double /*val*/) override     {}

   private:

      double                  _halign;
   };

   template <concepts::Element Subject>
   inline proxy<remove_cvref_t<Subject>, hport_element>
   hport(Subject&& subject);

   //--------------------------------------------------------------------------
   // Scrollers
   //--------------------------------------------------------------------------

   /**
    * @class scrollable
    *
    * \brief
    *    Mixin class for an element that is scrollable.
    *
    *    Provides functionality to scroll a given rectangle into view within
    *    the context of a scrollable area.
    */
   class scrollable
   {
   public:

      struct scrollable_context;

      virtual                    ~scrollable() = default;
      virtual bool               scroll_into_view(context const& ctx, rect r) = 0;
      static scrollable_context  find(context const& ctx);
   };

   /**
    * @struct scrollable::scrollable_context
    *
    * \brief
    *    The context within which a scrollable element operates.
    */
   struct scrollable::scrollable_context
   {
      context const* context_ptr;
      scrollable*    scrollable_ptr;

      bool           scroll_into_view(rect r_);
   };

   /**
    * @enum ScrollbarFlags
    * \brief Flags to control the visibility and behavior of scrollbars.
    * @var no_scrollbars:  Hides all scrollbars.
    * @var no_hscroll:     Disables horizontal scrolling.
    * @var no_vscroll:     Disables vertical scrolling.
    */
   enum
   {
      no_scrollbars  = 1,
      no_hscroll     = 1 << 1,
      no_vscroll     = 1 << 2
   };

   //--------------------------------------------------------------------------
   // Scroller base
   //--------------------------------------------------------------------------

   /**
    * @class scroller_base
    *
    * \brief
    *    Base proxy class for elements that are scrollable.
    *
    *    Inherits from `port_element` and `scrollable` to provide a
    *    foundation for elements that require scrollable functionality.
    *
    *    The `scroller_base` class also manages the visibility and behavior
    *    of horizontal and vertical scrollbars based on the traits specified
    *    upon construction. It supports dynamic adjustment of content
    *    positioning within the scrollable area.
    */
   class scroller_base : public port_element, public scrollable
   {
   public:

      explicit                scroller_base(int traits = 0);

      view_limits             limits(basic_context const& ctx) const override;
      void                    prepare_subject(context& ctx) override;
      element*                hit_test(context const& ctx, point p, bool leaf, bool control) override;
      void                    draw(context const& ctx) override;

      bool                    wants_control() const override;
      bool                    click(context const& ctx, mouse_button btn) override;
      void                    drag(context const& ctx, mouse_button btn) override;
      bool                    scroll(context const& ctx, point dir, point p) override;
      bool                    scroll_into_view(context const& ctx, rect r) override;
      bool                    cursor(context const& ctx, point p, cursor_tracking status) override;
      bool                    key(context const& ctx, key_info k) override;

      using scroll_callback_f = std::function<void(point p)>;

      scroll_callback_f       on_scroll = [](point){};

      void                    set_alignment(point p);
                              [[deprecated("use set_alignment(p) instead")]]
      void                    set_position(point p) { set_alignment(p); }

      struct scrollbar_info;

      virtual void            draw_scroll_bar(context const& ctx, scrollbar_info const& info, point mp);
      virtual rect            scroll_bar_position(context const& ctx, scrollbar_info const& info);

   private:

      struct scrollbar_bounds
      {
         rect vscroll_bounds;
         rect hscroll_bounds;
         bool has_v = false;
         bool has_h = false;
      };

      enum tracking_status
      {
         none,
         start,
         tracking_v,
         tracking_h
      };

      scrollbar_bounds  get_scrollbar_bounds(context const& ctx);
      bool              reposition(context const& ctx, point p);

      bool              has_scrollbars() const { return !(_traits & no_scrollbars); }
      bool              allow_hscroll() const { return !(_traits & no_hscroll); }
      bool              allow_vscroll() const { return !(_traits & no_vscroll); }

      point             _offset;
      tracking_status   _tracking;
      int               _traits;
   };

   //--------------------------------------------------------------------------
   // Scroller Info
   //--------------------------------------------------------------------------

   /**
    * @struct scrollbar_info
    *
    * \brief
    *    Information about a scrollbar.
    *
    * @var pos
    *    The current position of the scrollbar represented as an alignment
    *    fraction from 0.0 to 1.0.
    *
    * @var extent
    *    The size of the content area.
    *
    * @var bounds
    *    The geometric bounds of the scrollbar within the scroller. This
    *    `rect` defines the position and size of the scrollbar.
    */
   struct scroller_base::scrollbar_info
   {
      double   pos;
      float    extent;
      rect     bounds;
   };

   //--------------------------------------------------------------------------
   // Scroller creation functions
   //--------------------------------------------------------------------------

   template <concepts::Element Subject>
   inline proxy<remove_cvref_t<Subject>, scroller_base>
   scroller(Subject&& subject, int traits = 0);

   template <concepts::Element Subject>
   inline proxy<remove_cvref_t<Subject>, scroller_base>
   vscroller(Subject&& subject, int traits = 0);

   template <concepts::Element Subject>
   inline proxy<remove_cvref_t<Subject>, scroller_base>
   hscroller(Subject&& subject, int traits = 0);

   //--------------------------------------------------------------------------
   // Inlines
   //--------------------------------------------------------------------------

   /**
    * \brief
    *    Creates a proxy object that wraps a given UI element, allowing it to
    *    be displayed within a port.
    *
    *    This template function wraps a UI element (`subject`) in a
    *    `port_element`, enabling the element to be displayed as part of a
    *    port. A port is a window or viewport over a larger area, and
    *    elements within a port can be aligned and scrolled within this area.
    *
    * \tparam Subject
    *    The type of the UI element to be wrapped. Must meet the requirements
    *    of the `concepts::Element` concept.
    *
    * \param subject
    *    The UI element to be wrapped in the port.
    *
    * \return
    *    A proxy object that wraps the specified UI element in a
    *    `port_element`.
    */
   template <concepts::Element Subject>
   inline proxy<remove_cvref_t<Subject>, port_element>
   port(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }

   /**
    * \brief
    *    Default constructor for the port_element class.
    */
   inline port_element::port_element()
    : _halign(0.0)
    , _valign(0.0)
   {}

   /**
    * \brief
    *    Get the current horizontal alignment of the port element.
    *
    * \return
    *    The current horizontal alignment value, ranging from 0.0 to 1.0.
    */
   inline double port_element::halign() const
   {
      return _halign;
   }

   /**
    * \brief
    *    Set the horizontal alignment of the port element.
    *
    * \param val
    *    The new horizontal alignment value, ranging from 0.0 to 1.0.
    */
   inline void port_element::halign(double val)
   {
      _halign = val;
   }

   /**
    * \brief
    *    Get the current vertical alignment of the port element.
    *
    * \return
    *    The current vertical alignment value, ranging from 0.0 to 1.0.
    */
   inline double port_element::valign() const
   {
      return _valign;
   }

   /**
    * \brief
    *    Set the vertical alignment of the port element.
    *
    * \param val
    *    The new vertical alignment value, ranging from 0.0 to 1.0.
    */
   inline void port_element::valign(double val)
   {
      _valign = val;
   }

   /**
    * \brief
    *    Constructs a `vport_element`.
    */
   inline vport_element::vport_element()
    : _valign(0.0)
   {
   }

   /**
    * \brief
    *    Creates a vertical port element that wraps a given UI element,
    *    allowing it to be displayed within a vertical port.
    *
    *    This function template creates a `vport_element` that wraps the
    *    given UI element (`subject`), enabling the element to be displayed
    *    as part of a vertical port. A vertical port is a viewport that
    *    allows vertical scrolling and alignment within a larger area.
    *
    * \tparam Subject
    *    The type of the UI element to be wrapped. Must meet the requirements
    *    of the `concepts::Element` concept.
    *
    * \param subject
    *    The UI element to be wrapped in the vertical port.
    *
    * \return
    *    A proxy's subject that wraps the specified UI element in a
    *    `vport_element`.
    */
   template <concepts::Element Subject>
   inline proxy<remove_cvref_t<Subject>, vport_element>
   vport(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }

   /**
    * \brief
    *    Constructs an `hport_element`.
    */
   inline hport_element::hport_element()
    : _halign(0.0)
   {}

   /**
    * \brief
    *    Creates a horizontal port element that wraps a given UI element,
    *    allowing it to be displayed within a horizontal port.
    *
    *    This function template creates a `hport_element` that wraps the
    *    given UI element (`subject`), enabling the element to be displayed
    *    as part of a horizontal port. A horizontal port is a viewport that
    *    allows horizontal scrolling and alignment within a larger area.
    *
    * \tparam Subject
    *    The type of the UI element to be wrapped. Must meet the requirements
    *    of the `concepts::Element` concept.
    *
    * \param subject
    *    The UI element to be wrapped in the horizontal port.
    *
    * \return
    *    A proxy's subject that wraps the specified UI element in a
    *    `hport_element`.
    */
   template <concepts::Element Subject>
   inline proxy<remove_cvref_t<Subject>, hport_element>
   hport(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }

   /**
    * \brief
    *    Attempts to scroll the rectangle, r_, into view.
    *
    *    If both `scrollable_ptr` and `context_ptr` are valid, it calls the
    *    `scroll_into_view` member function of the scrollable object to
    *    attempt to scroll the rectangle into view.
    *
    * \param r_
    *    The rectangle to scroll into view.
    *
    * \return
    *    True if the operation was successful, false otherwise.
    */
   inline bool scrollable::scrollable_context::scroll_into_view(rect r_)
   {
      if (scrollable_ptr && context_ptr)
      {
         rect r = r_;
         return scrollable_ptr->scroll_into_view(*context_ptr, r);
      }
      return false;
   }

   /**
    * \brief
    *    Constructor for scroller_base.
    *
    * \param traits
    *    Configuration traits for the scroller, controlling its behavior and
    *    appearance.
    */
   inline scroller_base::scroller_base(int traits)
     : _tracking(none)
     , _traits(traits)
   {}

   /**
    * \brief
    *    Creates a generic scroller.
    *
    *    Constructs a proxy object wrapping a given subject with
    *    scroller_base, allowing the subject to be scrolled both horizontally
    *    and vertically, depending on the specified traits.
    *
    * \tparam Subject
    *    The type of the subject to be wrapped. Must satisfy the
    *    concepts::Element concept.
    *
    * \param subject
    *    The subject to be wrapped in a scroller.
    *
    * \param traits
    *    Configuration traits for the scroller.
    *
    * \return
    *    A proxy object wrapping the subject with scroller_base
    *    functionality.
    */
   template <concepts::Element Subject>
   inline proxy<remove_cvref_t<Subject>, scroller_base>
   scroller(Subject&& subject, int traits)
   {
      return {std::forward<Subject>(subject), traits};
   }

   /**
    * \brief
    *    Creates a vertical scroller.
    *
    *    Constructs a proxy object wrapping a given subject with
    *    scroller_base, specifically for vertical scrolling.
    *
    * \tparam Subject
    *    The type of the subject to be wrapped. Must satisfy the
    *    concepts::Element concept.
    *
    * \param subject
    *    The subject to be wrapped in a vertical scroller.
    *
    * \param traits
    *    Configuration traits for the scroller, with horizontal scrolling
    *    disabled.
    *
    * \return
    *    A proxy object wrapping the subject with vertical scroller_base
    *    functionality.
    */
   template <concepts::Element Subject>
   inline proxy<remove_cvref_t<Subject>, scroller_base>
   vscroller(Subject&& subject, int traits)
   {
      return {std::forward<Subject>(subject), traits | no_hscroll};
   }

   /**
    * \brief
    *    Creates a horizontal scroller.
    *
    *    Constructs a proxy object wrapping a given subject with
    *    scroller_base, specifically for horizontal scrolling.
    *
    * \tparam Subject
    *    The type of the subject to be wrapped. Must satisfy the
    *    concepts::Element concept.
    *
    * \param subject
    *    The subject to be wrapped in a horizontal scroller.
    *
    * \param traits
    *    Configuration traits for the scroller, with vertical scrolling
    *    disabled.
    *
    * \return
    *    A proxy object wrapping the subject with horizontal scroller_base
    *    functionality.
    */
   template <concepts::Element Subject>
   inline proxy<remove_cvref_t<Subject>, scroller_base>
   hscroller(Subject&& subject, int traits)
   {
      return {std::forward<Subject>(subject), traits | no_vscroll};
   }
}

#endif
