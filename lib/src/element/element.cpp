/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/element.hpp>
#include <elements/element/traversal.hpp>
#include <elements/element/composite.hpp>
#include <elements/support.hpp>
#include <elements/view.hpp>
#include <typeinfo>

#if !defined(_MSC_VER) && (defined(__GNUC__) || defined(__clang__))
# include <cxxabi.h>

   static std::string demangle(const char* name)
   {
      int status = 0;
      std::unique_ptr<char, void(*)(void*)> res{
            abi::__cxa_demangle(name, nullptr, nullptr, &status),
            std::free
         };

      return status==0? res.get() : name;
   }
#else
   static std::string demangle(const char* name)
   {
      return name;
   }
#endif

namespace cycfi { namespace elements
{
   /**
    * \brief
    *    Determines the minimum and maximum extents of an element.
    *
    *    This function returns the maximum and minimum size of the element's
    *    content and set constraints.
    *
    * \param ctx
    *    A reference to the basic_context of the element, which provides
    *    access to the current view and canvas. The context can be used in
    *    calculating size limits of the element.
    *
    * \return
    *    Returns the view_limits of the element, which encapsulate the
    *    minimum and maximum dimensions. The minimum size is (0.0, 0.0),
    *    indicating no lower bound constraint by default. The maximum size is
    *    effectively unlimited, represented by a large float value,
    *    `full_limits`, suggesting that the element can expand as much as
    *    needed unless explicitly constrained.
    */
   view_limits element::limits(basic_context const& /* ctx */) const
   {
      return full_limits;
   }

   /**
    * \brief
    *    Determines how much an element can stretch horizontally and
    *    vertically relative to other elements in a column or row.
    *
    *    This function provides the stretch factors along the horizontal and
    *    vertical axes, which determine how the element will expand or
    *    contract relative to the available space in its layout. The stretch
    *    factors influence how an element absorbs extra space or shrinks when
    *    necessary.
    *
    *    If a row (or column) has more space allocated to it than the minimum
    *    required by all elements in that row (or column), as specified by
    *    the sum of all the minimum limits of each of the elements, then
    *    resizable elements are stretched to fit the allocated space. The
    *    value returned by stretch dictates how much that element can
    *    stretch. The default is 1.0. A stretch value of 2.0 means that the
    *    element is twice as stretchable compared to all other elements in
    *    that row (or column), assuming the others have the default 1.0
    *    stretch value.
    *
    * \return
    *    Returns a view_stretch structure containing the stretch factors. By
    *    default, both horizontal and vertical stretch factors are set to
    *    1.0f, indicating uniform stretching behavior in both dimensions.
    */
   view_stretch element::stretch() const
   {
      return {1.0f, 1.0f};
   }

   /**
    * \brief
    *    Retrieves the span count of the element.
    *
    *    This function returns the number of discrete units or positions the
    *    element occupies within a layout. The span count can influence
    *    layout decisions, particularly in grid-based or flex layouts where
    *    multiple elements share and divide space.
    *
    * \return
    *    Returns the span count as an unsigned integer. The default value is
    *    1, indicating that the element occupies a single unit or position
    *    within the layout.
    */
   unsigned element::span() const
   {
      return 1;
   }

   /**
    * \brief
    *    Tests if a given point is within the element's bounds.
    *
    *    This function checks whether a specified point is contained within
    *    the boundaries of the element. If the point is within the bounds, it
    *    returns a pointer to this element; otherwise, it returns nullptr.
    *
    * \param ctx
    *    A constant reference to the context associated with this element,
    *    which includes the current view and canvas settings, and the bounds
    *    defining the active drawing area. The context also holds a reference
    *    to the parent context, which can be useful for hierarchical hit
    *    testing.
    *
    * \param p
    *    The point to test, typically representing a cursor or touch
    *    location.
    *
    * \param leaf
    *    A boolean indicating whether to test only leaf nodes. If true,
    *    return the deepest leaf node hit (innermost), otherwise, return the
    *    first element hit in the hierarchy (outermost). In this context,
    *    "innermost" or "deepest" refers to the leaf node that is furthest
    *    down in the hierarchy, while "outermost" refers to the first element
    *    hit, which is higher up in the hierarchy.
    *
    * \param control
    *    boolean indicating whether to consider control elements
    *    specifically. If true, filter only control elements.
    *
    * \return
    *    Returns a pointer to this element if the point is within its bounds,
    *    otherwise nullptr. This allows determining which element, if any,
    *    the point interacts with.
    */
   element* element::hit_test(context const& ctx, point p, bool /*leaf*/, bool /*control*/)
   {
      return (ctx.bounds.includes(p)) ? this : nullptr;
   }

   /**
    * \brief
    *    Handles the drawing of an element given the provided context.
    *
    *    The context's bounds member specifies where the element should be
    *    drawn. The element should use the context's canvas member for
    *    drawing. The draw member function should not be called directly or
    *    indirectly from other member functions, except from a parent's draw
    *    function, drawing its children. If you need to redraw an element
    *    from other member functions, call the refresh member function
    *    instead.
    *
    *    This member function is responsible for drawing the element on the
    *    screen. It should be overridden in derived classes to provide
    *    specific drawing functionality.
    *
    * \param ctx
    *    The context within which the element should be drawn. The context
    *    provides information about the view, canvas, and other properties
    *    that might be necessary for drawing the element.
    */
   void element::draw(context const& /* ctx */)
   {
   }

   /**
    * \brief
    *    Updates the layout of the element based on the given context.
    *
    *    This member function is designed to adjust the positioning and
    *    sizing of the element and its children (for composite elements)
    *    according to the layout rules defined for it. It is called whenever
    *    the view changes size or when an explicit request is made for a full
    *    layout, such as when the elements hierarchy changes. While this base
    *    implementation does nothing, derived classes should override this
    *    member function to implement specific layout behaviors such as
    *    repositioning child elements or resizing based on the dimensions
    *    provided by the context.
    *
    *    Some elements may make use of the layout member function if it needs
    *    to update information necessary for drawing. For example, composite
    *    elements, those that have children elements, may cache information
    *    about the coordinates of sub elements. The layout member function is
    *    called, prior to calling draw, to give the element a chance to
    *    prepare its children for drawing. The context's bounds member
    *    specifies the outermost bounds of the element. The composite element
    *    subdivides this area as appropriate for each of its children.
    *
    * \param ctx
    *    A constant reference to the context, which provides environmental
    *    settings and constraints that may influence the layout. The context
    *    includes details such as bounding dimensions and device
    *    characteristics that are crucial for correct element placement and
    *    sizing.
    */
   void element::layout(context const& /* ctx */)
   {
   }

   /**
    * \brief
    *    Send a refresh request for a particular element to redraw it.
    *
    *    If the element 'e' is the same as 'this' element, the function
    *    signals the view in the supplied context to refresh according to the
    *    'outward' parameter.
    *
    *    This function essentially invalidates the bounds of the element and
    *    triggers a subsequent redraw of that area in the window. The outward
    *    argument specifies which of the element's parents should be
    *    refreshed. For example, if a margin element encloses an image
    *    element, and you want to refresh the image, including its margin,
    *    then specify an outward argument of 1. The default outward argument
    *    is 0.
    *
    * \param ctx
    *    Represents the current context, providing access to the current view
    *    and the canvas and contains information for element placement and
    *    sizing, including bounding dimensions.
    *
    * \param e
    *    Element to be refreshed.
    *
    * \param outward
    *    Determines the extent of outward refreshing for the view. An outward
    *    parameter of 0 will refresh only the element. An outward of 1 (the
    *    default) will include the element and its parent.
    */
   void element::refresh(context const& ctx, element& e, int outward)
   {
      if (&e == this)
         ctx.view.refresh(ctx, outward);
   }

   /**
    * \brief
    *    Executes a function within the given context.
    *
    *    This function provides a way to encapsulate a behaviour or action
    *    that needs to be performed on a given element, `e`, when its proper
    *    context is found. The element hierarchy is traversed until we find
    *    the match for the element `e` at which point the function `f(ctx)`
    *    is invoked.
    *
    * \note
    *    context_function is declared as:
    *    using context_function = std::function<void(context const&)>;
    *
    * \param ctx
    *    The context within which the function should be executed.
    *
    * \param e
    *    The element that should match 'this' for the function to be
    *    executed.
    *
    * \param f
    *    The function to be executed if 'e' matches 'this' object.
    */
   void element::in_context_do(context const& ctx, element& e, context_function f)
   {
      if (&e == this)
         f(ctx);
   }

   /**
    * \brief
    *    Handles a click event.
    *
    *    This member function is called when the element is clicked. It
    *    should be overridden in derived classes to provide specific
    *    functionality.
    *
    * \param ctx
    *    The context within which the click event occurred. This parameter is
    *    currently unused.
    *
    * \param btn
    *    The mouse button that was clicked. This parameter is currently
    *    unused.
    *
    * \return
    *    false by default, indicating that the click event was not handled.
    */
   bool element::click(context const& /* ctx */, mouse_button /* btn */)
   {
      return false;
   }

   /**
    * \brief
    *    Handles a drag event on the element.
    *
    *    This member function is called after a mouse click, when the user
    *    drags the mouse. This will be called continuously while the mouse is
    *    being dragged. It should be overridden in derived classes to provide
    *    specific functionality for drag events.
    *
    * \param ctx
    *    The context within which the drag event occurred. This parameter
    *    provides information about the view, canvas, and other properties
    *    that might be necessary for handling the drag event.
    *
    * \param btn
    *    The mouse button that initiated the drag event. This is a struct
    *    containing information about the mouse button event, including
    *    whether the button is down, the number of clicks, the state (left,
    *    middle, right), any modifiers, and the position of the event.
    */
   void element::drag(context const& /* ctx */, mouse_button /* btn */)
   {
   }

   /**
    * \brief
    *    Handles raw key events on the element.
    *
    *    This member function is called when a key event occurs on the
    *    element, but only when the element is the focus. It should be
    *    overridden in derived classes to provide specific functionality for
    *    key events.
    *
    * \param ctx
    *    The context within which the key event occurred. This parameter
    *    provides information about the view, canvas, and other properties
    *    that might be necessary for handling the key event.
    *
    * \param k
    *    The key information for the event. This is a struct containing
    *    information about the key event, including the key code, any
    *    modifiers, and the state of the key (up or down).
    *
    * \return
    *    false by default, indicating that the key event was not handled.
    */
   bool element::key(context const& /* ctx */, key_info /* k */)
   {
      return false;
   }

   /**
    * \brief
    *    Handles a text event on the element.
    *
    *    While key handles raw key events, text handles processed text entry,
    *    possibly managing multiple key presses and multi-language inputs.
    *    The prepared text information is UCS4 code point. This member
    *    function is called only when the element is the focus. It should be
    *    overridden in derived classes to provide specific functionality for
    *    text events.
    *
    * \param ctx
    *    The context within which the text event occurred. This parameter
    *    provides information about the view, canvas, and other properties
    *    that might be necessary for handling the text event.
    *
    * \param info
    *    The text information for the event. This is a struct containing
    *    information about the text event, including the text content and any
    *    associated metadata.
    *
    * \return
    *    Return true if the text event was handled, otherwise false.
    */
   bool element::text(context const& /* ctx */, text_info /* info */)
   {
      return false;
   }

   /**
    * \brief
    *    Handles a cursor event on the element.
    *
    *    Handles cursor move events. This is called when the cursor hovers
    *    over an element. It should be overridden in derived classes to
    *    provide specific functionality for cursor events.
    *
    * \param ctx
    *    The context within which the cursor event occurred. This parameter
    *    provides information about the view, canvas, and other properties.
    *
    * \param p
    *    The point where the cursor event occurred. This is a struct
    *    containing the x and y coordinates of the cursor event.
    *
    * \param status
    *    The tracking status of the cursor. This is an enum that indicates
    *    whether the cursor is entering, leaving, or moving within the element.
    *
    * \return
    *    Return true if the cursor event was handled, otherwise false.
    */
   bool element::cursor(context const& /* ctx */, point /* p */, cursor_tracking /* status */)
   {
      return false;
   }

   /**
    * \brief
    *   Handle scroll-wheel or scroll gesture events.
    *
    *    This member function is called when a scroll event occurs on the
    *    element. A scroll event is typically generated when the user moves
    *    the scroll wheel on their mouse or uses a touch gesture on a
    *    touch-enabled device.
    *
    *    It should be overridden in derived classes to provide specific
    *    functionality for scroll events.
    *
    * \param ctx
    *    The context within which the scroll event occurred. This parameter
    *    provides information about the view, canvas, and other properties.
    *
    * \param dir
    *    The direction of the scroll event. This is a struct containing the
    *    x and y coordinates representing the scroll direction. For example,
    *    a positive y value typically represents scrolling up, and a negative
    *    y value represents scrolling down. Scrolling can be fractional for
    *    HDPI and also to allow inertial scrolling.
    *
    * \param p
    *    The point where the scroll event occurred. This is a struct
    *    containing the x and y coordinates of the scroll event. This could
    *    be used to handle scroll events differently based on where in the
    *    element the event occurred.
    *
    * \return
    *    Return true if the scroll event was handled, otherwise false.
    */
   bool element::scroll(context const& /* ctx */, point /* dir */, point /* p */)
   {
      return false;
   }

   /**
    * \brief
    *    Enables or disables the element.
    *
    *    This member function is used to enable or disable the element. When
    *    an element is disabled, it does not respond to user interactions.
    *
    * \param state
    *    The new enabled state for the element. True to enable the element,
    *    false to disable it.
    */
   void element::enable(bool /*state*/)
   {
   }

   /**
    * \brief
    *    Checks if the element is enabled.
    *
    *    This member function is used to check if the element is currently
    *    enabled or disabled.
    *
    * \return
    *    Return true if the element is enabled, otherwise false.
    */
   bool element::is_enabled() const
   {
      return true;
   }

   /**
    * \brief
    *    Checks if the element wants focus.
    *
    *    This member function is used to check if the element wants to
    *    receive focus. If this function returns true, the element can become
    *    the focus of user interactions.
    *
    * \return
    *    Return true if the element wants focus, otherwise false.
    */
   bool element::wants_focus() const
   {
      return false;
   }

   /**
    * \brief
    *    Begins the focus on the element.
    *
    *    This member function is called when the element is about to become
    *    focus. When an element becomes the focus, it will be the one
    *    receiving key and text inputs. It should be overridden in derived
    *    classes to provide specific functionality for when the element
    *    begins to have focus.
    *
    *    Elements that want to receive focus should return true from the
    *    wants_focus function.
    *
    * \param req
    *    The request for focus. This parameter is an enumeration of type
    *    `focus_request`,
    *    which provides information about the type of focus request:
    *    - `from_top`: Make the topmost element the focus.
    *    - `from_bottom`: Make the bottommost element the focus.
    *    - `restore_previous`: The focus request is to restore the previous
    *      focus.
    */
   void element::begin_focus(focus_request /*req*/)
   {
   }

   /**
    * \brief
    *    Ends the focus on the element.
    *
    *    This member function is called when the element is about to lose
    *    focus. It should be overridden in derived classes to provide
    *    specific functionality for when the element ends its focus.
    *
    *    Elements that want to receive focus should return true from the
    *    wants_focus function.
    *
    * \return
    *    Return true if the element successfully ends focus, otherwise false.
    *    It may return false if the element does not want to yield focus.
    */
   bool element::end_focus()
   {
      return true;
   }

   /**
    * \brief
    *    Relinquishes the focus from the current element.
    *
    *    This function is used to remove the focus from the current element.
    *    It first finds the enclosing composite element by calling the
    *    `find_composite` function with the provided context. If a composite
    *    element is found, it calls `relinquish_focus` on that element.
    *
    *    The `context` parameter encapsulates the state of the current
    *    element, including a reference to the element itself and the parent
    *    context. The `find_composite` function uses this information to
    *    traverse the hierarchy of elements and find the composite element.
    *
    * \param ctx
    *    The context within which the focus should be relinquished. This
    *    parameter provides information about the current element and the
    *    parent context, which allows traversal of the element hierarchy.
    */
   void relinquish_focus(context const& ctx)
   {
      auto [p, cctx] = find_composite(ctx);
      if (p)
         relinquish_focus(*p, *cctx);
   }

   /**
    * \brief
    *    Gets the element that currently has focus.
    *
    *    This member function is used to retrieve the element that currently
    *    has focus. By default, it returns the current element (`this`).
    *
    *    It should be overridden in derived classes if a different behavior
    *    is required for determining the focused element.
    *
    * \return
    *    A constant pointer to the element that currently has focus.
    */
   element const* element::focus() const
   {
      return this;
   }

   /**
    * \brief
    *    Gets the element that currently has focus.
    *
    *    This member function is used to retrieve the element that currently
    *    has focus. By default, it returns the current element (`this`).
    *
    *    It should be overridden in derived classes if a different behavior
    *    is required for determining the focused element.
    *
    * \return
    *    A pointer to the element that currently has focus.
    */
   element* element::focus()
   {
      return this;
   }

   /**
    * \brief
    *    Checks if the element wants to receive control events.
    *
    *    This member function is used to determine if the current element
    *    wants to receive control events. A control element is an element
    *    that requires user interaction and implements one or more of the
    *    element control member functions. By default, it returns false,
    *    indicating that the element does not want to receive control events.
    *
    *    It should be overridden in derived classes if a different behavior
    *    is required for determining if the element wants to receive control
    *    events.
    *
    * \return
    *    A boolean value indicating whether the element wants to receive
    *    control events. Returns true if the element wants to receive control
    *    events, otherwise false.
    */
   bool element::wants_control() const
   {
      return false;
   }

   /**
    * \brief
    *    Tracks the drop event on the element.
    *
    *    This member function is called when a drop event occurs on the
    *    element. A drop event is typically triggered when a user has dragged
    *    an item and then releases the mouse button to "drop" that item onto
    *    an element.
    *
    *    It should be overridden in derived classes to provide specific
    *    functionality.
    *
    * \param ctx
    *    The context within which the scroll event occurred. This parameter
    *    provides information about the view, canvas, and other properties.
    *
    * \param info
    *    Information about the drop event encapsulated in a `drop_info`
    *    struct. This includes the payload of the drop (`data`) and the
    *    location where the drop occurred (`where`).
    *
    * \param status
    *    The status of the cursor tracking. This is an enum that indicates
    *    the current state of the cursor. It can be used to determine the
    *    position of the drop event and to handle different states of the
    *    cursor, such as when it is entering, leaving, or moving within the
    *    element.
    */
   void element::track_drop(context const& /*ctx*/, drop_info const& /*info*/, cursor_tracking /*status*/)
   {
   }

   /**
    * \brief
    *    Handles the drop event on the element.
    *
    *    This member function is called when a drop event is completed on the
    *    element. A drop event is typically triggered when a user has dragged
    *    an item and then releases the mouse button to "drop" that item onto
    *    an element. By default, this function returns false, indicating that
    *    the drop event was not handled.
    *
    *    It should be overridden in derived classes to provide specific
    *    functionality for when a drop event is completed on the element,
    *    such as handling the dropped data and returning true to indicate
    *    that the event was handled.
    *
    * \param ctx
    *    The context within which the scroll event occurred. This parameter
    *    provides information about the view, canvas, and other properties.
    *
    * \param info
    *    Information about the drop event encapsulated in a `drop_info`
    *    struct. This includes the payload of the drop (`data`) and the
    *    location where the drop occurred (`where`).
    *
    * \return
    *    A boolean value indicating whether the drop event was handled.
    *    Returns true if the event was handled, otherwise false.
    */
   bool element::drop(context const& /*ctx*/, drop_info const& /*info*/)
   {
      return false;
   }

   /**
    * \brief
    *    Notifies the view about mouse events on the element.
    *
    *    This member function is called to notify the view when a mouse event
    *    occurs on the element. A mouse event is typically triggered when the
    *    mouse button or track pad is pressed down (begin tracking), released
    *    (end tracking), or dragged (while tracking) on the element.
    *
    *    This function delegates the notification of the mouse event to the
    *    `manage_on_tracking` function of the view associated with the element.
    *
    * \param ctx
    *    The context within which the mouse event occurred. This parameter
    *    provides information about the view, canvas, and other properties.
    *
    * \param state
    *    The current state of the tracking. This is an enum that indicates
    *    the current state of the cursor.
    */
   void element::on_tracking(context const& ctx, tracking state)
   {
      ctx.view.manage_on_tracking(*this, state);
   }

   /**
    * \brief
    *    Notifies the view about mouse events on the element.
    *
    *    This member function is called to notify the view when a mouse event
    *    occurs on the element. A mouse event is typically triggered when the
    *    mouse button or track pad is pressed down (begin tracking), released
    *    (end tracking), or dragged (while tracking) on the element.
    *
    *    This function delegates the notification of the mouse event to the
    *    `manage_on_tracking` function of the view passed as a parameter.
    *
    * \param view_
    *    The view associated with the element. This is the view that will
    *    manage the mouse event.
    *
    * \param state
    *    The current state of the tracking. This is an enum that indicates
    *    the current state of the cursor.
    */
   void element::on_tracking(view& view_, tracking state)
   {
      view_.manage_on_tracking(*this, state);
   }

   /**
    * \brief
    *    Retrieves the class name of the element for debugging purposes. Take
    *    note that the result is compiler specific.
    *
    *    This function utilizes the typeid operator to extract the class name
    *    and demangles the obtained string to return the name.
    *
    * \return
    *    A string representing the name of the class to which this element
    *    belongs.
    */
   std::string element::class_name() const
   {
      return demangle(typeid(*this).name());
   }
}}
