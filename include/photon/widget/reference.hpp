/*=======================================================================================
   Copyright (c) 2016-2017 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=======================================================================================*/
#if !defined(PHOTON_GUI_LIB_REFERENCE_APRIL_10_2016)
#define PHOTON_GUI_LIB_REFERENCE_APRIL_10_2016

#include <photon/widget/widget.hpp>

namespace photon
{
   //////////////////////////////////////////////////////////////////////////////////////
   // Widget Reference
   //
   // A widget reference holds another widget by (smart) pointer. Widget references
   // may be copied and all copies will refer to the same widget being referenced.
   //////////////////////////////////////////////////////////////////////////////////////
   template <typename Widget>
   class reference : public widget
   {
   public:
                              reference();
      explicit                reference(photon::widget_ptr ptr);
                              reference(reference&& rhs) = default;
                              reference(reference const& rhs) = default;
      reference&              operator=(reference&& rhs) = default;
      reference&              operator=(reference const& rhs) = default;

   // Image

      virtual widget_limits   limits(basic_context const& ctx) const;
      virtual widget*         hit_test(context const& ctx, point p);
      virtual void            draw(context const& ctx);
      virtual void            layout(context const& ctx);
      virtual bool            scroll(context const& ctx, point dir, point p);
      virtual void            refresh(context const& ctx, widget& widget);

   // Control

      virtual widget*         click(context const& ctx, mouse_button btn);
      virtual void            drag(context const& ctx, mouse_button btn);
      virtual bool            key(context const& ctx, key_info k);
      virtual bool            text(context const& ctx, text_info info);
      virtual bool            cursor(context const& ctx, point p, cursor_tracking status);
      virtual void            idle(basic_context const& ctx);

      virtual bool            focus(focus_request r);
      virtual widget const*   focus() const;
      virtual widget*         focus();
      virtual bool            is_control() const;

   // Receiver

      virtual void            value(bool val);
      virtual void            value(int val);
      virtual void            value(double val);
      virtual void            value(std::string val);

      Widget&                 get();
      Widget const&           get() const;
      widget_ptr              get_ptr();

   private:

      using widget_ptr = std::shared_ptr<Widget>;
      widget_ptr              ptr;
   };

   template <typename Widget>
   reference<Widget> ref(Widget&& rhs);

   template <typename Widget>
   reference<Widget> ref(Widget const& rhs);

   template <typename Widget>
   struct is_reference<reference<Widget>> : std::true_type {};

   //////////////////////////////////////////////////////////////////////////////////////
   // reference (inline) implementation
   //////////////////////////////////////////////////////////////////////////////////////
   template <typename Widget>
   inline reference<Widget>::reference()
   {}

   template <typename Widget>
   inline reference<Widget>::reference(photon::widget_ptr ptr)
    : ptr(std::dynamic_pointer_cast<Widget>(ptr))
   {}

   template <typename Widget>
   inline widget_limits
   reference<Widget>::limits(basic_context const& ctx) const
   {
      return ptr->limits(ctx);
   }

   template <typename Widget>
   inline widget*
   reference<Widget>::hit_test(context const& ctx, point p)
   {
      return ptr->hit_test(ctx, p);
   }

   template <typename Widget>
   inline void
   reference<Widget>::draw(context const& ctx)
   {
      ptr->draw(ctx);
   }

   template <typename Widget>
   inline void
   reference<Widget>::layout(context const& ctx)
   {
      ptr->layout(ctx);
   }

   template <typename Widget>
   inline bool
   reference<Widget>::scroll(context const& ctx, point dir, point p)
   {
      return ptr->scroll(ctx, dir, p);
   }

   template <typename Widget>
   inline void
   reference<Widget>::refresh(context const& ctx, widget& widget)
   {
      ptr->refresh(ctx, widget);
   }

   template <typename Widget>
   inline widget*
   reference<Widget>::click(context const& ctx, mouse_button btn)
   {
      return ptr->click(ctx, btn);
   }

   template <typename Widget>
   inline void
   reference<Widget>::drag(context const& ctx, mouse_button btn)
   {
      return ptr->drag(ctx, btn);
   }

   template <typename Widget>
   inline bool
   reference<Widget>::key(context const& ctx, key_info k)
   {
      return ptr->key(ctx, k);
   }

   template <typename Widget>
   inline bool
   reference<Widget>::text(context const& ctx, text_info info)
   {
      return ptr->text(ctx, info);
   }

   template <typename Widget>
   inline bool
   reference<Widget>::cursor(context const& ctx, point p, cursor_tracking status)
   {
      return ptr->cursor(ctx, p, status);
   }

   template <typename Widget>
   inline void
   reference<Widget>::idle(basic_context const& ctx)
   {
      ptr->idle(ctx);
   }

   template <typename Widget>
   inline bool
   reference<Widget>::focus(focus_request r)
   {
      return ptr->focus(r);
   }

   template <typename Widget>
   inline widget const*
   reference<Widget>::focus() const
   {
      return ptr->focus();
   }

   template <typename Widget>
   inline widget*
   reference<Widget>::focus()
   {
      return ptr->focus();
   }

   template <typename Widget>
   inline bool
   reference<Widget>::is_control() const
   {
      return ptr->is_control();
   }

   template <typename Widget>
   inline void reference<Widget>::value(bool val)
   {
      ptr->value(val);
   }

   template <typename Widget>
   inline void reference<Widget>::value(int val)
   {
      ptr->value(val);
   }

   template <typename Widget>
   inline void reference<Widget>::value(double val)
   {
      ptr->value(val);
   }

   template <typename Widget>
   inline void reference<Widget>::value(std::string val)
   {
      ptr->value(val);
   }

   template <typename Widget>
   inline Widget&
   reference<Widget>::get()
   {
      return *ptr.get();
   }

   template <typename Widget>
   inline Widget const&
   reference<Widget>::get() const
   {
      return *ptr.get();
   }

   template <typename Widget>
   inline widget_ptr
   reference<Widget>::get_ptr()
   {
      return ptr;
   }

   template <typename Widget>
   inline reference<Widget> ref(Widget&& rhs)
   {
      return reference<Widget>{ share(rhs) };
   }

   template <typename Widget>
   inline reference<Widget> ref(Widget const& rhs)
   {
      return reference<Widget>{ share(rhs) };
   }
}

#endif
