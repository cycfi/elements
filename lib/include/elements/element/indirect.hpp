/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_REFERENCE_APRIL_10_2016)
#define ELEMENTS_REFERENCE_APRIL_10_2016

#include <elements/element/element.hpp>
#include <functional>

namespace cycfi { namespace elements
{
   template <typename Indirect, typename Element, typename Enable = void>
   struct indirect_receiver {};

   template <typename Indirect, typename Element>
   struct indirect_receiver<
         Indirect, Element
       , typename std::enable_if<std::is_base_of<receiver_base, Element>::value>::type
    > : receiver_base
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

   template <typename Base>
   class indirect : public Base, public indirect_receiver<indirect<Base>, Base>
   {
   public:

      using Base::Base;
      using Base::operator=;

   // Display

      view_limits             limits(basic_context const& ctx) const override;
      view_stretch            stretch() const override;
      unsigned                span() const override;
      element*                hit_test(context const& ctx, point p) override;
      void                    draw(context const& ctx) override;
      void                    layout(context const& ctx) override;
      void                    refresh(context const& ctx, element& element, int outward = 0) override;

      using element::refresh;

   // Control

      bool                    wants_control() const override;
      bool                    click(context const& ctx, mouse_button btn) override;
      void                    drag(context const& ctx, mouse_button btn) override;
      bool                    key(context const& ctx, key_info k) override;
      bool                    text(context const& ctx, text_info info) override;
      bool                    cursor(context const& ctx, point p, cursor_tracking status) override;
      bool                    scroll(context const& ctx, point dir, point p) override;

      bool                    wants_focus() const override;
      void                    begin_focus() override;
      void                    end_focus() override;
      element const*          focus() const override;
      element*                focus() override;
   };

   struct indirect_base : element
   {
      virtual element&        get() = 0;
      virtual element const&  get() const = 0;
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

   template <typename Element>
   indirect<reference<typename std::remove_reference<Element>::type>>
   link(Element &rhs);

   ////////////////////////////////////////////////////////////////////////////
   // Just like reference, but shared_reference retains the shared pointer.
   ////////////////////////////////////////////////////////////////////////////
   template <typename Element>
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
   view_stretch indirect<Base>::stretch() const
   {
      return this->get().stretch();
   }

   template <typename Base>
   unsigned indirect<Base>::span() const
   {
      return this->get().span();
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
   inline bool
   indirect<Base>::wants_control() const
   {
      return this->get().wants_control();
   }

   template <typename Base>
   inline bool
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
