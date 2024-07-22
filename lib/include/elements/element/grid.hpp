/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_GRID_MARCH_2_2020)
#define ELEMENTS_GRID_MARCH_2_2020

#include <elements/view.hpp>
#include <elements/element/composite.hpp>
#include <elements/element/tracker.hpp>

namespace cycfi::elements
{
   //--------------------------------------------------------------------------
   // grid_base
   //--------------------------------------------------------------------------

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
      virtual float           get_grid_coord(std::size_t i) const = 0;
      virtual void            set_grid_coord(std::size_t i, float coord) const = 0;
      virtual std::size_t     num_spans() const = 0;
      virtual bool            is_fixed() const = 0;
   };

   //--------------------------------------------------------------------------
   // equal_grid
   //--------------------------------------------------------------------------

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

      float                   get_grid_coord(std::size_t i) const override;
      void                    set_grid_coord(std::size_t /*i*/, float /*coord*/) const override;
      bool                    is_fixed() const override;
   };

   //--------------------------------------------------------------------------
   // range_grid
   //--------------------------------------------------------------------------

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
    *
    * \tparam fixed
    *    A boolean template parameter that determines whether the grid
    *    coordinates are fixed (true) or modifiable (false). By default, it
    *    is set to true, indicating fixed coordinates.
    */
   template <typename Base, bool fixed = true>
   class range_grid : public Base
   {
   public:

      using coords_type = std::conditional_t<fixed, float const*, float*>;

                              range_grid(coords_type coords, std::size_t size);

      float                   get_grid_coord(std::size_t i) const override;
      void                    set_grid_coord(std::size_t i, float coord) const override;
      bool                    is_fixed() const override;

   private:

      coords_type             _coords;
      std::size_t             _size;
   };

   //--------------------------------------------------------------------------
   // container_grid
   //--------------------------------------------------------------------------

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
    *
    * \tparam fixed
    *    A boolean template parameter that determines whether the grid
    *    coordinates are fixed (true) or modifiable (false). By default, it
    *    is set to true, indicating fixed coordinates.
    */
   template <concepts::GridContainer Container, concepts::Composite Base, bool fixed = true>
   class container_grid : public Base
   {
   public:

      using coords_type = std::conditional_t<fixed, Container const&, Container&>;

                              container_grid(coords_type coords);

      float                   get_grid_coord(std::size_t i) const override;
      void                    set_grid_coord(std::size_t i, float coord) const override;
      bool                    is_fixed() const override;

   private:

      coords_type             _coords;
   };

   //--------------------------------------------------------------------------
   // vgrid and vgrid_composite
   //--------------------------------------------------------------------------

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

   template <std::size_t N, concepts::Element... E>
   inline auto vgrid(float const(&coords)[N], E&&... elements);

   template <std::size_t N, concepts::Element... E>
   inline auto vgrid(std::array<float, N> const& coords, E&&... elements);

   template <concepts::Element... E>
   inline auto vgrid(E&&... elements);

   //--------------------------------------------------------------------------
   // hgrid and hgrid_composite
   //--------------------------------------------------------------------------

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

   template <std::size_t N, concepts::Element... E>
   inline auto hgrid(float const(&coords)[N], E&&... elements);

   template <std::size_t N, concepts::Element... E>
   inline auto hgrid(std::array<float, N> const& coords, E&&... elements);

   template <concepts::Element... E>
   inline auto hgrid(E&&... elements);


   //--------------------------------------------------------------------------
   // vdivider and adjustable_vgrid
   //--------------------------------------------------------------------------
   struct divider_tracker_info : tracker_info
   {
      using tracker_info::tracker_info;

      std::size_t          _index;
      view_limits          _limits0;
      view_limits          _limits1;
      float                _end_pos;
   };

   /**
    *
    * \class
    *    vdivider_element
    *
    *    The `vdivider_element` acts as a proxy that wraps a subject element
    *    and adding a draggable vertical divider to its top side. This allows
    *    for dynamic resizing of both the subject and the adjacent element
    *    above it within a vertical grid layout. The subject is expected to
    *    be placed inside a vertical grid and will not work outside of it.
    */
   class vdivider_element : public tracker<proxy_base, divider_tracker_info>
   {
   public:

      using tracker = tracker<proxy_base, divider_tracker_info>;

      element*             hit_test(context const& ctx, point p, bool leaf, bool control) override;
      bool                 click(context const& ctx, mouse_button btn) override;
      bool                 cursor(context const& ctx, point p, cursor_tracking status) override;
      void                 begin_tracking(context const& ctx, tracker_info& track_info) override;
      void                 keep_tracking(context const& ctx, tracker_info& track_info) override;
   };

   template <concepts::Element Subject>
   inline proxy<remove_cvref_t<Subject>, vdivider_element>
   vdivider(Subject&& subject);

   template <std::size_t N, concepts::Element... E>
   inline auto adjustable_vgrid(float(&coords)[N], E&&... elements);

   template <std::size_t N, concepts::Element... E>
   inline auto adjustable_vgrid(std::array<float, N>& coords, E&&... elements);

   //--------------------------------------------------------------------------
   // hdivider and adjustable_hgrid
   //--------------------------------------------------------------------------

   /**
    * \class
    *    hdivider_element
    *
    *    The `hdivider_element` acts as a proxy that wraps a subject element
    *    and adding a draggable horizontal divider to its left side. This
    *    allows for dynamic resizing of both the subject and the adjacent
    *    element on the left within a horizontal grid layout. The subject is
    *    expected to be placed inside a horizontal grid and will not work
    *    outside of it.
    */
   class hdivider_element : public tracker<proxy_base, divider_tracker_info>
   {
   public:

      using tracker = tracker<proxy_base, divider_tracker_info>;

      element*             hit_test(context const& ctx, point p, bool leaf, bool control) override;
      bool                 click(context const& ctx, mouse_button btn) override;
      bool                 cursor(context const& ctx, point p, cursor_tracking status) override;
      void                 begin_tracking(context const& ctx, tracker_info& track_info) override;
      void                 keep_tracking(context const& ctx, tracker_info& track_info) override;
   };

   template <concepts::Element Subject>
   inline proxy<remove_cvref_t<Subject>, hdivider_element>
   hdivider(Subject&& subject);

   template <std::size_t N, concepts::Element... E>
   inline auto adjustable_hgrid(float(&coords)[N], E&&... elements);

   template <std::size_t N, concepts::Element... E>
   inline auto adjustable_hgrid(std::array<float, N>& coords, E&&... elements);

   template <std::size_t N>
   inline std::array<float, N> make_equal_grid();

   //--------------------------------------------------------------------------
   // Inlines
   //--------------------------------------------------------------------------

   /**
    * \brief
    *    Calculates the normalized position of the i-th grid span.
    *    The position is normalized between 0 and 1.
    *
    * \param i
    *    The index of the grid span.
    *
    * \return
    *    The normalized position of the grid span as a float.
    */
   template <typename Base>
   inline float equal_grid<Base>::get_grid_coord(std::size_t i) const
   {
      return float(i+1) / this->num_spans();
   }

   /**
    * \brief
    *    A no-op function for setting grid coordinates in an equal_grid.
    *
    * \param i
    *    Unused.
    *
    * \param coord
    *    Unused.
    */
   template <typename Base>
   inline void equal_grid<Base>::set_grid_coord(std::size_t /*i*/, float /*coord*/) const
   {}

   /**
    * \brief
    *    Indicates that the grid layout is fixed.
    *
    *    In an equal_grid, the layout is predetermined and not subject to
    *    change, hence this function always returns true.
    *
    * \return
    *    Always true, indicating the grid layout is fixed.
    */
   template <typename Base>
   inline bool equal_grid<Base>::is_fixed() const
   {
      return true;
   };

   /**
    * \brief
    *    Constructs a range_grid object with specified coordinates and size.
    *
    * \param coords
    *    A pointer to an array of coordinate values for the grid.
    *
    * \param size
    *    The number of elements in the coordinates array, defining the grid's
    *    size.
    */
   template <typename Base, bool fixed>
   inline range_grid<Base, fixed>::range_grid(coords_type coords, std::size_t size)
    : _coords{coords}
    , _size{size}
   {
   }

   /**
    * \brief
    *    Retrieves the coordinate of a grid element at a specified index.
    *
    *    This function returns the coordinate value of the grid element at
    *    the given index. If the index is out of bounds, it defaults to
    *    returning 1.0f.
    *
    * \param i
    *    The index of the grid element.
    *
    * \return
    *    The coordinate value at the specified index or 1.0f if out of bounds.
    */
   template <typename Base, bool fixed>
   inline float range_grid<Base, fixed>::get_grid_coord(std::size_t i) const
   {
      return (i < _size) ? _coords[i] : 1.0f;
   }

   /**
    * \brief
    *    Sets the coordinate of a grid element at a specified index.
    *
    *    For grids where coordinates are not fixed, this function updates the
    *    coordinate value of the grid element at the given index. If the grid
    *    is fixed or the index is out of bounds, the function does nothing.
    *
    * \param i
    *    The index of the grid element.
    *
    * \param
    *    coord The new coordinate value to set.
    */
   template <typename Base, bool fixed>
   inline void range_grid<Base, fixed>::set_grid_coord(std::size_t i, float coord) const
   {
      if constexpr (!fixed)
      {
         if (i < _size)
            _coords[i] = coord;
      }
   }

   /**
    * \brief
    *    Checks if the grid's layout is fixed.
    *
    *    This function returns a boolean indicating whether the grid's layout
    *    and coordinates are fixed (true) or can be modified (false).
    *
    * \return
    *    True if the grid's layout is fixed, false otherwise.
    */
   template <typename Base, bool fixed>
   inline bool range_grid<Base, fixed>::is_fixed() const
   {
      return fixed;
   }

   /**
    * \brief
    *    Constructs a container_grid object with specified coordinates.
    *
    * \param coords
    *    A container of coordinates used to initialize the grid.
    */
   template <concepts::GridContainer Container, concepts::Composite Base, bool fixed>
   inline container_grid<Container, Base, fixed>::container_grid(coords_type coords)
      : _coords(coords)
   {}

   /**
    * \brief
    *    Retrieves the coordinate of a grid element at a specified index.
    *
    *    This function returns the coordinate value of the grid element at
    *    the given index. If the index is out of bounds, it defaults to
    *    returning 1.0f, indicating the end of the grid.
    *
    * \param i
    *    The index of the grid element.
    *
    * \return
    *    The coordinate value at the specified index or 1.0f if out of bounds.
    */
   template <concepts::GridContainer Container, concepts::Composite Base, bool fixed>
   inline float container_grid<Container, Base, fixed>::get_grid_coord(std::size_t i) const
   {
      return (i < _coords.size())? _coords[i] : 1.0f;
   }

   /**
    * \brief
    *    Sets the coordinate of a grid element at a specified index.
    *
    *    For grids where coordinates are not fixed, this function updates the
    *    coordinate value of the grid element at the given index. If the grid
    *    is fixed or the index is out of bounds, the function does nothing.
    *
    * \param i
    *    The index of the grid element.
    *
    * \param coord
    *    The new coordinate value to set.
    */
   template <concepts::GridContainer Container, concepts::Composite Base, bool fixed>
   inline void container_grid<Container, Base, fixed>::set_grid_coord(std::size_t i, float coord) const
   {
      if constexpr(!fixed)
      {
         if (i < _coords.size())
            _coords[i] = coord;
      }
   }

   /**
    * \brief
    *    Checks if the grid's layout is fixed.
    *
    *    This function returns a boolean indicating whether the grid's layout
    *    and coordinates are fixed (true) or can be modified (false).
    *
    * \return
    *    True if the grid's layout is fixed, false otherwise.
    */
   template <concepts::GridContainer Container, concepts::Composite Base, bool fixed>
   inline bool container_grid<Container, Base, fixed>::is_fixed() const
   {
      return fixed;
   };

   /**
    * \brief
    *    Create vertical grid layout, given row coordinates as a C-style
    *    array.
    *
    * \tparam N
    *    The number of rows.
    *
    * \tparam E
    *    A parameter pack of elements, constrained by the `Element` concept,
    *    to be placed in the grid cells.
    *
    * \param coords
    *    C-style array with coordinates for grid cells.
    *
    * \param elements
    *    Elements intended to place in grid cells. The elements must satisfy
    *    the `Element` concept.
    *
    * \returns
    *    A vertical grid element represented as an instance of
    *    `array_composite` that is derived from `range_grid<vgrid_element>`.
    *    The row coordinates are provided by the `coords` parameter.
    */
   template <std::size_t N, concepts::Element... E>
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
    *    A parameter pack of elements, constrained by the `Element` concept,
    *    to be placed in the grid cells.
    *
    * \param coords
    *    `std::array` with coordinates for grid cells.
    *
    * \param elements
    *    Elements intended to place in grid cells. The elements must satisfy
    *    the `Element` concept.
    *
    * \returns
    *    A vertical grid element represented as an instance of
    *    `array_composite` that is derived from `range_grid<vgrid_element>`.
    *    The row coordinates are provided by the `coords` parameter.
    */
   template <std::size_t N, concepts::Element... E>
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
    *    A parameter pack of elements, constrained by the `Element` concept,
    *    to be placed in the grid cells.
    *
    * \param elements
    *    Elements intended to place in grid cells. The elements must satisfy
    *    the `Element` concept.
    *
    * \returns
    *    A vertical grid element represented as an instance of
    *    `array_composite` that is derived from `equal_grid<vgrid_element>.
    *    The rows are equally spaced.
    */
   template <concepts::Element... E>
   inline auto vgrid(E&&... elements)
   {
      using composite = array_composite<sizeof...(elements), equal_grid<vgrid_element>>;
      using container = typename composite::container_type;
      composite r{};
      r = container{{share(std::forward<E>(elements))...}};
      return r;
   }

   /**
    * \brief
    *    Create horizontal grid layout, given column coordinates as a C-style
    *    array.
    *
    * \tparam N
    *    The number of columns.
    *
    * \tparam E
    *    A parameter pack of elements, constrained by the `Element` concept,
    *    to be placed in the grid cells.
    *
    * \param coords
    *    C-style array with coordinates for grid cells.
    *
    * \param elements
    *    Elements intended to place in grid cells. The elements must satisfy
    *    the `Element` concept.
    *
    * \returns
    *    A horizontal grid element represented as an instance of
    *    `array_composite` that is derived from `range_grid<hgrid_element>`.
    *    The column coordinates are provided by the `coords` parameter.
    */
   template <std::size_t N, concepts::Element... E>
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
    *    A parameter pack of elements, constrained by the `Element` concept,
    *    to be placed in the grid cells.
    *
    * \param coords
    *    `std::array` with coordinates for grid cells.
    *
    * \param elements
    *    Elements intended to place in grid cells. The elements must satisfy
    *    the `Element` concept.
    *
    * \returns
    *    A horizontal grid element represented as an instance of
    *    `array_composite` that is derived from `range_grid<hgrid_element>`.
    *    The column coordinates are provided by the `coords` parameter.
    */
   template <std::size_t N, concepts::Element... E>
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
    *    A parameter pack of elements, constrained by the `Element` concept,
    *    to be placed in the grid cells.
    *
    * \param elements
    *    Elements intended to place in grid cells. The elements must satisfy
    *    the `Element` concept.
    *
    * \returns
    *    A horizontal grid element represented as an instance of
    *    `array_composite` that is derived from `equal_grid<hgrid_element>.
    *    The columns are equally spaced.
    */
   template <concepts::Element... E>
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
    *    Creates a horizontal divider element with the given subject.
    *
    * \param
    *    subject The subject element to be wrapped inside the
    *    `hdivider_element`.
    *
    * \return
    *    A proxy object containing the `hdivider_element` and the subject.
    */
   template <concepts::Element Subject>
   inline proxy<remove_cvref_t<Subject>, hdivider_element>
   hdivider(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }

   /**
    * \brief
    *    Creates an adjustable horizontal grid with specified coordinates and
    *    elements.
    *
    *    Constructs an adjustable horizontal grid composed of the specified
    *    elements and dividers between them. The grid's layout can be
    *    adjusted by dragging the dividers. The coordinates array specifies
    *    the initial positions and sizes of the elements.
    *
    * \tparam N
    *    The number of elements in the `coords` array.
    *
    * \tparam E
    *    The types of the elements to be included in the grid.
    *
    * \param coords
    *    An array of float values representing the initial coordinates of the
    *    grid elements.
    *
    * \param elements
    *    A variadic list of elements to be included in the grid.
    *
    * \return
    *    A composite object representing the adjustable horizontal grid.
    */
   template <std::size_t N, concepts::Element... E>
   inline auto adjustable_hgrid(float(&coords)[N], E&&... elements)
   {
      using composite = array_composite<sizeof...(elements), range_grid<hgrid_element, false>>;
      composite r{coords, N};

      auto fill = [&, i = 0](auto&& e) mutable
      {
         if (i != 0)
            r[i++] = share(hdivider(std::forward<decltype(e)>(e)));
         else
            r[i++] = share(std::forward<decltype(e)>(e));
      };

      (fill(elements), ...);
      return r;
   }

   /**
    * \brief
    *    Overload of adjustable_hgrid for std::array coordinates.
    *
    *    This function template is an overload of `adjustable_hgrid` that
    *    accepts a std::array for the coordinates instead of a plain array.
    *    It forwards its arguments to the primary `adjustable_hgrid` function
    *    after converting the std::array to a plain array reference.
    *
    * \tparam N
    *    The size of the std::array representing the number of elements.
    *
    * \tparam E
    *    The types of the elements to be included in the grid.    *
    *
    * \param coords
    *    A std::array of float values representing the initial coordinates of
    *    the grid elements.
    *
    * \param elements
    *    A variadic list of elements to be included in the grid.
    *
    * \return
    *    A composite adjustable horizontal grid.
    */
   template <std::size_t N, concepts::Element... E>
   inline auto adjustable_hgrid(std::array<float, N>& coords, E&&... elements)
   {
      using plain_array = float (&)[N];
      return adjustable_hgrid(plain_array(*coords.data()), std::forward<E>(elements)...);
   }

   /**
    * \brief
    *    Creates a vertical divider element with the given subject.
    *
    * \param
    *    subject The subject element to be wrapped inside the
    *    `vdivider_element`.
    *
    * \return
    *    A proxy object containing the `vdivider_element` and the subject.
    */
   template <concepts::Element Subject>
   inline proxy<remove_cvref_t<Subject>, vdivider_element>
   vdivider(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }

   /**
    * \brief
    *    Creates an adjustable vertical grid with specified coordinates and
    *    elements.
    *
    *    Constructs an adjustable vertical grid composed of the specified
    *    elements and dividers between them. The grid's layout can be
    *    adjusted by dragging the dividers. The `coords` array specifies the
    *    initial positions and sizes of the elements, allowing for a
    *    customizable layout.
    *
    * \tparam N
    *    The number of elements in the `coords` array.
    *
    * \tparam E
    *    The types of the elements to be included in the grid.
    *
    * \param coords
    *    An array of float values representing the initial coordinates of the
    *    grid elements.
    *
    * \param elements
    *    A variadic list of elements to be included in the grid.
    *
    * \return
    *    A composite object representing the adjustable vertical grid.
    */
   template <std::size_t N, concepts::Element... E>
   inline auto adjustable_vgrid(float(&coords)[N], E&&... elements)
   {
      using composite = array_composite<sizeof...(elements), range_grid<vgrid_element, false>>;
      composite r{coords, N};

      auto fill = [&, i = 0](auto&& e) mutable
      {
         if (i != 0)
            r[i++] = share(vdivider(std::forward<decltype(e)>(e)));
         else
            r[i++] = share(std::forward<decltype(e)>(e));
      };

      (fill(elements), ...);
      return r;
   }

   /**
    * \brief Overload of `adjustable_vgrid` accepting an std::array for
    *    coordinates.
    *
    *    This function template is an overload of `adjustable_vgrid` designed
    *    to accept a std::array for the coordinates instead of a plain array.
    *    It forwards its arguments to the primary `adjustable_vgrid` function
    *    after converting the std::array to a plain array reference.
    *
    * \tparam N The size of the std::array representing the number of
    *    elements.
    *
    * \tparam E The types of the elements to be included in the grid.
    *
    * \param coords
    *    A std::array of float values representing the initial coordinates of
    *    the grid elements.
    *
    * \param elements
    *    A variadic list of elements to be included in the grid.
    *
    * \return
    *    A composite adjustable vertical grid.
    */
   template <std::size_t N, concepts::Element... E>
   inline auto adjustable_vgrid(std::array<float, N>& coords, E&&... elements)
   {
      using plain_array = float (&)[N];
      return adjustable_vgrid(plain_array(*coords.data()), std::forward<E>(elements)...);
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
