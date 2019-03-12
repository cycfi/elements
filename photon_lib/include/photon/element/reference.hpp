/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_PHOTON_GUI_LIB_REFERENCE_APRIL_10_2016)
#define CYCFI_PHOTON_GUI_LIB_REFERENCE_APRIL_10_2016

#include <photon/element/element.hpp>
#include <photon/element/element.hpp>
#include <functional>

namespace cycfi { namespace photon
{
   ////////////////////////////////////////////////////////////////////////////
   // Element Reference
   //
   // A element reference holds another element using std::reference_wrapper.
   // Element references may be copied and all copies will refer to the same
   // element being referenced. It is the responsibility of the client to
   // manage the lifetime of the referenced element and make sure it is valid
   // (alive) when a reference member function is called.
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

   // Image

      virtual view_limits     limits(basic_context const& ctx) const;
      virtual element*        hit_test(context const& ctx, point p);
      virtual void            draw(context const& ctx);
      virtual void            layout(context const& ctx);
      virtual bool            scroll(context const& ctx, point dir, point p);
      virtual void            refresh(context const& ctx, element& element);

      using element::refresh;

   // Control

      virtual element*        click(context const& ctx, mouse_button btn);
      virtual void            drag(context const& ctx, mouse_button btn);
      virtual bool            key(context const& ctx, key_info k);
      virtual bool            text(context const& ctx, text_info info);
      virtual bool            cursor(context const& ctx, point p, cursor_tracking status);
      virtual void            idle(basic_context const& ctx);

      virtual bool            focus(focus_request r);
      virtual element const*  focus() const;
      virtual element*        focus();
      virtual bool            is_control() const;

   // Receiver

      virtual void            value(bool val);
      virtual void            value(int val);
      virtual void            value(double val);
      virtual void            value(std::string val);

      Element&                get();
      Element const&          get() const;

   private:

      using element_ref = std::reference_wrapper<Element>;
      element_ref             ref;
   };

   template <typename Element>
   reference<typename std::remove_reference<Element>::type>
   link(Element& rhs);

   ////////////////////////////////////////////////////////////////////////////
   // reference (inline) implementation
   ////////////////////////////////////////////////////////////////////////////
   template <typename Element>
   inline reference<Element>::reference(Element& e)
    : ref(e)
   {}

   template <typename Element>
   inline view_limits
   reference<Element>::limits(basic_context const& ctx) const
   {
      return ref.get().limits(ctx);
   }

   template <typename Element>
   inline element*
   reference<Element>::hit_test(context const& ctx, point p)
   {
      return ref.get().hit_test(ctx, p);
   }

   template <typename Element>
   inline void
   reference<Element>::draw(context const& ctx)
   {
      ref.get().draw(ctx);
   }

   template <typename Element>
   inline void
   reference<Element>::layout(context const& ctx)
   {
      ref.get().layout(ctx);
   }

   template <typename Element>
   inline bool
   reference<Element>::scroll(context const& ctx, point dir, point p)
   {
      return ref.get().scroll(ctx, dir, p);
   }

   template <typename Element>
   inline void
   reference<Element>::refresh(context const& ctx, element& element)
   {
      ref.get().refresh(ctx, element);
   }

   template <typename Element>
   inline element*
   reference<Element>::click(context const& ctx, mouse_button btn)
   {
      return ref.get().click(ctx, btn);
   }

   template <typename Element>
   inline void
   reference<Element>::drag(context const& ctx, mouse_button btn)
   {
      return ref.get().drag(ctx, btn);
   }

   template <typename Element>
   inline bool
   reference<Element>::key(context const& ctx, key_info k)
   {
      return ref.get().key(ctx, k);
   }

   template <typename Element>
   inline bool
   reference<Element>::text(context const& ctx, text_info info)
   {
      return ref.get().text(ctx, info);
   }

   template <typename Element>
   inline bool
   reference<Element>::cursor(context const& ctx, point p, cursor_tracking status)
   {
      return ref.get().cursor(ctx, p, status);
   }

   template <typename Element>
   inline void
   reference<Element>::idle(basic_context const& ctx)
   {
      ref.get().idle(ctx);
   }

   template <typename Element>
   inline bool
   reference<Element>::focus(focus_request r)
   {
      return ref.get().focus(r);
   }

   template <typename Element>
   inline element const*
   reference<Element>::focus() const
   {
      return ref.get().focus();
   }

   template <typename Element>
   inline element*
   reference<Element>::focus()
   {
      return ref.get().focus();
   }

   template <typename Element>
   inline bool
   reference<Element>::is_control() const
   {
      return ref.get().is_control();
   }

   template <typename Element>
   inline void reference<Element>::value(bool val)
   {
      ref.get().value(val);
   }

   template <typename Element>
   inline void reference<Element>::value(int val)
   {
      ref.get().value(val);
   }

   template <typename Element>
   inline void reference<Element>::value(double val)
   {
      ref.get().value(val);
   }

   template <typename Element>
   inline void reference<Element>::value(std::string val)
   {
      ref.get().value(val);
   }

   template <typename Element>
   inline Element&
   reference<Element>::get()
   {
      return ref.get();
   }

   template <typename Element>
   inline Element const&
   reference<Element>::get() const
   {
      return ref.get();
   }

   template <typename Element>
   inline reference<typename std::remove_reference<Element>::type>
   link(Element& rhs)
   {
      return reference<typename std::remove_reference<Element>::type>{ rhs };
   }

   template <typename Element>
   inline reference<Element>
   link(std::shared_ptr<Element> rhs)
   {
      return reference<Element>{ *rhs };
   }
}}

#endif
