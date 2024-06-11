/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_LAYER_APRIL_16_2016)
#define ELEMENTS_LAYER_APRIL_16_2016

#include <elements/element/composite.hpp>
#include <algorithm>

namespace cycfi::elements
{
   /**
    * @class layer_element
    *
    * @brief
    *    A class which represents an element in a layer. The layer_element is
    *    a composite that allows groups of elements to be placed in the
    *    z-axis. Higher-level elements obscure or hide lower-level elements.
    */
   class layer_element : public composite_base
   {
   public:

      view_limits             limits(basic_context const& ctx) const override;
      void                    layout(context const& ctx) override;
      void                    draw(context const& ctx) override;
      hit_info                hit_element(context const& ctx, point p, bool control) const override;
      rect                    bounds_of(context const& ctx, std::size_t index) const override;
      bool                    reverse_index() const override { return true; }

      using composite_base::focus;

   private:

      point                   _previous_size;
   };

   /**
    * @brief
    *    `layer_composite` is a composite with indeterminate (dynamic)
    *    number of elements
    */
   using layer_composite = vector_composite<layer_element>;

   /**
    * @brief
    *    Create a layer from provided elements. The layer contains group of
    *    elements placed in z-axis. Higher-level elements obscure or hide
    *    lower-level elements.
    *
    * @tparam E
    *    Variadic template parameter for the types of the elements.
    *
    * @param elements
    *    The elements to be added to the layer (a parameter pack).
    *
    * @return
    *    A reverse layer composite of the provided elements (top-most element
    *    last).
    */
   template <typename... E>
   inline auto layer(E&&... elements)
   {
      using composite = array_composite<sizeof...(elements), layer_element>;
      using container = typename composite::container_type;
      composite r{};
      r = container{{share(std::forward<E>(elements))...}};
      std::reverse(r.begin(), r.end());
      return r;
   }

   /**
    * @class deck_element
    *
    * @brief
    *    Represents an element in a deck, which inherits from the
    *    `layer_element` class. Elements are placed in the z-axis. Unlike
    *    layers, only the selected element is active (top-most by default).
    */
   class deck_element : public layer_element
   {
   public:
                           deck_element()
                            : _selected_index(0)
                           {}

      void                 draw(context const& ctx) override;
      void                 refresh(context const& ctx, element& element, int outward = 0) override;
      void                 in_context_do(context const& ctx, element& e, context_function f) override;
      hit_info             hit_element(context const& ctx, point p, bool control) const override;
      void                 begin_focus(focus_request req) override;

      using element::refresh;
      using composite_base::focus;

      void                 select(std::size_t index);
      std::size_t          selected() const { return _selected_index; }

   private:

      std::size_t          _selected_index;
   };

   /**
    * @brief
    *    `deck_composite` is a composite with indeterminate (dynamic)
    *    number of elements
    */
   using deck_composite = vector_composite<deck_element>;

   /**
    * @brief
    *    Create a deck from provided elements. The deck contains a group of
    *    elements placed in z-axis. Unlike layers, only the selected element
    *    is active (top-most by default).
    *
    * @tparam E
    *    Variadic template parameter for the types of the elements.
    *
    * @param elements
    *    The elements to be added to the deck (a parameter pack).
    *
    * @return
    *    A deck composite of the provided elements.
    */
   template <typename... E>
   inline auto deck(E&&... elements)
   {
      using composite = array_composite<sizeof...(elements), deck_element>;
      using container = typename composite::container_type;
      composite r{};
      r = container{{share(std::forward<E>(elements))...}};
      return r;
   }
}

#endif
