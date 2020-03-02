/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_GRID_MARCH_2_2020)
#define ELEMENTS_GRID_MARCH_2_2020

#include <elements/element/composite.hpp>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Vertical Grids
   ////////////////////////////////////////////////////////////////////////////
   class vgrid_element : public composite_base
   {
   public:
                              vgrid_element(float const* grid_)
                               : _grid(grid_)
                              {}

      view_limits             limits(basic_context const& ctx) const override;
      void                    layout(context const& ctx) override;
      rect                    bounds_of(context const& ctx, std::size_t index) const override;

   private:

      float const*            _grid;
   };

   using vgrid_composite = vector_composite<vgrid_element>;

   template <typename... E>
   inline auto vgrid(float (&grid_)[sizeof...(E)], E&&... elements)
   {
      using composite = array_composite<sizeof...(elements), vgrid_element>;
      using container = typename composite::container_type;
      composite r{ grid_ };
      r = container{{ share(std::forward<E>(elements))... }};
      return r;
   }

   ////////////////////////////////////////////////////////////////////////////
   // Horizontal Tiles
   ////////////////////////////////////////////////////////////////////////////
   class hgrid_element : public composite_base
   {
   public:
                              hgrid_element(float const* grid_)
                               : _grid(grid_)
                              {}

      view_limits             limits(basic_context const& ctx) const override;
      void                    layout(context const& ctx) override;
      rect                    bounds_of(context const& ctx, std::size_t index) const override;

   private:

      float const*            _grid;
   };

   using hgrid_composite = vector_composite<hgrid_element>;

   template <typename... E>
   inline auto hgrid(float (&grid_)[sizeof...(E)], E&&... elements)
   {
      using composite = array_composite<sizeof...(elements), hgrid_element>;
      using container = typename composite::container_type;
      composite r{ grid_ };
      r = container{{ share(std::forward<E>(elements))... }};
      return r;
   }
}}

#endif
