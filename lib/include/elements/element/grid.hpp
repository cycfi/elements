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
      virtual float           grid_coord(std::size_t i) const = 0;
      virtual std::size_t     num_spans() const = 0;
   };

   template <typename Base>
   class equal_grid : public Base
   {
   public:

      std::size_t             grid_size() const override
                              { return std::numeric_limits<std::size_t>::max(); }

      float                   grid_coord(std::size_t i) const override
                              { return float(i+1) / this->num_spans(); }
   };

   template <typename Base>
   class range_grid : public Base
   {
   public:
                              range_grid(float const* coords, std::size_t size)
                               : _coords(coords), _size(size)
                              {}

      std::size_t             grid_size() const override    { return _size; }
      float                   grid_coord(std::size_t i) const override
                              { return (i < _size)? _coords[i] : 1.0f; }

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
      float                   grid_coord(std::size_t i) const override
                              { return (i < _container.size())? _container[i] : 1.0f; }

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
      std::size_t             num_spans() const override { return _num_spans; }

   private:

      std::vector<float>      _positions;
      std::size_t             _num_spans;
   };

   using vgrid_composite = vector_composite<
         container_grid<std::vector<float>, vgrid_element>
      >;

   template <std::size_t N, typename... E>
   inline auto vgrid(float const(&coords)[N], E&&... elements)
   {
      using composite = array_composite<sizeof...(elements), range_grid<vgrid_element>>;
      using container = typename composite::container_type;
      composite r{ coords, N };
      r = container{{ share(std::forward<E>(elements))... }};
      return r;
   }

   template <std::size_t N, typename... E>
   inline auto vgrid(std::array<float, N> const& coords, E&&... elements)
   {
      using plain_array = float const (&)[N];
      return vgrid(plain_array(*coords.data()), std::forward<E>(elements)...);
   }

   template <typename... E>
   inline auto vgrid(E&&... elements)
   {
      using composite = array_composite<sizeof...(elements), equal_grid<vgrid_element>>;
      using container = typename composite::container_type;
      composite r{};
      r = container{{ share(std::forward<E>(elements))... }};
      return r;
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
      std::size_t             num_spans() const override { return _num_spans; }

   private:

      std::vector<float>      _positions;
      std::size_t             _num_spans;
   };

   using hgrid_composite = vector_composite<
         container_grid<std::vector<float>, hgrid_element>
      >;

   template <std::size_t N, typename... E>
   inline auto hgrid(float const(&coords)[N], E&&... elements)
   {
      using composite = array_composite<sizeof...(elements), range_grid<hgrid_element>>;
      using container = typename composite::container_type;
      composite r{ coords, N };
      r = container{{ share(std::forward<E>(elements))... }};
      return r;
   }

   template <std::size_t N, typename... E>
   inline auto hgrid(std::array<float, N> const& coords, E&&... elements)
   {
      using plain_array = float const (&)[N];
      return hgrid(plain_array(*coords.data()), std::forward<E>(elements)...);
   }

   template <typename... E>
   inline auto hgrid(E&&... elements)
   {
      using composite = array_composite<sizeof...(elements), equal_grid<hgrid_element>>;
      using container = typename composite::container_type;
      composite r{};
      r = container{{ share(std::forward<E>(elements))... }};
      return r;
   }
}}

#endif
