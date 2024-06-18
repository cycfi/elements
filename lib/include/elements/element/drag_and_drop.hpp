/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_DRAG_AND_DROP_OCTOBER_11_2023)
#define ELEMENTS_DRAG_AND_DROP_OCTOBER_11_2023

#include <elements/element/proxy.hpp>
#include <elements/element/tracker.hpp>
#include <elements/element/floating.hpp>
#include <elements/element/selection.hpp>
#include <functional>
#include <set>

namespace cycfi::elements
{
   // forward declarations
   class composite_base;

   /**
    * \class drop_receiver
    *
    * \brief
    *    A proxy class responsible for managing the drop-receiver side of the
    *    drag-and-drop functionality.
    */
   class drop_receiver : public proxy_base
   {
   public:

      using base_type = proxy_base;
      using mime_types = std::set<std::string>;

                              drop_receiver(std::initializer_list<std::string> mime_types_);
      bool                    wants_control() const override;
      void                    prepare_subject(context& ctx) override;
      void                    track_drop(context const& ctx, drop_info const& info, cursor_tracking status) override;
      bool                    drop(context const& ctx, drop_info const& info) override;

      bool                    is_tracking() const     { return _is_tracking; }
      mime_types const&       get_mime_types() const  { return _mime_types; }
      mime_types&             get_mime_types()        { return _mime_types; }

   private:

      bool                    _is_tracking = false;
      mime_types              _mime_types;
   };

   /**
    * \class drop_box_element
    *
    * \brief
    *    A GUI element that inherits from `drop_receiver`, a proxy class
    *    designed to handle and process drop operations.
    *
    *    The `drop_receiver` accepts in an initializer list of MIME type
    *    strings that the specifies payloads that it can accept. While
    *    interacting with the user on a drag and drop operation, the
    *    `drop_box_element` class provides a visual cue indicating that the
    *    element is receptive to the payload.
    *
    *    An `on_drop` callback function is called on a successful drop
    *    operation when the user releases the payload onto the element. This
    *    client-supplied callback should return `true` if it wants to accept
    *    the drop. The default lambda function returns false, indicating that
    *    the the drop was not successful.
    */
   class drop_box_element : public drop_receiver
   {
   public:

      using base_type = drop_receiver;
      using on_drop_function = std::function<bool(drop_info const& info)>;

                              drop_box_element(std::initializer_list<std::string> mime_types_);
      void                    draw(context const& ctx) override;
      element*                hit_test(context const& ctx, point p, bool leaf, bool control) override;
      bool                    drop(context const& ctx, drop_info const& info) override;

      on_drop_function        on_drop = [](drop_info const& /*info*/){ return false; };
   };

   /**
    * \brief
    *    Function for adding `drop_box_element` functionality to a supplied
    *    subject.
    *
    * \tparam Subject
    *    The subject element type which must satisfy the `Element` concept.
    *
    * \param subject
    *    The subject.
    *
    * \param mime_types
    *    An initializer list of MIME type strings that the `drop_box_element`
    *    should recognise and accept in drag-and-drop operations.
    *
    * \returns
    *    A `drop_box_element` proxy over the subject.
    */
   template <concepts::Element Subject>
   inline proxy<remove_cvref_t<Subject>, drop_box_element>
   drop_box(Subject&& subject, std::initializer_list<std::string> mime_types)
   {
      return {std::forward<Subject>(subject), mime_types};
   }

   /**
    * \class drop_inserter_element
    *
    * \brief
    *    A specialized drag-and-drop receiver proxy that handles the
    *    insertion of payloads into a list, as well as drag and drop
    *    operations within the list, including items selection, moving
    *    selections, and erasing selections.
    *
    *    The `drop_inserter_element` extends the `drop_receiver` class,
    *    providing capabilities to handle precise drop-and-drop operations.
    *    The class can accept certain MIME data types and allows for advanced
    *    interactions including ability to move and erase entries, and handle
    *    selections. Client-supplied callback functions allow external
    *    application data to synchronize with the GUI list representation.
    *
    * \var on_drop_function on_drop
    *    The callback that gets invoked when a drop event is detected.
    *
    * \var on_move_function on_move
    *    The callback that gets invoked when items in the list need to be
    *    moved.
    *
    * \var on_delete_function on_erase
    *    The callback that gets invoked when items in the list need to be
    *    erased.
    *
    * \var on_select_function on_select
    *    The callback that gets invoked when a selection is made in the list.
    */
   class drop_inserter_element : public drop_receiver
   {
   public:

      using base_type = drop_receiver;
      using indices_type = std::vector<std::size_t>;
      using on_drop_function = std::function<bool(drop_info const& info, std::size_t ix)>;
      using on_move_function = std::function<void(std::size_t pos, indices_type const& indices)>;
      using on_delete_function = std::function<void(indices_type const& indices)>;
      using on_select_function = std::function<void(indices_type const& indices, std::size_t latest)>;

                              drop_inserter_element(std::initializer_list<std::string> mime_types_);

      void                    draw(context const& ctx) override;
      void                    track_drop(context const& ctx, drop_info const& info, cursor_tracking status) override;
      bool                    drop(context const& ctx, drop_info const& info) override;
      bool                    click(context const& ctx, mouse_button btn) override;
      bool                    key(context const& ctx, key_info k) override;
      bool                    wants_focus() const override { return true; }

      on_drop_function        on_drop = [](drop_info const&, std::size_t){ return false; };
      on_move_function        on_move = [](std::size_t, indices_type const&){};
      on_delete_function      on_erase = [](indices_type const&){};
      on_select_function      on_select = [](indices_type const&, std::size_t){};

      int                     insertion_pos() const { return _insertion_pos; }
      void                    move(indices_type const& indices);
      void                    erase(indices_type const& indices);

   public:

      int                     _insertion_pos = -1;
   };

   namespace detail
   {
      template <concepts::Element Subject>
      inline proxy<remove_cvref_t<Subject>, drop_inserter_element>
      make_drop_inserter(Subject&& subject, std::initializer_list<std::string> mime_types)
      {
         return {std::forward<Subject>(subject), mime_types};
      }
   }

   /**
    * \brief
    *    Create a `drop_inserter` proxy which is capable of handling
    *    drag-and-drop payloads into a list for a specified subject.
    *
    * \tparam Subject
    *    The subject element type which must satisfy the `Element` concept.
    *
    * \param subject
    *    The subject. This is expected to contain a list type element, but is
    *    not enforced.
    *
    * \param mime_types
    *    An initializer list of MIME type strings that the `drop_inserter`
    *    should recognize and accept in drag-and-drop operations.
    *
    * \returns
    *    A `drop_inserter` proxy that is capable of handling the insertion of
    *    drag-and-drop payloads into a list.
    */
   template <concepts::Element Subject>
   auto drop_inserter(Subject&& subject, std::initializer_list<std::string> mime_types)
   {
      return
         detail::make_drop_inserter(
            selection_list(std::forward<Subject>(subject))
          , mime_types
         );
   }

   /**
    * \class draggable_element
    *
    * \brief
    *    A proxy that allows a given element to be selectable and draggable.
    *
    *    `draggable_element` extends both the `tracker<proxy_base>` and
    *    `selectable` classes, making the wrapped element both draggable and
    *    selectable. The dragging operation renders an image of the wrapped
    *    element that follows the cursor during the drag.
    */
   class draggable_element : public tracker<proxy_base>, public selectable
   {
   public:

      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;
      element*                hit_test(context const& ctx, point p, bool leaf, bool control) override;
      bool                    key(context const& ctx, key_info k) override;
      bool                    wants_focus() const override { return true; }

      bool                    is_selected() const override;
      void                    select(bool state) override;

   private:

      void                    begin_tracking(context const& ctx, tracker_info& track_info) override;
      void                    keep_tracking(context const& ctx, tracker_info& track_info) override;
      void                    end_tracking(context const& ctx, tracker_info& track_info) override;

      using drag_image_ptr = std::shared_ptr<floating_element>;

      bool                    _selected = false;
      drag_image_ptr          _drag_image;
   };

   /**
    * \brief
    *    Create a draggable element proxy.
    *
    * \tparam Subject
    *    The element type which must satisfy the `Element` concept.
    *
    * \param subject
    *    The `Subject` that needs to be treated as draggable.
    *
    * \returns
    *    A `proxy` object that wraps the `Subject` and treats it as a
    *    `draggable_element`.
    */
   template <concepts::Element Subject>
   inline proxy<remove_cvref_t<Subject>, draggable_element>
   draggable(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }

   /**
    * \brief
    *    Checks if the draggable element is selected.
    *
    * \returns
    *    `true` if selected, `false` otherwise.
    */
   inline bool draggable_element::is_selected() const
   {
      return _selected;
   }

   /**
    * \brief
    *    Sets the selected state of the draggable element.
    *
    * \param state_
    *    The new selected state (`true`==selected and `false` otherwise).
    */
   inline void draggable_element::select(bool state_)
   {
      _selected = state_;
   }
}

#endif
