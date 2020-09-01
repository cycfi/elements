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
   // grid_base: Base class for vertical and horizontal grids, supplies grid
   // coordinates in the form of a float const*
   ////////////////////////////////////////////////////////////////////////////
   struct grid_base : public composite_base
   {
      virtual std::size_t     grid_size() const = 0;
      virtual float const*    grid() const = 0;
   };

   template <typename Base>
   class range_grid : public Base
   {
   public:
                              range_grid(float const* coords, std::size_t size)
                               : _coords(coords), _size(size)
                              {}

      std::size_t             grid_size() const override    { return _size; }
      float const*            grid() const override         { return _coords; }

   private:

      float const*            _coords;
      std::size_t             _size;
   };

   template <typename Container, typename Base>
   class container_grid : public Base
   {
   public:
                              container_grid(Container const& container)
                               : _container(container)
                              {}

      std::size_t             grid_size() const override    { return _container.size(); }
      float const*            grid() const override         { return _container.data(); }

   private:

      Container const&        _container;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Vertical Grids
   ////////////////////////////////////////////////////////////////////////////
   class vgrid_element : public grid_base
   {
   public:

      view_limits             limits(basic_context const& ctx) const override;
      void                    layout(context const& ctx) override;
      rect                    bounds_of(context const& ctx, std::size_t index) const override;
   };

   using vgrid_composite = vector_composite<
         container_grid<std::vector<float>, vgrid_element>
      >;

#if defined(_MSC_VER)
   template <typename... E, std::size_t N = sizeof...(E)>
   inline auto vgrid(float const(&coords)[N], E&&... elements)
#else
   template <typename... E>
   inline auto vgrid(float const(&coords)[sizeof...(E)], E&&... elements)
#endif
   {
      using composite = array_composite<sizeof...(elements), range_grid<vgrid_element>>;
      using container = typename composite::container_type;
      composite r{ coords, sizeof...(E) };
      r = container{{ share(std::forward<E>(elements))... }};
      return r;
   }

   template <typename... E>
   inline auto vgrid(std::array<float, sizeof...(E)> const& coords, E&&... elements)
   {
      using plain_array = float const (&)[sizeof...(E)];
      return vgrid(plain_array(*coords.data()), std::forward<E>(elements)...);
   }

   ////////////////////////////////////////////////////////////////////////////
   // Horizontal Tiles
   ////////////////////////////////////////////////////////////////////////////
   class hgrid_element : public grid_base
   {
   public:

      view_limits             limits(basic_context const& ctx) const override;
      void                    layout(context const& ctx) override;
      rect                    bounds_of(context const& ctx, std::size_t index) const override;
   };

   using hgrid_composite = vector_composite<
         container_grid<std::vector<float>, hgrid_element>
      >;

#if defined(_MSC_VER)
   template <typename... E, std::size_t N = sizeof...(E)>
   inline auto hgrid(float const(&coords)[N], E&&... elements)
#else
   template <typename... E>
   inline auto hgrid(float const(&coords)[sizeof...(E)], E&&... elements)
#endif
   {
      using composite = array_composite<sizeof...(elements), range_grid<hgrid_element>>;
      using container = typename composite::container_type;
      composite r{ coords, sizeof...(E) };
      r = container{{ share(std::forward<E>(elements))... }};
      return r;
   }

   template <typename... E>
   inline auto hgrid(std::array<float, sizeof...(E)> const& coords, E&&... elements)
   {
      using plain_array = float const (&)[sizeof...(E)];
      return hgrid(plain_array(*coords.data()), std::forward<E>(elements)...);
   }
}}

#endif
