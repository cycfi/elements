/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_LAYER_APRIL_16_2016)
#define ELEMENTS_LAYER_APRIL_16_2016

#include <elements/element/composite.hpp>
#include <algorithm>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Layer
   ////////////////////////////////////////////////////////////////////////////
   class layer_element : public composite_base
   {
   public:

      view_limits             limits(basic_context const& ctx) const override;
      void                    layout(context const& ctx) override;
      void                    draw(context const& ctx) override;
      hit_info                hit_element(context const& ctx, point p, bool control) const override;
      rect                    bounds_of(context const& ctx, std::size_t index) const override;
      void                    begin_focus() override;
      bool                    reverse_index() const override { return true; }

      using composite_base::focus;

   private:

      void                    focus_top();
      point                   _previous_size;
   };

   using layer_composite = vector_composite<layer_element>;

   template <typename... E>
   inline auto layer(E&&... elements)
   {
      using composite = array_composite<sizeof...(elements), layer_element>;
      using container = typename composite::container_type;
      composite r{};
      r = container{{ share(std::forward<E>(elements))... }};
      std::reverse(r.begin(), r.end());
      return r;
   }

   ////////////////////////////////////////////////////////////////////////////
   // Deck
   ////////////////////////////////////////////////////////////////////////////
   class deck_element : public layer_element
   {
   public:
                           deck_element()
                            : _selected_index(0)
                           {}

      void                 draw(context const& ctx) override;
      void                 refresh(context const& ctx, element& element, int outward = 0) override;
      hit_info             hit_element(context const& ctx, point p, bool control) const override;
      void                 begin_focus() override;

      using element::refresh;
      using composite_base::focus;

      void                 select(std::size_t index);
      std::size_t          selected() const { return _selected_index; }

   private:

      std::size_t          _selected_index;
   };

   using deck_composite = vector_composite<deck_element>;

   template <typename... E>
   inline auto deck(E&&... elements)
   {
      using composite = array_composite<sizeof...(elements), deck_element>;
      using container = typename composite::container_type;
      composite r{};
      r = container{{ share(std::forward<E>(elements))... }};
      return r;
   }
}}

#endif
