/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_REFERENCE_APRIL_10_2016)
#define ELEMENTS_REFERENCE_APRIL_10_2016

#include <elements/element/element.hpp>
#include <elements/element/element.hpp>
#include <functional>

namespace cycfi { namespace elements
{
   template <typename Base>
   class indirect : public Base
   {
   public:

      using Base::Base;
      using Base::operator=;

                              indirect(indirect&& rhs) = default;
                              indirect(indirect const& rhs) = default;
      indirect&               operator=(indirect&& rhs) = default;
      indirect&               operator=(indirect const& rhs) = default;

   // Image

      virtual view_limits     limits(basic_context const& ctx) const;
      virtual element*        hit_test(context const& ctx, point p);
      virtual void            draw(context const& ctx);
      virtual void            layout(context const& ctx);
      virtual bool            scroll(context const& ctx, point dir, point p);
      virtual void            refresh(context const& ctx, element& element, int outward = 0);

      using element::refresh;

   // Control

      virtual element*        click(context const& ctx, mouse_button btn);
      virtual void            drag(context const& ctx, mouse_button btn);
      virtual bool            key(context const& ctx, key_info k);
      virtual bool            text(context const& ctx, text_info info);
      virtual bool            cursor(context const& ctx, point p, cursor_tracking status);

      virtual bool            wants_focus() const;
      virtual void            begin_focus();
      virtual void            end_focus();
      virtual element const*  focus() const;
      virtual element*        focus();
      virtual bool            is_control() const;

   // Receiver

      virtual void            value(bool val);
      virtual void            value(int val);
      virtual void            value(double val);
      virtual void            value(std::string_view val);
   };

   ////////////////////////////////////////////////////////////////////////////
   // reference
   //
   // A reference holds another element using std::reference_wrapper. Element
   // references may be copied and all copies will refer to the same element
   // being referenced. It is the responsibility of the client to manage the
   // lifetime of the referenced element and make sure it is valid (alive)
   // when a reference member function is called.
   ////////////////////////////////////////////////////////////////////////////
   template <typename Element>
   class reference : public element
   {
   public:

      explicit                reference(Element& e);
                              reference(reference&& rhs) = default;
                              reference(reference const& rhs) = default;
      reference&              operator=(reference&& rhs) = default;
      reference&              operator=(reference const& rhs) = default;

      Element&                get();
      Element const&          get() const;

   private:

      using element_ref = std::reference_wrapper<Element>;
      element_ref             _ref;
   };

   template <typename Element>
   indirect<reference<typename std::remove_reference<Element>::type>>
   link(Element &rhs);

   ////////////////////////////////////////////////////////////////////////////
   // Just like reference, but shared_reference retains the shared pointer.
   ////////////////////////////////////////////////////////////////////////////
   template <typename Element>
   class shared_element : public element
   {
   public:

      explicit                shared_element(std::shared_ptr<Element> ptr);
                              shared_element(shared_element&& rhs) = default;
                              shared_element(shared_element const& rhs) = default;
      shared_element&         operator=(shared_element&& rhs) = default;
      shared_element&         operator=(shared_element const& rhs) = default;
      shared_element&         operator=(std::shared_ptr<Element> ptr);

      Element&                get();
      Element const&          get() const;

   private:

      std::shared_ptr<Element> _ptr;
   };

   template <typename Element>
   indirect<shared_element<Element>>
   hold(std::shared_ptr<Element> rhs);

   indirect<shared_element<element>>
   hold_any(std::shared_ptr<element> rhs);

   ////////////////////////////////////////////////////////////////////////////
   // indirect (inline) implementation
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base>
   inline view_limits
   indirect<Base>::limits(basic_context const& ctx) const
   {
      return this->get().limits(ctx);
   }

   template <typename Base>
   inline element*
   indirect<Base>::hit_test(context const& ctx, point p)
   {
      return this->get().hit_test(ctx, p);
   }

   template <typename Base>
   inline void
   indirect<Base>::draw(context const& ctx)
   {
      this->get().draw(ctx);
   }

   template <typename Base>
   inline void
   indirect<Base>::layout(context const& ctx)
   {
      this->get().layout(ctx);
   }

   template <typename Base>
   inline bool
   indirect<Base>::scroll(context const& ctx, point dir, point p)
   {
      return this->get().scroll(ctx, dir, p);
   }

   template <typename Base>
   inline void
   indirect<Base>::refresh(context const& ctx, element& element, int outward)
   {
      this->get().refresh(ctx, element, outward);
   }

   template <typename Base>
   inline element*
   indirect<Base>::click(context const& ctx, mouse_button btn)
   {
      return this->get().click(ctx, btn);
   }

   template <typename Base>
   inline void
   indirect<Base>::drag(context const& ctx, mouse_button btn)
   {
      return this->get().drag(ctx, btn);
   }

   template <typename Base>
   inline bool
   indirect<Base>::key(context const& ctx, key_info k)
   {
      return this->get().key(ctx, k);
   }

   template <typename Base>
   inline bool
   indirect<Base>::text(context const& ctx, text_info info)
   {
      return this->get().text(ctx, info);
   }

   template <typename Base>
   inline bool
   indirect<Base>::cursor(context const& ctx, point p, cursor_tracking status)
   {
      return this->get().cursor(ctx, p, status);
   }

   template <typename Base>
   inline bool
   indirect<Base>::wants_focus() const
   {
      return this->get().wants_focus();
   }

   template <typename Base>
   inline void
   indirect<Base>::begin_focus()
   {
      return this->get().begin_focus();
   }

   template <typename Base>
   inline void
   indirect<Base>::end_focus()
   {
      return this->get().end_focus();
   }

   template <typename Base>
   inline element const*
   indirect<Base>::focus() const
   {
      return this->get().focus();
   }

   template <typename Base>
   inline element*
   indirect<Base>::focus()
   {
      return this->get().focus();
   }

   template <typename Base>
   inline bool
   indirect<Base>::is_control() const
   {
      return this->get().is_control();
   }

   template <typename Base>
   inline void indirect<Base>::value(bool val)
   {
      this->get().value(val);
   }

   template <typename Base>
   inline void indirect<Base>::value(int val)
   {
      this->get().value(val);
   }

   template <typename Base>
   inline void indirect<Base>::value(double val)
   {
      this->get().value(val);
   }

   template <typename Base>
   inline void indirect<Base>::value(std::string_view val)
   {
      this->get().value(val);
   }

   ////////////////////////////////////////////////////////////////////////////
   // reference (inline) implementation
   ////////////////////////////////////////////////////////////////////////////
   template <typename Element>
   inline reference<Element>::reference(Element& e)
    : _ref(e)
   {}

   template <typename Element>
   inline Element&
   reference<Element>::get()
   {
      return _ref.get();
   }

   template <typename Element>
   inline Element const&
   reference<Element>::get() const
   {
      return _ref.get();
   }

   template <typename Element>
   inline indirect<reference<typename std::remove_reference<Element>::type>>
   link(Element &rhs)
   {
      return indirect<reference<typename std::remove_reference<Element>::type>>{ rhs };
   }

   ////////////////////////////////////////////////////////////////////////////
   // shared_reference (inline) implementation
   ////////////////////////////////////////////////////////////////////////////
   template <typename Element>
   inline shared_element<Element>::shared_element(std::shared_ptr<Element> ptr)
    : _ptr(ptr)
   {}

   template <typename Element>
   inline Element&
   shared_element<Element>::get()
   {
      return *_ptr;
   }

   template <typename Element>
   inline Element const&
   shared_element<Element>::get() const
   {
      return *_ptr;
   }

   template <typename Element>
   inline shared_element<Element>&
   shared_element<Element>::operator=(std::shared_ptr<Element> ptr)
   {
      _ptr = ptr;
      return *this;
   }

   template <typename Element>
   inline indirect<shared_element<Element>>
   hold(std::shared_ptr<Element> rhs)
   {
      return indirect<shared_element<Element>>{ rhs };
   }

   inline indirect<shared_element<element>>
   hold_any(std::shared_ptr<element> rhs)
   {
      return indirect<shared_element<element>>{ rhs };
   }
}}

#endif
