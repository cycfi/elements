/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_ELEMENT_APRIL_10_2016)
#define ELEMENTS_ELEMENT_APRIL_10_2016

#include <elements/base_view.hpp>

#include <infra/string_view.hpp>
#include <memory>
#include <type_traits>

namespace cycfi { namespace elements
{
   struct basic_context;
   class context;
   class view;

   // Hoist artist colors namespace and  type for backward compatibility with
   // original cairo API.
   using artist::rgba;
   namespace colors = artist::colors;

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
      void                    refresh(context const& ctx, int outward = 0) { refresh(ctx, *this, outward); }

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

      enum focus_request { from_top, from_bottom, restore_previous };

      virtual bool            wants_focus() const;
      virtual void            begin_focus(focus_request req);
      virtual bool            end_focus();
      virtual element const*  focus() const;
      virtual element*        focus();

                              // Utility function for relinquishing focus
      friend void             relinquish_focus(context const& ctx);

      virtual void            track_drop(context const& ctx, drop_info const& info, cursor_tracking status);
      virtual bool            drop(context const& ctx, drop_info const& info);

      enum tracking { none, begin_tracking, while_tracking, end_tracking };

      virtual std::string     class_name() const;

   protected:

      void                    on_tracking(context const& ctx, tracking state);
      void                    on_tracking(view& view_, tracking state);
   };

   // Utility function for relinquishing focus
   void  relinquish_focus(context const& ctx);

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
   inline auto get(std::shared_ptr<Element> const& ptr)
   {
      return std::weak_ptr<Element>(ptr);
   }

   inline element empty()
   {
      return {};
   }
}}

#endif
