/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_APRIL_10_2016)
#define ELEMENTS_APRIL_10_2016

#include <elements/base_view.hpp>
#include <elements/support/rect.hpp>

#include <memory>
#include <string>
#include <type_traits>

namespace cycfi { namespace elements
{
   struct basic_context;
   struct context;

   ////////////////////////////////////////////////////////////////////////////
   // Elements
   //
   // This is the class that deals with the graphic representation of fine-
   // grained elements inside a window which may be static graphics or active
   // controls.
   ////////////////////////////////////////////////////////////////////////////
   class element : public std::enable_shared_from_this<element>
   {
   public:

      using element_ptr = std::shared_ptr<element>;
      using element_const_ptr = std::shared_ptr<element const>;

                              element() {}
                              virtual ~element() {}

                              element(element&&) = default;
                              element(element const&) = default;
      element&                operator=(element&&) = default;
      element&                operator=(element const&) = default;

   // Image

      virtual view_limits     limits(basic_context const& ctx) const;
      virtual view_stretch    stretch() const;
      virtual element*        hit_test(context const& ctx, point p);
      virtual void            draw(context const& ctx);
      virtual void            layout(context const& ctx);
      virtual bool            scroll(context const& ctx, point dir, point p);
      virtual void            refresh(context const& ctx, element& element);
      void                    refresh(context const& ctx) { refresh(ctx, *this); }

   // Control

      virtual element*        click(context const& ctx, mouse_button btn);
      virtual void            drag(context const& ctx, mouse_button btn);
      virtual bool            key(context const& ctx, key_info k);
      virtual bool            text(context const& ctx, text_info info);
      virtual bool            cursor(context const& ctx, point p, cursor_tracking status);

      virtual bool            focus(focus_request r);
      virtual element const*  focus() const;
      virtual element*        focus();
      virtual bool            is_control() const;

   // Receiver

      virtual void            value(bool val);
      virtual void            value(int val);
      virtual void            value(double val);
      virtual void            value(std::string val);

   protected:

      void                    on_begin_tracking(context const& ctx);
      void                    on_end_tracking(context const& ctx);
   };

   ////////////////////////////////////////////////////////////////////////////
   using element_ptr = std::shared_ptr<element>;
   using element_const_ptr = std::shared_ptr<element const>;
   using weak_element_ptr = std::weak_ptr<element>;
   using weak_element_const_ptr = std::weak_ptr<element const>;

   template <typename Element>
   inline auto share(Element&& e)
   {
      using element_type = typename std::decay<Element>::type;
      return std::make_shared<element_type>(std::forward<Element>(e));
   }

   template <typename Element>
   inline auto get(std::shared_ptr<Element> ptr)
   {
      return std::weak_ptr<Element>(ptr);
   }

   inline element empty()
   {
      return {};
   }
}}

#endif
