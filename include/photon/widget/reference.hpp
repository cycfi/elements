/*=============================================================================
   Copyright (c) 2016-2017 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(PHOTON_GUI_LIB_REFERENCE_APRIL_10_2016)
#define PHOTON_GUI_LIB_REFERENCE_APRIL_10_2016

#include <photon/element/element.hpp>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////
   // Element Reference
   //
   // A element reference holds another element by (smart) pointer. Element references
   // may be copied and all copies will refer to the same element being referenced.
   ////////////////////////////////////////////////////////////////////////////
   template <typename Element>
   class reference : public element
   {
   public:
                              reference();
      explicit                reference(photon::element_ptr ptr);
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

      Element&                 get();
      Element const&           get() const;
      element_ptr             get_ptr();

   private:

      using element_ptr = std::shared_ptr<Element>;
      element_ptr             ptr;
   };

   template <typename Element>
   reference<Element> ref(Element&& rhs);

   template <typename Element>
   reference<Element> ref(Element const& rhs);

   template <typename Element>
   struct is_reference<reference<Element>> : std::true_type {};

   ////////////////////////////////////////////////////////////////////////////
   // reference (inline) implementation
   ////////////////////////////////////////////////////////////////////////////
   template <typename Element>
   inline reference<Element>::reference()
   {}

   template <typename Element>
   inline reference<Element>::reference(photon::element_ptr ptr)
    : ptr(std::dynamic_pointer_cast<Element>(ptr))
   {}

   template <typename Element>
   inline view_limits
   reference<Element>::limits(basic_context const& ctx) const
   {
      return ptr->limits(ctx);
   }

   template <typename Element>
   inline element*
   reference<Element>::hit_test(context const& ctx, point p)
   {
      return ptr->hit_test(ctx, p);
   }

   template <typename Element>
   inline void
   reference<Element>::draw(context const& ctx)
   {
      ptr->draw(ctx);
   }

   template <typename Element>
   inline void
   reference<Element>::layout(context const& ctx)
   {
      ptr->layout(ctx);
   }

   template <typename Element>
   inline bool
   reference<Element>::scroll(context const& ctx, point dir, point p)
   {
      return ptr->scroll(ctx, dir, p);
   }

   template <typename Element>
   inline void
   reference<Element>::refresh(context const& ctx, element& element)
   {
      ptr->refresh(ctx, element);
   }

   template <typename Element>
   inline element*
   reference<Element>::click(context const& ctx, mouse_button btn)
   {
      return ptr->click(ctx, btn);
   }

   template <typename Element>
   inline void
   reference<Element>::drag(context const& ctx, mouse_button btn)
   {
      return ptr->drag(ctx, btn);
   }

   template <typename Element>
   inline bool
   reference<Element>::key(context const& ctx, key_info k)
   {
      return ptr->key(ctx, k);
   }

   template <typename Element>
   inline bool
   reference<Element>::text(context const& ctx, text_info info)
   {
      return ptr->text(ctx, info);
   }

   template <typename Element>
   inline bool
   reference<Element>::cursor(context const& ctx, point p, cursor_tracking status)
   {
      return ptr->cursor(ctx, p, status);
   }

   template <typename Element>
   inline void
   reference<Element>::idle(basic_context const& ctx)
   {
      ptr->idle(ctx);
   }

   template <typename Element>
   inline bool
   reference<Element>::focus(focus_request r)
   {
      return ptr->focus(r);
   }

   template <typename Element>
   inline element const*
   reference<Element>::focus() const
   {
      return ptr->focus();
   }

   template <typename Element>
   inline element*
   reference<Element>::focus()
   {
      return ptr->focus();
   }

   template <typename Element>
   inline bool
   reference<Element>::is_control() const
   {
      return ptr->is_control();
   }

   template <typename Element>
   inline void reference<Element>::value(bool val)
   {
      ptr->value(val);
   }

   template <typename Element>
   inline void reference<Element>::value(int val)
   {
      ptr->value(val);
   }

   template <typename Element>
   inline void reference<Element>::value(double val)
   {
      ptr->value(val);
   }

   template <typename Element>
   inline void reference<Element>::value(std::string val)
   {
      ptr->value(val);
   }

   template <typename Element>
   inline Element&
   reference<Element>::get()
   {
      return *ptr.get();
   }

   template <typename Element>
   inline Element const&
   reference<Element>::get() const
   {
      return *ptr.get();
   }

   template <typename Element>
   inline element_ptr
   reference<Element>::get_ptr()
   {
      return ptr;
   }

   template <typename Element>
   inline reference<Element> ref(Element&& rhs)
   {
      return reference<Element>{ share(rhs) };
   }

   template <typename Element>
   inline reference<Element> ref(Element const& rhs)
   {
      return reference<Element>{ share(rhs) };
   }
}

#endif
