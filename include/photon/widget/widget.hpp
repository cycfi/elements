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
#include <array>

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
   };

   using widget_ptr = std::shared_ptr<widget>;
   using widget_const_ptr = std::shared_ptr<widget const>;

   template <typename Widget>
   inline widget_ptr new_(Widget&& w)
   {
      using widget_type = typename std::decay<Widget>::type;
      return std::make_shared<widget_type>(std::forward<Widget>(w));
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Proxies
   //
   // A widget that encapsulates another widget. The proxy delegates its methods
   // to an enlosed widget.
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class proxy_base : public widget
   {
   public:

   // image

      virtual rect            limits(basic_context const& ctx) const;
      virtual widget*         hit_test(context const& ctx, point p);
      virtual void            draw(context const& ctx);
      virtual void            layout(context const& ctx);
      virtual bool            scroll(context const& ctx, point p);
      virtual void            prepare_subject(context& ctx);

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

   // proxy

      virtual widget const&  subject() const = 0;
      virtual widget&        subject() = 0;
   };

   template <typename Subject, typename Base = proxy_base>
   class proxy : public Base
   {
   public:

      template <typename... T>
      proxy(Subject&& subject_, T const&... args)
       : Base(args...)
       , _subject(std::move(subject_)) {}

      template <typename... T>
      proxy(Subject const& subject_, T const&... args)
       : Base(args...)
       , _subject(subject_) {}

      virtual widget const&   subject() const { return _subject; }
      virtual widget&         subject() { return _subject; }

   private:

      Subject                 _subject;
   };

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Composites
   //
   // Class for a widget that is composed of other widgets
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class composite_base : public widget
   {
   public:

   // image

      virtual rect            limits(basic_context const& ctx) const = 0;
      virtual widget*         hit_test(context const& ctx, point p);
      virtual void            draw(context const& ctx);
      virtual void            layout(context const& ctx) = 0;
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
      virtual void            focus(std::size_t index);
      virtual bool            is_control() const;

   // composite

      struct hit_info
      {
         widget*   element;
         rect      bounds;
         int       index;
      };

      using iterator = widget_ptr*;
      using const_iterator = widget_ptr const*;

      virtual hit_info        hit_element(context const& ctx, point p) const;
      virtual rect            bounds_of(context const& ctx, std::size_t index) const = 0;

      virtual std::size_t     size() const = 0;
      virtual iterator        begin() = 0;
      virtual const_iterator  begin() const = 0;

      iterator                end()                            { return begin()+size(); }
      const_iterator          end() const                      { return begin()+size(); }
      bool                    empty() const                    { return size() == 0; }
      widget_ptr&             operator[](std::size_t ix)       { return begin()[ix]; }
      widget_ptr const&       operator[](std::size_t ix) const { return begin()[ix]; }

   private:

   	int			            _focus = -1;
      int                     _drag_tracking = -1;
      hit_info                _click_info;
   };

   template <typename Container, typename Base>
   class composite : public Base
   {
   public:

      using iterator = typename Base::iterator;
      using const_iterator = typename Base::const_iterator;

      virtual std::size_t     size() const   { return elements.size(); };
      virtual iterator        begin()        { return &elements[0]; }
      virtual const_iterator  begin() const  { return &elements[0]; }

      Container               elements;
   };

   template <size_t N, typename Base>
   using array_composite = composite<std::array<widget_ptr, N>, Base>;

   template <typename Base>
   using vector_composite = composite<std::vector<widget_ptr>, Base>;

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Scrollable Views
   //
   // Mixin class for a widget that is scrollable// scroll the rectangle, r into view
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class scrollable
   {
   public:

      struct scrollable_context
      {
         context const* context_ptr;
         scrollable*    scrollable_ptr;

         // scroll the rectangle, r into view
         bool scroll_into_view(rect r_)
         {
            if (scrollable_ptr && context_ptr)
            {
               rect r = r_;
               return scrollable_ptr->scroll_into_view(*context_ptr, r);
            }
            return false;
         }
      };

      virtual bool               scroll_into_view(context const& ctx, rect r) = 0;
      static scrollable_context  find(context const& ctx);
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
}


#endif