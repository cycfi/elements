/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_REFERENCE_APRIL_10_2016)
#define PHOTON_GUI_LIB_REFERENCE_APRIL_10_2016

#include <photon/widget/widget.hpp>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Widget Reference
   //
   // A widget reference holds another widget by (smart) pointer. Widget references
   // may be copied and all copies will refer to the same widget being referenced.
   ////////////////////////////////////////////////////////////////////////////////////////////////
   template <typename Widget>
   class widget_reference : public widget
   {
   public:
                              widget_reference(Widget&& rhs);
                              widget_reference(Widget const& rhs);

   // image

      virtual rect            limits(basic_context const& ctx) const;
      virtual widget*         hit_test(context const& ctx, point p);
      virtual void            draw(context const& ctx);
      virtual void            layout(context const& ctx);
      virtual bool            scroll(context const& ctx, point p);

   // control

      virtual widget*         click(context const& ctx, mouse_button btn);
      virtual void            drag(context const& ctx, mouse_button btn);
      virtual bool            key(context const& ctx, key_info const& k);
      virtual bool            text(context const& ctx, text_info const& info);
      virtual bool            cursor(context const& ctx, point p);

      virtual bool            focus(focus_request r);
      virtual widget const*   focus() const;
      virtual widget*         focus();
      virtual bool            is_control() const;

      Widget&                 get();
      Widget const&           get() const;

   private:

      using widget_ptr = std::shared_ptr<Widget>;
      widget_ptr              ptr;
   };

   template <typename Widget>
   widget_reference<Widget> ref(Widget&& rhs);

   template <typename Widget>
   widget_reference<Widget> ref(Widget const& rhs);

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // widget_reference (inline) implementation
   ////////////////////////////////////////////////////////////////////////////////////////////////
   template <typename Widget>
   inline widget_reference<Widget>::widget_reference(Widget&& rhs)
    : ptr(std::dynamic_pointer_cast<Widget>(new_(rhs)))
   {}

   template <typename Widget>
   inline widget_reference<Widget>::widget_reference(Widget const& rhs)
    : ptr(std::dynamic_pointer_cast<Widget>(new_(rhs)))
   {}

   template <typename Widget>
   inline rect
   widget_reference<Widget>::limits(basic_context const& ctx) const
   {
      return ptr->limits(ctx);
   }

   template <typename Widget>
   inline widget*
   widget_reference<Widget>::hit_test(context const& ctx, point p)
   {
      return ptr->hit_test(ctx, p);
   }

   template <typename Widget>
   inline void
   widget_reference<Widget>::draw(context const& ctx)
   {
      ptr->draw(ctx);
   }

   template <typename Widget>
   inline void
   widget_reference<Widget>::layout(context const& ctx)
   {
      ptr->layout(ctx);
   }

   template <typename Widget>
   inline bool
   widget_reference<Widget>::scroll(context const& ctx, point p)
   {
      return ptr->scroll(ctx, p);
   }

   template <typename Widget>
   inline widget*
   widget_reference<Widget>::click(context const& ctx, mouse_button btn)
   {
      return ptr->click(ctx, btn);
   }

   template <typename Widget>
   inline void
   widget_reference<Widget>::drag(context const& ctx, mouse_button btn)
   {
      return ptr->drag(ctx, btn);
   }

   template <typename Widget>
   inline bool
   widget_reference<Widget>::key(context const& ctx, key_info const& k)
   {
      return ptr->key(ctx, k);
   }

   template <typename Widget>
   inline bool
   widget_reference<Widget>::text(context const& ctx, text_info const& info)
   {
      return ptr->text(ctx, info);
   }

   template <typename Widget>
   inline bool
   widget_reference<Widget>::cursor(context const& ctx, point p)
   {
      return ptr->cursor(ctx, p);
   }

   template <typename Widget>
   inline bool
   widget_reference<Widget>::focus(focus_request r)
   {
      return ptr->focus(r);
   }

   template <typename Widget>
   inline widget const*
   widget_reference<Widget>::focus() const
   {
      return ptr->focus();
   }

   template <typename Widget>
   inline widget*
   widget_reference<Widget>::focus()
   {
      return ptr->focus();
   }

   template <typename Widget>
   inline bool
   widget_reference<Widget>::is_control() const
   {
      return ptr->is_control();
   }

   template <typename Widget>
   Widget&
   widget_reference<Widget>::get()
   {
      return *ptr.get();
   }

   template <typename Widget>
   Widget const&
   widget_reference<Widget>::get() const
   {
      return *ptr.get();
   }

   template <typename Widget>
   inline widget_reference<Widget> ref(Widget&& rhs)
   {
      return { rhs };
   }

   template <typename Widget>
   inline widget_reference<Widget> ref(Widget const& rhs)
   {
      return { rhs };
   }
}

#endif
