/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_PROXY_APRIL_10_2016)
#define ELEMENTS_PROXY_APRIL_10_2016

#include <elements/element/element.hpp>
#include <type_traits>

namespace cycfi::elements
{
   //--------------------------------------------------------------------------
   // Proxy base
   //--------------------------------------------------------------------------

   /**
    * \class proxy_base
    *
    * \brief
    *    The base class for all proxy classes: Elements that encapsulates
    *    another element (its 'subject'). The proxy delegates its functions
    *    to its encapsulated subject, but may augment or totally override its
    *    behavior.
    *
    *    The `proxy_base` class provides the basic interface and common
    *    functionality for all proxy classes. A proxy class is used to
    *    control access to another element, referred to as the 'subject'. The
    *    proxy can handle additional tasks, such as adding decorations,
    *    handling alignment, managing margins around an element, filtering
    *    information, and controlling access.
    *
    *    The `proxy_base` class defines the `prepare_subject` and
    *    `restore_subject` methods that are used to prepare the subject for
    *    use and then restore it after use. These methods should be
    *    overridden in derived classes to provide specific functionality.
    */
   class proxy_base : public element
   {
   public:

   // Display

      view_limits             limits(basic_context const& ctx) const override;
      view_stretch            stretch() const override;
      unsigned                span() const override;
      element*                hit_test(context const& ctx, point p, bool leaf, bool control) override;
      void                    draw(context const& ctx) override;
      void                    layout(context const& ctx) override;
      void                    refresh(context const& ctx, element& element, int outward = 0) override;
      void                    in_context_do(context const& ctx, element& e, context_function f) override;

   // Subject preparation

      virtual void            prepare_subject(context& ctx);
      virtual void            prepare_subject(context& ctx, point& p);
      virtual void            restore_subject(context& ctx);

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

   // Proxy

      virtual element const&  subject() const = 0;
      virtual element&        subject() = 0;
   };

   namespace concepts
   {
      template <typename T>
      concept Proxy = std::is_base_of_v<proxy_base, std::decay_t<T>>;
   }

   //--------------------------------------------------------------------------
   // Proxy Template
   //--------------------------------------------------------------------------

   namespace concepts
   {
     template<typename Subject>
      concept ProxySubject =
         Element<Subject> &&
         !std::is_reference_v<Subject> && !std::is_const_v<Subject>;
   }

   /**
    * \class proxy
    *
    * \brief
    *    A template class that provides a proxy for the given subject type.
    *
    * \tparam Subject
    *    The type of the subject that this proxy encapsulates. The type must
    *    satisfy the `ProxySubject` concept: It must conform to the Element
    *    concept, not be a reference, and not be const.
    *
    * \tparam Base
    *    The base class for this proxy. It defaults to `proxy_base` if not
    *    specified.
    *
    *    The `proxy` class is a template class that encapsulates an instance
    *    of the `Subject` type. It inherits from the `Base` class and can
    *    override or augment the behavior of the `Subject`. The proxy
    *    delegates its functions to its encapsulated subject, but may augment
    *    or totally override its behavior.
    */
   template <concepts::ProxySubject Subject, concepts::Proxy Base = proxy_base>
   class proxy : public Base
   {
   public:

      using subject_type = Subject;

                              template <typename... T>
                              proxy(Subject subject_, T&&... args);

      void                    subject(Subject&& subject_);
      void                    subject(Subject const& subject_);
      element const&          subject() const override;
      element&                subject() override;
      Subject const&          actual_subject() const;
      Subject&                actual_subject();

      Subject const*          operator->() const;
      Subject*                operator->();
      Subject const*          operator*() const;
      Subject*                operator*();

   private:

      Subject                 _subject;
   };

   //--------------------------------------------------------------------------
   // Inlines
   //--------------------------------------------------------------------------

   /**
    * \brief
    *    Constructs a proxy object.
    *
    * \tparam Subject
    *    The type of the subject that this proxy encapsulates. The type must
    *    satisfy the `ProxySubject` concept: It must conform to the Element
    *    concept, not be a reference, and not be const.
    *
    * \tparam Base
    *    The base class for this proxy.
    *
    * \tparam T
    *    The types of additional arguments to be forwarded to the base class
    *    constructor.
    *
    * \param subject_
    *    The subject to be encapsulated by this proxy.
    *
    * \param args
    *    Additional arguments to be forwarded to the base class constructor.
    *
    * This constructor initializes the proxy with the provided subject and
    * forwards any additional arguments to the base class constructor.
    */
   template <concepts::ProxySubject Subject, concepts::Proxy Base>
   template <typename... T>
   inline proxy<Subject, Base>::proxy(Subject subject_, T&&... args)
    : Base(std::forward<T>(args)...)
    , _subject(std::move(subject_))
   {}

   /**
    * \brief
    *    Sets the subject of the proxy using an rvalue reference.
    *
    * \tparam Subject
    *    The type of the subject that this proxy encapsulates.
    *
    * \tparam Base
    *    The base class for this proxy.
    *
    * \param subject_
    *    An rvalue reference to the subject to be set.
    *
    * This function sets the subject of the proxy by moving the provided
    * subject.
    */
   template <concepts::ProxySubject Subject, concepts::Proxy Base>
   inline void proxy<Subject, Base>::subject(Subject&& subject_)
   {
      _subject = std::move(subject_);
   }

   /**
    * \brief
    *    Sets the subject of the proxy using a const reference.
    *
    * \tparam Subject
    *    The type of the subject that this proxy encapsulates.
    *
    * \tparam Base
    *    The base class for this proxy.
    *
    * \param subject_
    *    A const reference to the subject to be set.
    *
    * This function sets the subject of the proxy by copying the provided
    * subject.
    */
   template <concepts::ProxySubject Subject, concepts::Proxy Base>
   inline void proxy<Subject, Base>::subject(Subject const& subject_)
   {
      _subject = subject_;
   }

   /**
    * \brief
    *    Returns a constant reference to the subject.
    *
    * \return
    *    A constant reference to the subject.
    */
   template <concepts::ProxySubject Subject, concepts::Proxy Base>
   inline element const& proxy<Subject, Base>::subject() const
   {
      return _subject;
   }

   /**
    * \brief
    *    Returns a reference to the subject.
    *
    * \return
    *    A reference to the subject.
    */
   template <concepts::ProxySubject Subject, concepts::Proxy Base>
   inline element& proxy<Subject, Base>::subject()
   {
      return _subject;
   }

   /**
    * \brief
    *    Returns a constant reference to the actual subject.
    *
    * \return
    *    A constant reference to the actual subject.
    */
   template <concepts::ProxySubject Subject, concepts::Proxy Base>
   inline Subject const& proxy<Subject, Base>::actual_subject() const
   {
      return _subject;
   }

   /**
    * \brief
    *    Returns a reference to the actual subject.
    *
    * \return
    *    A reference to the actual subject.
    */
   template <concepts::ProxySubject Subject, concepts::Proxy Base>
   inline Subject& proxy<Subject, Base>::actual_subject()
   {
      return _subject;
   }

   /**
    * \brief
    *    Overloads the arrow operator to return a const pointer to the actual
    *    subject.
    *
    * \return
    *    A const pointer to the actual subject.
    */
   template <concepts::ProxySubject Subject, concepts::Proxy Base>
   inline Subject const* proxy<Subject, Base>::operator->() const
   {
      return &_subject;
   }

   /**
    * \brief
    *    Overloads the arrow operator to return a pointer to the actual
    *    subject.
    *
    * \return
    *    A pointer to the actual subject.
    */
   template <concepts::ProxySubject Subject, concepts::Proxy Base>
   inline Subject* proxy<Subject, Base>::operator->()
   {
      return &_subject;
   }

   /**
    * \brief
    *    Overloads the dereference operator to return a const pointer to the
    *    actual subject.
    *
    * \return
    *    A const pointer to the actual subject.
    */
   template <concepts::ProxySubject Subject, concepts::Proxy Base>
   inline Subject const* proxy<Subject, Base>::operator*() const
   {
      return &_subject;
   }

   /**
    * \brief
    *    Overloads the dereference operator to return a pointer to the actual
    *    subject.
    *
    * \return
    *    A pointer to the actual subject.
    */
   template <concepts::ProxySubject Subject, concepts::Proxy Base>
   inline Subject* proxy<Subject, Base>::operator*()
   {
      return &_subject;
   }
}

#endif
