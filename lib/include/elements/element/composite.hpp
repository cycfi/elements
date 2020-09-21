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
#include <set>

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
      bool                    click(context const& ctx, mouse_button btn) override;
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

      using weak_element_ptr = std::weak_ptr<elements::element>;

      struct hit_info
      {
         element_ptr          element;
         rect                 bounds   = rect{};
         int                  index    = -1;
      };

      virtual hit_info        hit_element(context const& ctx, point p, bool control) const;
      virtual rect            bounds_of(context const& ctx, std::size_t index) const = 0;
      virtual bool            reverse_index() const { return false; }

                              template <typename F>
      void                    for_each(F&& f, bool reverse = false) const;

   private:

      void                    new_focus(context const& ctx, int index);

      int                     _focus = -1;
      int                     _saved_focus = -1;
      int                     _click_tracking = -1;
      int                     _cursor_tracking = -1;
      std::set<int>           _cursor_hovering;
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

   template <typename F>
   inline void composite_base::for_each(F&& f, bool reverse) const
   {
      if (reverse_index() ^ reverse)
      {
         for (int ix = int(size())-1; ix >= 0; --ix)
            if (!f(ix, at(ix)))
               break;
      }
      else
      {
         for (std::size_t ix = 0; ix < size(); ++ix)
            if (!f(ix, at(ix)))
               break;
      }
   }
}}

#endif
