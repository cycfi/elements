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

      using widget_ptr = std::shared_ptr<widget>;
      using widget_const_ptr = std::shared_ptr<widget const>;

      widget() {}
      ~widget() {}

      widget(widget const&) = delete;
      widget& operator=(widget const&) = delete;

   // image

      virtual rect            limits(basic_context const& ctx) const;
      virtual widget*         hit_test(context const& ctx, point const& p);
      virtual void            draw(context const& ctx);
      virtual void            layout(context const& ctx);

   // control

      virtual widget*         click(context const& ctx, mouse_button btn);
      virtual void            drag(context const& ctx, mouse_button btn);
      virtual bool            key(context const& ctx, key_info const& k);
      virtual bool            cursor(context const& ctx, point const& p);
      virtual bool            focus(focus_request r);
      virtual widget const*   focus() const;
   };

   using widget_ptr = std::shared_ptr<widget>;
   using widget_const_ptr = std::shared_ptr<widget const>;

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Proxies
   //
   // A widget that encapsulates another widget. The proxy delegates its methods
   // to an enlosed widget. The proxy acts as a single atomic element instead
   // of a composite.
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class proxy : public widget {

   public:

      proxy(widget_ptr subject_)
       : _subject(subject_) {}
      ~proxy() {}

   // image

      virtual rect            limits(basic_context const& ctx) const;
      virtual widget*         hit_test(context const& ctx, point const& p);
      virtual void            draw(context const& ctx);
      virtual void            layout(context const& ctx);
      virtual void            prepare_subject(context const& ctx);

   // control

      virtual widget*         click(context const& ctx, mouse_button btn);
      virtual void            drag(context const& ctx, mouse_button btn);
      virtual bool            key(context const& ctx, key_info const& k);
      virtual bool            cursor(context const& ctx, point const& p);
      virtual bool            focus(focus_request r);
      virtual widget const*   focus() const;

   // proxy

      widget_ptr              subject() const { return _subject; }
      void                    subject(widget_ptr subject_) { _subject = subject_; }

   private:

      widget_ptr              _subject;
   };

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Composites
   //
   // Class for a widget that is composed of other widgets
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class composite : public widget
   {
   public:

      composite() : _focus(-1) {}
      ~composite() {}

   // image

      virtual rect            limits(basic_context const& ctx) const = 0;
      virtual widget*         hit_test(context const& ctx, point const& p);
      virtual void            draw(context const& ctx);
      virtual void            layout(context const& ctx) = 0;

   // control

      virtual widget*         click(context const& ctx, mouse_button btn);
      virtual void            drag(context const& ctx, mouse_button btn);
      virtual bool            key(context const& ctx, key_info const& k);
      virtual bool            cursor(context const& ctx, point const& p);
      virtual bool            focus(focus_request r);
      virtual widget const*   focus() const;

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
   	int			            _focus;
   };

}

#endif