/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_APRIL_10_2016)
#define PHOTON_GUI_LIB_WIDGET_APRIL_10_2016

#include <photon/support.hpp>
#include <photon/rect.hpp>
#include <photon/point.hpp>
#include <photon/context.hpp>
#include <photon/key.hpp>

#include <memory>
#include <vector>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Widgets
   //
   // This is the class that deals with the graphic representation of fine-grained
   // elements inside a window which may be static images or active controls.
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class widget : public std::enable_shared_from_this<widget>
   {
   public:

      widget() {}
      virtual ~widget() {}

      widget(widget&&) = default;
      widget(widget const&) = default;
      widget& operator=(widget&&) = default;
      widget& operator=(widget const&) = default;

      using widget_ptr = std::shared_ptr<widget>;
      using widget_const_ptr = std::shared_ptr<widget const>;

   // image

      virtual rect            limits(basic_context const& ctx) const;
      virtual widget*         hit_test(context const& ctx, point const& p);
      virtual void            draw(context const& ctx);
      virtual void            layout(context const& ctx);
      virtual bool            scroll(context const& ctx, point const& p);

   // control

      virtual widget*         click(context const& ctx, mouse_button btn);
      virtual void            drag(context const& ctx, mouse_button btn);
      virtual bool            key(context const& ctx, key_info const& k);
      virtual bool            text(context const& ctx, text_info const& info);
      virtual bool            cursor(context const& ctx, point const& p);

      virtual bool            focus(focus_request r);
      virtual widget const*   focus() const;
      virtual widget*         focus();
      virtual bool            is_control() const;
   };

   using widget_ptr = std::shared_ptr<widget>;
   using widget_const_ptr = std::shared_ptr<widget const>;

   template <typename Widget>
   inline widget_ptr new_(Widget&& w)
   {
      using widget_type = typename std::decay<Widget>::type;
      return widget_ptr{ new widget_type(std::forward<Widget>(w)) };
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Proxies
   //
   // A widget that encapsulates another widget. The proxy delegates its methods
   // to an enlosed widget.
   ////////////////////////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   class proxy : public virtual widget
   {
   public:

      proxy(Subject&& subject_)
       : _subject(std::move(subject_)) {}

      proxy(Subject const& subject_)
       : _subject(subject_) {}

   // image

      virtual rect            limits(basic_context const& ctx) const;
      virtual widget*         hit_test(context const& ctx, point const& p);
      virtual void            draw(context const& ctx);
      virtual void            layout(context const& ctx);
      virtual bool            scroll(context const& ctx, point const& p);
      virtual void            prepare_subject(context& ctx);

   // control

      virtual widget*         click(context const& ctx, mouse_button btn);
      virtual void            drag(context const& ctx, mouse_button btn);
      virtual bool            key(context const& ctx, key_info const& k);
      virtual bool            text(context const& ctx, text_info const& info);
      virtual bool            cursor(context const& ctx, point const& p);

      virtual bool            focus(focus_request r);
      virtual widget const*   focus() const;
      virtual widget*         focus();
      virtual bool            is_control() const;

   // proxy

      Subject const&          subject() const { return _subject; }
      Subject&                subject() { return _subject; }

   private:

      Subject                 _subject;
   };

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Composites
   //
   // Class for a widget that is composed of other widgets
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class composite : public widget
   {
   public:

   // image

      virtual rect            limits(basic_context const& ctx) const = 0;
      virtual widget*         hit_test(context const& ctx, point const& p);
      virtual void            draw(context const& ctx);
      virtual void            layout(context const& ctx) = 0;
      virtual bool            scroll(context const& ctx, point const& p);

   // control

      virtual widget*         click(context const& ctx, mouse_button btn);
      virtual void            drag(context const& ctx, mouse_button btn);
      virtual bool            key(context const& ctx, key_info const& k);
      virtual bool            text(context const& ctx, text_info const& info);
      virtual bool            cursor(context const& ctx, point const& p);

      virtual bool            focus(focus_request r);
      virtual widget const*   focus() const;
      virtual widget*         focus();
      virtual void            focus(std::size_t index);
      virtual bool            is_control() const;

   // composite

      using composite_type = std::vector<widget_ptr>;

      struct hit_info
      {
         widget*   element;
         rect      bounds;
         int       index;
      };

      composite_type&         elements() { return _elements; }
      composite_type const&   elements() const { return _elements; }
      virtual hit_info        hit_element(context const& ctx, point const& p) const;
      virtual rect            bounds_of(context const& ctx, std::size_t index) const = 0;

   private:

      composite_type          _elements;
   	int			            _focus = -1;
      int                     _drag_tracking = -1;
   };

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Scrollable Views
   //
   // Mixin class for a widget that is scrollable// scroll the rectangle, r into view
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class scrollable
   {
   public:

      virtual bool            scroll_into_view(context const& ctx, rect const& r) = 0;

      struct scrollable_context
      {
         context const* context_ptr;
         scrollable*    scrollable_ptr;

         // scroll the rectangle, r into view
         bool scroll_into_view(rect const& r_)
         {
            if (scrollable_ptr && context_ptr)
            {
               rect r = r_;
               return scrollable_ptr->scroll_into_view(*context_ptr, r);
            }
            return false;
         }
      };

      // find the innermost scrollable class in the context.
      static scrollable_context
      find(context const& ctx);
   };

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Basic Widget
   //
   // The basic widget takes in a function that draws something
   ////////////////////////////////////////////////////////////////////////////////////////////////
   template <typename F>
   class basic_widget : public widget
   {
   public:

      basic_widget(F f)
       : f(f)
      {}

      virtual void
      draw(context const& ctx)
      {
         f(ctx);
      }

   private:

      F f;
   };

   template <typename F>
   inline basic_widget<F> basic(F f)
   {
      return { f };
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // proxy class implementation
   ////////////////////////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   inline rect proxy<Subject>::limits(basic_context const& ctx) const
   {
      return _subject.limits(ctx);
   }

   template <typename Subject>
   inline widget* proxy<Subject>::hit_test(context const& ctx, point const& p)
   {
      context sctx { ctx, &_subject, ctx.bounds };
      prepare_subject(sctx);
      return _subject.hit_test(sctx, p);
   }

   template <typename Subject>
   inline void proxy<Subject>::draw(context const& ctx)
   {
      context sctx { ctx, &_subject, ctx.bounds };
      prepare_subject(sctx);
      _subject.draw(sctx);
   }

   template <typename Subject>
   inline void proxy<Subject>::layout(context const& ctx)
   {
      context sctx { ctx, &_subject, ctx.bounds };
      prepare_subject(sctx);
      _subject.layout(sctx);
   }

   template <typename Subject>
   inline void proxy<Subject>::prepare_subject(context& ctx)
   {
   }

   template <typename Subject>
   inline widget* proxy<Subject>::click(context const& ctx, mouse_button btn)
   {
      context sctx { ctx, &_subject, ctx.bounds };
      prepare_subject(sctx);
      return _subject.click(sctx, btn);
   }

   template <typename Subject>
   inline void proxy<Subject>::drag(context const& ctx, mouse_button btn)
   {
      context sctx { ctx, &_subject, ctx.bounds };
      prepare_subject(sctx);
      _subject.drag(sctx, btn);
   }

   template <typename Subject>
   inline bool proxy<Subject>::key(context const& ctx, key_info const& k)
   {
      context sctx { ctx, &_subject, ctx.bounds };
      prepare_subject(sctx);
      return _subject.key(sctx, k);
   }

   template <typename Subject>
   inline bool proxy<Subject>::text(context const& ctx, text_info const& info)
   {
      context sctx { ctx, &_subject, ctx.bounds };
      prepare_subject(sctx);
      return _subject.text(sctx, info);
   }

   template <typename Subject>
   inline bool proxy<Subject>::cursor(context const& ctx, point const& p)
   {
      context sctx { ctx, &_subject, ctx.bounds };
      prepare_subject(sctx);
      return _subject.cursor(sctx, p);
   }

   template <typename Subject>
   inline bool proxy<Subject>::scroll(context const& ctx, point const& p)
   {
      context sctx { ctx, &_subject, ctx.bounds };
      prepare_subject(sctx);
      return _subject.scroll(sctx, p);
   }

   template <typename Subject>
   inline bool proxy<Subject>::focus(focus_request r)
   {
      return _subject.focus(r);
   }

   template <typename Subject>
   inline widget const* proxy<Subject>::focus() const
   {
      return _subject.focus();
   }

   template <typename Subject>
   inline widget* proxy<Subject>::focus()
   {
      return _subject.focus();
   }

   template <typename Subject>
   inline bool proxy<Subject>::is_control() const
   {
      return _subject.is_control();
   }
}


#endif