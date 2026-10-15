/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_SPLIT_OCTOBER_10_2026)
#define ELEMENTS_SPLIT_OCTOBER_10_2026

#include <elements/element/composite.hpp>
#include <vector>

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Splits: tiles whose boundaries the user drags. Laid out as a tile until
   // a boundary is dragged; from then on, each child keeps its share of the
   // split's extent, within its limits, as the split is resized. Dragging a
   // boundary trades space between the two children beside it, within both
   // children's limits.
   ////////////////////////////////////////////////////////////////////////////
   template <axis Axis>
   class split_element : public composite_base
   {
   public:

      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;
      void                    layout(context const& ctx) override;
      rect                    bounds_of(context const& ctx, std::size_t index) const override;

      element*                hit_test(context const& ctx, point p, bool leaf, bool control) override;
      bool                    wants_control() const override { return true; }
      bool                    click(context const& ctx, mouse_button btn) override;
      void                    drag(context const& ctx, mouse_button btn) override;
      bool                    cursor(context const& ctx, point p, cursor_tracking status) override;
      std::string             class_name() const override;

      // The width of the area around a boundary that grabs it.
      float                   grip = 6;

      // The children's shares of the extent, once a boundary is dragged.
      std::vector<float> const& shares() const { return _shares; }
      void                    shares(std::vector<float> shares_);

   private:

      int                     boundary_at(context const& ctx, point p) const;

      std::vector<float>      _tiles;     // end offset of each child
      std::vector<float>      _shares;    // empty while laid out as a tile
      int                     _dragging = -1;
      float                   _drag_start = 0;
      std::vector<float>      _start_sizes;
   };

   using hsplit_element = split_element<axis::x>;
   using vsplit_element = split_element<axis::y>;

   using hsplit_composite = vector_composite<hsplit_element>;
   using vsplit_composite = vector_composite<vsplit_element>;

   template <concepts::Element... E>
   inline auto hsplit(E&&... elements)
   {
      using composite = array_composite<sizeof...(elements), hsplit_element>;
      using container = typename composite::container_type;
      composite r{};
      r = container{{share(std::forward<E>(elements))...}};
      return r;
   }

   template <concepts::Element... E>
   inline auto vsplit(E&&... elements)
   {
      using composite = array_composite<sizeof...(elements), vsplit_element>;
      using container = typename composite::container_type;
      composite r{};
      r = container{{share(std::forward<E>(elements))...}};
      return r;
   }
}

#endif
