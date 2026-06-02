/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_REFERENCE_APRIL_10_2016)
#define ELEMENTS_REFERENCE_APRIL_10_2016

#include <elements/element/element.hpp>
#include <elements/support/receiver.hpp>
#include <functional>

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // TMP code makes `indirect_receiver` derive from `receiver_base` if the
   // Element derives from `receiver_base`, and implements the `value` getter
   // and setter member functions.
   ////////////////////////////////////////////////////////////////////////////
   template <typename Indirect, concepts::Element Element>
   struct indirect_receiver {};

   template <typename Indirect, concepts::Element Element>
      requires std::is_base_of_v<receiver_base, std::decay_t<Element>>
   struct indirect_receiver<Indirect, Element> : receiver_base
   {
      using receiver_type = typename Element::receiver_type;
      using getter_type = typename Element::getter_type;
      using param_type = typename Element::param_type;

      virtual void value(param_type val)
      {
         static_cast<Indirect*>(this)->get().value(val);
      }

      virtual getter_type value() const
      {
         return static_cast<Indirect const*>(this)->get().value();
      }
   };

   /**
    * \brief
    *    An adapter class providing indirect delegation to the `Base` element
    *    class.
    *
    *    The `indirect` class template fulfills the same interfaces as the
    *    `Base` element class through the indirection.
    *
    * \tparam Base
    *    Must conform to the `Element` concept.
    */
   template <concepts::Element Base>
   class indirect : public Base, public indirect_receiver<indirect<Base>, Base>
   {
   public:

      using Base::Base;
      using Base::operator=;
      using focus_request = typename Base::focus_request;
      using context_function = typename Base::context_function;
      static constexpr auto restore_previous = Base::restore_previous;

   // Display

      view_limits             limits(basic_context const& ctx) const override;
      view_stretch            stretch() const override;
      unsigned                span() const override;
      element*                hit_test(context const& ctx, point p, bool leaf, bool control) override;
      void                    draw(context const& ctx) override;
      void                    layout(context const& ctx) override;
      void                    refresh(context const& ctx, element& element, int outward = 0) override;
      void                    in_context_do(context const& ctx, element& e, context_function f) override;

      using element::refresh;

   // Control

      bool                    wants_control() const override;
      bool                    click(context const& ctx, mouse_button btn) override;
      void                    drag(context const& ctx, mouse_button btn) override;
      bool                    key(context const& ctx, key_info k) override;
      bool                    text(context const& ctx, text_info info) override;
      bool                    cursor(context const& ctx, point p, cursor_tracking status) override;
      bool                    scroll(context const& ctx, point dir, point p) override;
      void                    enable(bool state = true) override;
      bool                    is_enabled() const override;

      bool                    wants_focus() const override;
      void                    begin_focus(focus_request req) override;
      bool                    end_focus() override;
      element const*          focus() const override;
      element*                focus() override;

      void                    track_drop(context const& ctx, drop_info const& info, cursor_tracking status) override;
      bool                    drop(context const& ctx, drop_info const& info) override;

   };

   struct indirect_base : element
   {
      virtual element&        get() = 0;
      virtual element const&  get() const = 0;
   };

   /**
    * \class reference
    *
    * \brief
    *    A class template that wraps another element using
    *    std::reference_wrapper.
    *
    *    A reference holds another element using std::reference_wrapper.
    *    Element references may be copied and all copies will refer to the
    *    same element being referenced. It is the responsibility of the
    *    client to manage the lifetime of the referenced element and make
    *    sure it is valid (alive) when a reference member function is called.
    *
    * \tparam Element
    *    A type that fulfills the `Element` concept.
    */
   template <concepts::Element Element>
   class reference : public indirect_base
   {
   public:

      explicit                reference(Element& e);

      Element&                get() override;
      Element const&          get() const override;

   private:

      using element_ref = std::reference_wrapper<Element>;
      element_ref             _ref;
   };

   template <concepts::Element Element>
   indirect<reference<typename std::remove_reference<Element>::type>>
   link(Element &rhs);

   /**
    * \class shared_element
    *
    * \brief
    *    A class template that retains a shared pointer to another element.
    *    Much like the `reference` class, with the addition of lifetime
    *    management.
    *
    * \tparam Element
    *    A type that fulfills the `Element` concept.
    */
   template <concepts::Element Element>
   class shared_element : public indirect_base
   {
   public:

      explicit                shared_element(std::shared_ptr<Element> ptr);
      shared_element&         operator=(std::shared_ptr<Element> ptr);

      Element&                get() override;
      Element const&          get() const override;

   private:

      std::shared_ptr<Element> _ptr;
   };

   template <concepts::Element Element>
   indirect<shared_element<Element>>
   hold(std::shared_ptr<Element> rhs);

   indirect<shared_element<element>>
   hold_any(std::shared_ptr<element> rhs);

   //--------------------------------------------------------------------------
   // Inlines
   //--------------------------------------------------------------------------

   template <concepts::Element Base>
   inline view_limits
   indirect<Base>::limits(basic_context const& ctx) const
   {
      return this->get().limits(ctx);
   }

   template <concepts::Element Base>
   view_stretch indirect<Base>::stretch() const
   {
      return this->get().stretch();
   }

   template <concepts::Element Base>
   unsigned indirect<Base>::span() const
   {
      return this->get().span();
   }

   template <concepts::Element Base>
   inline element*
   indirect<Base>::hit_test(context const& ctx, point p, bool leaf, bool control)
   {
      return this->get().hit_test(ctx, p, leaf, control);
   }

   template <concepts::Element Base>
   inline void
   indirect<Base>::draw(context const& ctx)
   {
      this->get().draw(ctx);
   }

   template <concepts::Element Base>
   inline void
   indirect<Base>::layout(context const& ctx)
   {
      this->get().layout(ctx);
   }

   template <concepts::Element Base>
   inline bool
   indirect<Base>::scroll(context const& ctx, point dir, point p)
   {
      return this->get().scroll(ctx, dir, p);
   }

   template <concepts::Element Base>
   inline void
   indirect<Base>::enable(bool state)
   {
      return this->get().enable(state);
   }

   template <concepts::Element Base>
   inline bool
   indirect<Base>::is_enabled() const
   {
      return this->get().is_enabled();
   }

   template <concepts::Element Base>
   inline void
   indirect<Base>::refresh(context const& ctx, element& e, int outward)
   {
      this->get().refresh(ctx, e, outward);
   }

   template <concepts::Element Base>
   inline void
   indirect<Base>::in_context_do(context const& ctx, element& e, context_function f)
   {
      this->get().in_context_do(ctx, e, f);
   }

   template <concepts::Element Base>
   inline bool
   indirect<Base>::wants_control() const
   {
      return this->get().wants_control();
   }

   template <concepts::Element Base>
   inline bool
   indirect<Base>::click(context const& ctx, mouse_button btn)
   {
      return this->get().click(ctx, btn);
   }

   template <concepts::Element Base>
   inline void
   indirect<Base>::drag(context const& ctx, mouse_button btn)
   {
      return this->get().drag(ctx, btn);
   }

   template <concepts::Element Base>
   inline bool
   indirect<Base>::key(context const& ctx, key_info k)
   {
      return this->get().key(ctx, k);
   }

   template <concepts::Element Base>
   inline bool
   indirect<Base>::text(context const& ctx, text_info info)
   {
      return this->get().text(ctx, info);
   }

   template <concepts::Element Base>
   inline bool
   indirect<Base>::cursor(context const& ctx, point p, cursor_tracking status)
   {
      return this->get().cursor(ctx, p, status);
   }

   template <concepts::Element Base>
   inline bool
   indirect<Base>::wants_focus() const
   {
      return this->get().wants_focus();
   }

   template <concepts::Element Base>
   inline void
   indirect<Base>::begin_focus(focus_request req)
   {
      return this->get().begin_focus(req);
   }

   template <concepts::Element Base>
   inline bool
   indirect<Base>::end_focus()
   {
      return this->get().end_focus();
   }

   template <concepts::Element Base>
   inline element const*
   indirect<Base>::focus() const
   {
      return this->get().focus();
   }

   template <concepts::Element Base>
   inline element*
   indirect<Base>::focus()
   {
      return this->get().focus();
   }

   template <concepts::Element Base>
   inline void indirect<Base>::track_drop(context const& ctx, drop_info const& info, cursor_tracking status)
   {
      this->get().track_drop(ctx, info, status);
   }

   template <concepts::Element Base>
   inline bool indirect<Base>::drop(context const& ctx, drop_info const& info)
   {
      return this->get().drop(ctx, info);
   }

   /**
    * \brief
    *    Constructs a `reference` to an `Element` by binding it to the given
    *    `Element&` reference.
    *
    *    Intentionally binds to the provided `Element&` instead of making a
    *    copy. This constructor enables reference semantics for the instances
    *    of `Element` class.
    *
    * \tparam Element
    *    Must satisfy the `Element` concept.
    *
    * \param e
    *    A reference to an instance of `Element`.
    */
   template <concepts::Element Element>
   inline reference<Element>::reference(Element& e)
    : _ref(e)
   {}

   /**
    * \brief
    *    Retrieves a non-const reference to the element referenced by this
    *    object.
    *
    * \tparam Element
    *    Must satisfy the `Element` concept.
    *
    * \return
    *    A non-const reference to the element referenced by this object.
    */
   template <concepts::Element Element>
   inline Element&
   reference<Element>::get()
   {
      return _ref.get();
   }

   /**
    * \brief
    *    Retrieves a const reference to the element referenced by this
    *    object.
    *
    * \tparam Element
    *    Must satisfy the `Element` concept.
    *
    * \return
    *    A const reference to the element referenced by this object.
    */
   template <concepts::Element Element>
   inline Element const&
   reference<Element>::get() const
   {
      return _ref.get();
   }

   /**
    * \brief
    *    Creates a `reference` to the provided element, wrapped in an
    *    `indirect` delegate.
    *
    * \tparam Element
    *    Must satisfy the `Element` concept.
    *
    * \param rhs
    *    The element to which the `reference` should link.
    *
    * \return
    *    An `indirect` delegate encapsulating a `reference` to the provided
    *    element `rhs`.
    */
   template <concepts::Element Element>
   inline indirect<reference<typename std::remove_reference<Element>::type>>
   link(Element &rhs)
   {
      return indirect<reference<typename std::remove_reference<Element>::type>>{rhs};
   }

   /**
    * \brief
    *    Constructs a `shared_element` that retains a shared pointer to an
    *    instance of a derived class `Element`.
    *
    * \tparam Element
    *    Must satisfy the `Element` concept.
    *
    * \param ptr
    *    A shared pointer to an instance of the derived `Element`.
    */
   template <concepts::Element Element>
   inline shared_element<Element>::shared_element(std::shared_ptr<Element> ptr)
    : _ptr(ptr)
   {}

   /**
    * \brief
    *    Retrieves a non-const reference to the element held by the retained
    *    shared pointer.
    *
    * \tparam Element
    *    Must satisfy the `Element` concept.
    *
    * \return
    *    A non-const reference to the element owned by the retained shared
    *    pointer.
    */
   template <concepts::Element Element>
   inline Element&
   shared_element<Element>::get()
   {
      return *_ptr;
   }

   /**
    * \brief
    *    Retrieves a const reference to the element owned by the retained
    *    shared pointer.
    *
    * \tparam Element
    *    Must satisfy the `Element` concept.
    *
    * \return
    *    A const reference to the element owned by the retained shared
    *    pointer.
    */
   template <concepts::Element Element>
   inline Element const&
   shared_element<Element>::get() const
   {
      return *_ptr;
   }

   template <concepts::Element Element>
   inline shared_element<Element>&
   shared_element<Element>::operator=(std::shared_ptr<Element> ptr)
   {
      _ptr = ptr;
      return *this;
   }

   /**
    * \brief
    *    Creates a `shared_element` using the provided shared pointer,
    *    wrapped in an `indirect` delegate.
    *
    *    Take note that the element is held by a pointer to the most derived
    *    `elememnt` class, and will therefore retain static access to all the
    *    members specific to the derived class.
    *
    * \tparam Element
    *    Must satisfy the `Element` concept.
    *
    * \param rhs
    *    The shared pointer to the element to be held.
    *
    * \return
    *    An `indirect` delegate encapsulating a `shared_element` that holds
    *    the shared pointer to the provided element `rhs`.
    */
   template <concepts::Element Element>
   inline indirect<shared_element<Element>>
   hold(std::shared_ptr<Element> rhs)
   {
      return indirect<shared_element<Element>>{rhs};
   }

   /**
    * \brief
    *    Creates a `shared_element` using the provided shared pointer to an
    *    `element`, wrapped in an `indirect` delegate.
    *
    *    Take note that since this function handles the element through a
    *    pointer to the most base class `element`, it will therefore lose
    *    static access to all the derived class-specific members.
    *
    * \param rhs
    *    The shared pointer to the element to be held.
    *
    * \return
    *    An `indirect` delegate encapsulating a `shared_element` that holds
    *    the shared pointer to the provided `element`.
    */
   inline indirect<shared_element<element>>
   hold_any(std::shared_ptr<element> rhs)
   {
      return indirect<shared_element<element>>{rhs};
   }
}

#endif
