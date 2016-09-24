/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_COMPOSITE_APRIL_10_2016)
#define PHOTON_GUI_LIB_COMPOSITE_APRIL_10_2016

#include <photon/widget/widget.hpp>
#include <vector>
#include <array>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Composites
   //
   // Class for a widget that is composed of other widgets
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class container
   {
   public:

      virtual std::size_t     size() const = 0;
      bool                    empty() const                    { return size() == 0; }
      virtual widget&         at(std::size_t ix) const = 0;
   };

   class composite_base : public widget, public container
   {
   public:

   // Image

      virtual widget_limits   limits(basic_context const& ctx) const = 0;
      virtual widget*         hit_test(context const& ctx, point p);
      virtual void            draw(context const& ctx);
      virtual void            layout(context const& ctx) = 0;
      virtual bool            scroll(context const& ctx, point dir, point p);

   // Control

      virtual widget*         click(context const& ctx, mouse_button btn);
      virtual void            drag(context const& ctx, mouse_button btn);
      virtual bool            key(context const& ctx, key_info k);
      virtual bool            text(context const& ctx, text_info info);
      virtual bool            cursor(context const& ctx, point p, cursor_tracking status);

      virtual bool            focus(focus_request r);
      virtual widget const*   focus() const;
      virtual widget*         focus();
      virtual void            focus(std::size_t index);
      virtual bool            is_control() const;
      virtual void            idle(basic_context const& ctx);

   // Composite

      struct hit_info
      {
         widget*   element    = nullptr;
         rect      bounds     = rect{};
         int       index      = -1;
      };

      virtual hit_info        hit_element(context const& ctx, point p) const;
      virtual rect            bounds_of(context const& ctx, std::size_t index) const = 0;

   private:

      int                     _focus = -1;
      int                     _drag_tracking = -1;
      hit_info                _click_info;
      hit_info                _cursor_info;
   };

   ////////////////////////////////////////////////////////////////////////////////////////////////
   template <typename Container, typename Base>
   class composite : public Base, public Container
   {
   public:

      using Container::Container;
      using Container::operator=;

      virtual std::size_t     size() const               { return Container::size(); };
      virtual widget&         at(std::size_t ix) const   { return *(*this)[ix].get(); }
   };

   template <size_t N, typename Base>
   using array_composite = composite<std::array<widget_ptr, N>, Base>;

   template <typename Base>
   using vector_composite = composite<std::vector<widget_ptr>, Base>;

   template <typename Base>
   class range_composite : public Base
   {
   public:
                              range_composite(
                                 container&     container_
                               , std::size_t    first
                               , std::size_t    last
                              )
                               : _first(first)
                               , _last(last)
                               , _container(container_)
                              {}

      virtual std::size_t     size() const               { return _last - _first; };
      virtual widget&         at(std::size_t ix) const   { return _container.at(_first + ix); }

   private:

      std::size_t             _first;
      std::size_t             _last;
      container&              _container;
   };
}

#endif