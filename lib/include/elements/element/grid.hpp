/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_GRID_MARCH_2_2020)
#define ELEMENTS_GRID_MARCH_2_2020

#include <elements/view.hpp>
#include <elements/element/composite.hpp>

namespace cycfi::elements
{
   /**
    * \struct grid_base
    *
    * \brief
    *    Base class for vertical and horizontal grids in a grid layout.
    *    Provides supplies grid coordinates.
    *
    *    `grid_base` provides the core functionality shared by all grid-like
    *    structures, such as being able to determine their size, map index to
    *    a coordinate, and get a number of spans.
    */
   struct grid_base : public composite_base
   {
      virtual std::size_t     grid_size() const = 0;
      virtual float           grid_coord(std::size_t i) const = 0;
      virtual std::size_t     num_spans() const = 0;
   };

   /**
    * \class equal_grid
    *
    * \brief
    *    A specialized grid where grid columns (or rows) have equal size.
    *
    * \tparam Base
    *    The base class on which `equal_grid` is derived from.
    */
   template <typename Base>
   class equal_grid : public Base
   {
   public:

      std::size_t             grid_size() const override
                              { return std::numeric_limits<std::size_t>::max(); }

      float                   grid_coord(std::size_t i) const override
                              { return float(i+1) / this->num_spans(); }
   };

   /**
    * \class range_grid
    *
    * \brief
    *    A specialized grid that lays out cells based on a supplied array of
    *    coordinates in the form of a pointer to a float array (`float
    *    const*`).
    *
    * \tparam Base
    *    The base class on which `range_grid` derived from.
    */
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

   namespace concepts
   {
      template <typename T>
      concept GridContainer = requires (T a, std::size_t ix)
      {
         { a.size() } -> std::same_as<std::size_t>;
         { a[ix] } -> std::convertible_to<float>;
      };
   }

   /**
    * \class container_grid
    *
    * \brief
    *    A specialized grid that lays out cells based on coordinates provided
    *    by a container.
    *
    *    `container_grid` is a grid where the size and coordinates of cells
    *    are defined by the elements of a container that is given at
    *    construction time. `container_grid` does not manage and does not own
    *    the container.
    *
    * \tparam Container
    *    The type of container which provides data for the grid coordinates.
    *    The Container should satisfy the `GridContainer` concept.
    *
    * \tparam Base
    *    The base class on which `container_grid` is derived from. The base
    *    class should satisfy the `Composite` concept.
    */
   template <concepts::GridContainer Container, concepts::Composite Base>
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

   /**
    * \class vgrid_element
    *
    * \brief
    *    A specialized grid class that provides layout features for a
    *    vertical grid structure.
    */
   class vgrid_element : public grid_base
   {
   public:

      view_limits             limits(basic_context const& ctx) const override;
      void                    layout(context const& ctx) override;
      rect                    bounds_of(context const& ctx, std::size_t index) const override;
      std::size_t             num_spans() const override { return _num_spans; }

   private:

      std::vector<float>      _positions;
      mutable std::size_t     _num_spans;
   };

   using vgrid_composite = vector_composite<
         container_grid<std::vector<float>, vgrid_element>
      >;

   /**
    * \brief
    *    Create vertical grid layout, given row coordinates as a C-style
    *    array.
    *
    * \tparam N
    *    The number of rows.
    *
    * \tparam E
    *    A parameter pack representing a variadic template parameter. It
    *    consists of the elements put into grid cells.
    *
    * \param coords
    *    C-style array with coordinates for grid cells.
    *
    * \param elements
    *    Elements intended to place in grid cells.
    *
    * \returns
    *    A vertical grid element represented as an instance of
    *    `array_composite` that is derived from `range_grid<vgrid_element>`.
    *    The row coordinates are provided by the `coords` parameter.
    */
   template <std::size_t N, typename... E>
   inline auto vgrid(float const(&coords)[N], E&&... elements)
   {
      using composite = array_composite<sizeof...(elements), range_grid<vgrid_element>>;
      using container = typename composite::container_type;
      composite r{coords, N};
      r = container{{share(std::forward<E>(elements))...}};
      return r;
   }

   /**
    * \brief
    *    Create a vertical grid layout, given row coordinates as an
    *    `std::array`.
    *
    * \tparam N
    *    The number of rows.
    *
    * \tparam E
    *    A parameter pack representing a variadic template parameter. It
    *    consists of the elements put into grid cells.
    *
    * \param coords
    *    `std::array` with coordinates for grid cells.
    *
    * \param elements
    *    Elements intended to place in grid cells.
    *
    * \returns
    *    A vertical grid element represented as an instance of
    *    `array_composite` that is derived from `range_grid<vgrid_element>`.
    *    The row coordinates are provided by the `coords` parameter.
    */
   template <std::size_t N, typename... E>
   inline auto vgrid(std::array<float, N> const& coords, E&&... elements)
   {
      using plain_array = float const (&)[N];
      return vgrid(plain_array(*coords.data()), std::forward<E>(elements)...);
   }

   /**
    * \brief
    *    Create a vertical grid layout, given the elements to place in
    *    equally sized rows.
    *
    * \tparam E
    *    A parameter pack representing a variadic template parameter. It
    *    consists of the elements put into grid cells.
    *
    * \param elements
    *    Elements intended to place in grid cells.
    *
    * \returns
    *    A vertical grid element represented as an instance of
    *    `array_composite` that is derived from `equal_grid<vgrid_element>.
    *    The rows are equally spaced.
    */
   template <typename... E>
   inline auto vgrid(E&&... elements)
   {
      using composite = array_composite<sizeof...(elements), equal_grid<vgrid_element>>;
      using container = typename composite::container_type;
      composite r{};
      r = container{{share(std::forward<E>(elements))...}};
      return r;
   }

   /**
    * \class hgrid_element
    *
    * \brief
    *    A specialized grid class that provides layout features for a
    *    horizontal grid structure.
    */
   class hgrid_element : public grid_base
   {
   public:

      view_limits             limits(basic_context const& ctx) const override;
      void                    layout(context const& ctx) override;
      rect                    bounds_of(context const& ctx, std::size_t index) const override;
      std::size_t             num_spans() const override { return _num_spans; }

   private:

      std::vector<float>      _positions;
      mutable std::size_t     _num_spans;
   };

   using hgrid_composite = vector_composite<
         container_grid<std::vector<float>, hgrid_element>
      >;

   /**
    * \brief
    *    Create horizontal grid layout, given column coordinates as a C-style
    *    array.
    *
    * \tparam N
    *    The number of columns.
    *
    * \tparam E
    *    A parameter pack representing a variadic template parameter. It
    *    consists of the elements put into grid cells.
    *
    * \param coords
    *    C-style array with coordinates for grid cells.
    *
    * \param elements
    *    Elements intended to place in grid cells.
    *
    * \returns
    *    A horizontal grid element represented as an instance of
    *    `array_composite` that is derived from `range_grid<hgrid_element>`.
    *    The column coordinates are provided by the `coords` parameter.
    */
   template <std::size_t N, typename... E>
   inline auto hgrid(float const(&coords)[N], E&&... elements)
   {
      using composite = array_composite<sizeof...(elements), range_grid<hgrid_element>>;
      using container = typename composite::container_type;
      composite r{coords, N};
      r = container{{share(std::forward<E>(elements))...}};
      return r;
   }

   /**
    * \brief
    *    Create a horizontal grid layout, given column coordinates as an
    *    `std::array`.
    *
    * \tparam N
    *    The number of columns.
    *
    * \tparam E
    *    A parameter pack representing a variadic template parameter. It
    *    consists of the elements put into grid cells.
    *
    * \param coords
    *    `std::array` with coordinates for grid cells.
    *
    * \param elements
    *    Elements intended to place in grid cells.
    *
    * \returns
    *    A horizontal grid element represented as an instance of
    *    `array_composite` that is derived from `range_grid<hgrid_element>`.
    *    The column coordinates are provided by the `coords` parameter.
    */
   template <std::size_t N, typename... E>
   inline auto hgrid(std::array<float, N> const& coords, E&&... elements)
   {
      using plain_array = float const (&)[N];
      return hgrid(plain_array(*coords.data()), std::forward<E>(elements)...);
   }

   /**
    * \brief
    *    Create a horizontal grid layout, given the elements to place in
    *    equally sized grid cells.
    *
    * \tparam E
    *    A parameter pack representing a variadic template parameter. It
    *    consists of the elements put into grid cells.
    *
    * \param elements
    *    Elements intended to place in grid cells.
    *
    * \returns
    *    A horizontal grid element represented as an instance of
    *    `array_composite` that is derived from `equal_grid<hgrid_element>.
    *    The columns are equally spaced.
    */
   template <typename... E>
   inline auto hgrid(E&&... elements)
   {
      using composite = array_composite<sizeof...(elements), equal_grid<hgrid_element>>;
      using container = typename composite::container_type;
      composite r{};
      r = container{{share(std::forward<E>(elements))...}};
      return r;
   }

   /**
    * \brief
    *    Creates an equally spaced grid of floats.
    *
    *    This function generates a std::array of N floats, where each element
    *    is equally spaced between 0 and 1. The spacing between the elements
    *    is 1 / N.
    *
    * \tparam N
    *    The size of the array to be created.
    *
    * \return
    *    A std::array of N floats, where each element is equally spaced
    *    between 0 and 1.
    */
   template <std::size_t N>
   inline std::array<float, N> make_equal_grid()
   {
      std::array<float, N> grid;
      std::generate(grid.begin(), grid.end(),
         [n = 1]() mutable
         {
            return float(n++) / N;
         }
      );
      return grid;
   }
}

#endif
