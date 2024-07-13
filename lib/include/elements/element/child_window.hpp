/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_CHILD_WINDOW_JANUARY_19_2021)
#define ELEMENTS_CHILD_WINDOW_JANUARY_19_2021

#include <elements/element/floating.hpp>
#include <elements/element/tracker.hpp>
#include <elements/element/traversal.hpp>
#include <elements/element/button.hpp>

namespace cycfi::elements
{
   /**
    * \class child_window_element
    *
    * \brief
    *    Class that represents a child window within a larger display context.
    *
    *    Child windows are floating elements that may overlap with other
    *    elements and can move to the front when clicked.
    */
   class child_window_element : public floating_element
   {
   public:

      using floating_element::floating_element;

      bool                 click(context const& ctx, mouse_button btn) override;
   };

   /**
    *  \brief
    *      Factory function to create a child window element.
    *
    *      This function creates a child window by wrapping the given subject
    *      (element) in a child_window_element. The resulting window has a
    *      specified rectangular area.
    *
    *  \tparam Subject
    *      Subject must meet the `Element` concept.
    *
    *  \param bounds
    *      The boundaries of the child window.
    *
    *  \param subject
    *      An rvalue reference to the subject, the element to be wrapped in a
    *      child window.
    *
    *  \return
    *      A proxy object of type `child_window_element` containing the
    *      subject element, within defined boundaries.
    */
   template <concepts::Element Subject>
   inline proxy<remove_cvref_t<Subject>, child_window_element>
   child_window(rect bounds, Subject&& subject)
   {
      return {std::forward<Subject>(subject), bounds};
   }

   struct window_mover_tracker_info : tracker_info
   {
      using tracker_info::tracker_info;

      float                _offs_top, _offs_bottom;
   };

   /**
    * \class window_mover_element
    *
    * \brief
    *    A base class representing a window mover element.
    */
   class window_mover_element : public tracker<proxy_base, window_mover_tracker_info>
   {
   public:

      using tracker = tracker<proxy_base, window_mover_tracker_info>;

      element*             hit_test(context const& ctx, point p, bool leaf, bool control) override;
      bool                 click(context const& ctx, mouse_button btn) override;
      void                 drag(context const& ctx, mouse_button btn) override;
      void                 keep_tracking(context const& ctx, tracker_info& track_info) override;
   };

   /**
    * \brief
    *    Make an element (usually a title bar) able to move a window.
    *
    *    movable allows an element (e.g. title_bar) to be movable if it is
    *    contained inside a floating_element. The whole floating_element
    *    moves when the element is dragged around.
    *
    * \tparam Subject
    *    The type of the subject. The Subject must meet the `Element`
    *    concept.
    *
    * \param subject
    *    The subject element that will be made movable.
    *
    * \return
    *    The window element.
    */
   template <concepts::Element Subject>
   inline proxy<remove_cvref_t<Subject>, window_mover_element>
   window_mover(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }

   struct window_resizer_tracker_info : tracker_info
   {
      using tracker_info::tracker_info;

      enum handle : unsigned char
      {
         top      = 1,
         left     = 2,
         bottom   = 4,
         right    = 8
      };

      unsigned char        _handle = 0;
   };

   class window_resizer_element : public tracker<proxy_base, window_resizer_tracker_info>
   {
   public:

      using tracker = tracker<proxy_base, window_resizer_tracker_info>;

      bool                 cursor(context const& ctx, point p, cursor_tracking status) override;
      element*             hit_test(context const& ctx, point p, bool leaf, bool control) override;
      bool                 click(context const& ctx, mouse_button btn) override;
      void                 drag(context const& ctx, mouse_button btn) override;
      void                 keep_tracking(context const& ctx, tracker_info& track_info) override;
   };

   template <concepts::Element Subject>
   inline proxy<remove_cvref_t<Subject>, window_resizer_element>
   window_resizer(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }

   /**
    * \class window_closer_element
    *
    * \brief
    *    Class that encapsulates a window closer UI element functionality.
    *
    *    When contained inside a floating_element, this class allows a button
    *    (or any clickable element) to close (or hide) the floating_element
    *    when it is clicked.
    *
    * \tparam Subject
    *    The type of the subject. Must satisfy the `Element` concept.
    */
   template <concepts::Element Subject>
   class window_closer_element : public proxy<Subject>
   {
   public:

      using proxy<Subject>::proxy;

      void                    prepare_subject(context& ctx) override;
   };

   /**
    * \brief
    *    Create a window_closer element.
    *
    *    If the subject is a part of a floating_element, this function
    *    enables the subject to close its parent floating_element upon a
    *    click event.
    *
    * \tparam Subject
    *    The type of the subject. Must satisfy the `Element` concept.
    *
    * \param subject
    *    The element that is to be made window_closer.
    *
    * \return
    *    The window_closer element.
    */
   template <concepts::Element Subject>
   inline window_closer_element<remove_cvref_t<Subject>>
   window_closer(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }

   void close_floating_element(context& ctx, floating_element* cw);

   template <concepts::Element Subject>
   inline void window_closer_element<Subject>::prepare_subject(context& ctx)
   {
      auto btn = find_subject<basic_button*>(this);
      if (btn)
      {
         btn->on_click =
            [&ctx](bool)
            {
               auto fl = find_parent<floating_element*>(ctx);
               if (fl)
                  close_floating_element(ctx, fl);
            };
      }
   }

   template <concepts::Element Subject>
   class window_minimizer_element : public proxy<Subject>
   {
   public:

      using proxy<Subject>::proxy;

      void                    prepare_subject(context& ctx) override;
   };

   template <concepts::Element Subject>
   inline window_minimizer_element<remove_cvref_t<Subject>>
   window_minimizer(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }

   void minimize_window(context& ctx, floating_element* cw);

   template <concepts::Element Subject>
   inline void
   window_minimizer_element<Subject>::prepare_subject(context& ctx)
   {
      auto btn = find_subject<basic_button*>(this);
      if (btn)
      {
         btn->on_click =
            [&ctx](bool)
            {
               auto fl = find_parent<floating_element*>(ctx);
               if (fl)
                  minimize_window(ctx, fl);
            };
      }
   }

   template <concepts::Element Subject>
   class window_maximizer_element : public proxy<Subject>
   {
   public:

      using proxy<Subject>::proxy;

      void                    prepare_subject(context& ctx) override;
   };

   template <concepts::Element Subject>
   inline window_maximizer_element<remove_cvref_t<Subject>>
   window_maximizer(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }

   void maximize_window(context& ctx, floating_element* cw);

   template <concepts::Element Subject>
   inline void
   window_maximizer_element<Subject>::prepare_subject(context& ctx)
   {
      auto btn = find_subject<basic_button*>(this);
      if (btn)
      {
         btn->on_click =
            [&ctx](bool)
            {
               auto fl = find_parent<floating_element*>(ctx);
               if (fl)
                  maximize_window(ctx, fl);
            };
      }
   }
}

#endif
