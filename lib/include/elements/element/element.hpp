/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_ELEMENT_APRIL_10_2016)
#define ELEMENTS_ELEMENT_APRIL_10_2016

#include <elements/base_view.hpp>
#include <elements/support/receiver.hpp>
#include <elements/support/rect.hpp>
#include <infra/support.hpp>
#include <memory>
#include <type_traits>
#include <concepts>

namespace cycfi::elements
{
   struct basic_context;
   class context;

   namespace concepts
   {
      // $$$ TODO: Move this to a better place $$$
      template <typename T>
      concept NullaryFunction = requires(T f)
      {
         { f() } -> std::same_as<void>;
      };
   }

   /**
    * \brief
    *    Base class for all UI elements in the Cycfi Elements library.
    *
    *    This is the class that deals with the graphic representation of
    *    fine-grained elements inside a window, which may be static graphics
    *    or active controls. The `element` class provides a common interface
    *    and foundational functionality for all user interface components. It
    *    handles tasks such as rendering, event processing, and layout
    *    calculations, serving as a building block for more complex UI
    *    elements and ensuring consistent behavior across different parts of
    *    the UI.
    *
    *    Elements are light-weight objects with minimal memory footprint. For
    *    example, elements do not have information about their coordinates.
    *    Instead, a context object encapsulates this information with the
    *    element's bounds calculated on the fly on demand by a container or
    *    parent element responsible for layout. This allows elements to be
    *    reused in and shared among views.
    */
   class element : public std::enable_shared_from_this<element>
   {
   public:
                              element() {}
                              virtual ~element() = default;

   // Display

      virtual view_limits     limits(basic_context const& ctx) const;
      virtual view_stretch    stretch() const;
      virtual unsigned        span() const;
      virtual element*        hit_test(context const& ctx, point p, bool leaf, bool control);
      virtual void            draw(context const& ctx);
      virtual void            layout(context const& ctx);
      virtual void            refresh(context const& ctx, element& e, int outward = 0);
      void                    refresh(context const& ctx, int outward = 0);

      using context_function = std::function<void(context const& ctx)>;
      virtual void            in_context_do(context const& ctx, element& e, context_function f);

   // Control

      virtual bool            wants_control() const;
      virtual bool            click(context const& ctx, mouse_button btn);
      virtual void            drag(context const& ctx, mouse_button btn);
      virtual bool            key(context const& ctx, key_info k);
      virtual bool            text(context const& ctx, text_info info);
      virtual bool            cursor(context const& ctx, point p, cursor_tracking status);
      virtual bool            scroll(context const& ctx, point dir, point p);
      virtual void            enable(bool state = true);
      virtual bool            is_enabled() const;

      /**
       * \enum element::focus_request
       * \brief The type of focus requested on an element.
       *
       * \var from_top Make topmost element the focus.
       * \var from_bottom Make bottommost element the focus.
       * \var restore_previous Restore the previous focus state.
       */
      enum focus_request { from_top, from_bottom, restore_previous };

      virtual bool            wants_focus() const;
      virtual void            begin_focus(focus_request req);
      virtual bool            end_focus();
      virtual element const*  focus() const;
      virtual element*        focus();

      friend void             relinquish_focus(context const& ctx);

      virtual void            track_drop(context const& ctx, drop_info const& info, cursor_tracking status);
      virtual bool            drop(context const& ctx, drop_info const& info);

      /**
       * \enum element::tracking
       * \brief Represents the state of mouse tracking on an element.
       *
       * \var none No tracking is currently happening on the element.
       * \var begin_tracking Ttracking has just started.
       * \var while_tracking Tracking is ongoing.
       * \var end_tracking Tracking has just ended.
       */
      enum tracking { none, begin_tracking, while_tracking, end_tracking };

      virtual std::string     class_name() const;

   protected:

      void                    on_tracking(context const& ctx, tracking state);
      void                    on_tracking(view& view_, tracking state);
   };

   namespace concepts
   {
      template <typename T>
      concept Element = std::is_base_of_v<element, std::decay_t<T>>;

      template<typename T>
      concept ElementPtr = requires(T p)
      {
         { *p } -> std::convertible_to<element&>;
      };
   }

   ////////////////////////////////////////////////////////////////////////////
   // Additional declarations
   ////////////////////////////////////////////////////////////////////////////

   // Type alisases
   using element_ptr = std::shared_ptr<element>;
   using element_const_ptr = std::shared_ptr<element const>;
   using weak_element_ptr = std::weak_ptr<element>;
   using weak_element_const_ptr = std::weak_ptr<element const>;

   void relinquish_focus(context const& ctx);

   using cycfi::share;
   using cycfi::get;

   inline element empty();

   //--------------------------------------------------------------------------
   // Inlines
   //--------------------------------------------------------------------------

   /**
    * \brief
    *    Refreshes the state of the element.
    *
    *    This function is used to refresh the state of the element. It can be
    *    used when the state of the element has changed and it needs to be
    *    updated to reflect these changes.
    *
    * \param ctx
    *    Represents the current context, providing access to the current view
    *    and the canvas and contains information for element placement and
    *    sizing, including bounding dimensions.
    *
    * \param outward
    *    Determines the extent of outward refreshing for the view. An outward
    *    parameter of 0 will refresh only the element. An outward of 1 (the
    *    default) will include the element and its parent.
    */
   inline void element::refresh(context const& ctx, int outward)
   {
      refresh(ctx, *this, outward);
   }

   /**
    * \brief
    *    Constructs an empty `element` instance.
    *
    *    This function is used to create an instance of an `element` that is
    *    effectively empty. This can be useful for creating placeholder or
    *    default elements within a UI.
    *
    * \return
    *    An instance of an `element` that has no associated properties or
    *    state.
    */
   inline element empty()
   {
      return {};
   }
}

#endif
