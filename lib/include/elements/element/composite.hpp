/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_COMPOSITE_APRIL_10_2016)
#define ELEMENTS_COMPOSITE_APRIL_10_2016

#include <elements/element/element.hpp>
#include <elements/element/proxy.hpp>
#include <elements/support/context.hpp>

#include <vector>
#include <array>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Composites
   //
   // Class for a element that is composed of other elements
   ////////////////////////////////////////////////////////////////////////////
   class container
   {
   public:

      virtual                 ~container() = default;

      virtual std::size_t     size() const = 0;
      bool                    empty() const { return size() == 0; }
      virtual element&        at(std::size_t ix) const = 0;
   };

   class context;
   class composite_base : public element, public container
   {
   public:

   // Display

      view_limits             limits(basic_context const& ctx) const override = 0;
      element*                hit_test(context const& ctx, point p) override;
      void                    draw(context const& ctx) override;
      void                    layout(context const& ctx) override = 0;
      void                    refresh(context const& ctx, element& element, int outward = 0) override;

      using element::refresh;

   // Control

      bool                    wants_control() const override;
      element*                click(context const& ctx, mouse_button btn) override;
      void                    drag(context const& ctx, mouse_button btn) override;
      bool                    key(context const& ctx, key_info k) override;
      bool                    text(context const& ctx, text_info info) override;
      bool                    cursor(context const& ctx, point p, cursor_tracking status) override;
      bool                    scroll(context const& ctx, point dir, point p) override;

      bool                    wants_focus() const override;
      void                    begin_focus() override;
      void                    end_focus() override;
      element const*          focus() const override;
      element*                focus() override;
      void                    focus(std::size_t index);
      virtual void            reset();

   // Composite

      struct hit_info
      {
         using weak_ptr = std::weak_ptr<elements::element>;

         weak_ptr             element;
         rect                 bounds   = rect{};
         int                  index    = -1;
      };

      virtual hit_info        hit_element(context const& ctx, point p) const;
      virtual rect            bounds_of(context const& ctx, std::size_t index) const = 0;
      virtual bool            reverse_index() const { return false; }

   private:

      void                    new_focus(context const& ctx, int index);

      int                     _focus = -1;
      int                     _saved_focus = -1;
      int                     _drag_tracking = -1;
      hit_info                _click_info;
      hit_info                _cursor_info;
   };

   ////////////////////////////////////////////////////////////////////////////
   template <typename Container, typename Base>
   class composite : public Base, public Container
   {
   public:

      using base_type = Base;
      using container_type = Container;
      using Base::Base;
      using Container::Container;
      using Container::operator=;

      std::size_t             size() const override;
      element&                at(std::size_t ix) const override;

      using Container::empty;
   };

   template <size_t N, typename Base>
   using array_composite = composite<std::array<element_ptr, N>, Base>;

   template <typename Base>
   using vector_composite = composite<std::vector<element_ptr>, Base>;

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

      std::size_t             size() const override;
      element&                at(std::size_t ix) const override;

   private:

      std::size_t             _first;
      std::size_t             _last;
      container&              _container;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Inlines
   ////////////////////////////////////////////////////////////////////////////
   template <typename Container, typename Base>
   inline std::size_t composite<Container, Base>::size() const
   {
      return Container::size();
   }

   template <typename Container, typename Base>
   inline element& composite<Container, Base>::at(std::size_t ix) const
   {
      return *(*this)[ix].get();
   }

   template <typename Base>
   inline std::size_t range_composite<Base>::size() const
   {
      return _last - _first;
   }

   template <typename Base>
   inline element& range_composite<Base>::at(std::size_t ix) const
   {
      return _container.at(_first + ix);
   }
}}

#endif
