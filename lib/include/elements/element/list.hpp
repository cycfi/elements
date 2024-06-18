/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_LIST_MARCH_2_2020)
#define ELEMENTS_LIST_MARCH_2_2020

#include <elements/element/composite.hpp>
#include <memory>
#include <vector>
#include <functional>
#include <set>
#include<iostream>

namespace cycfi::elements
{
   /**
    * \class cell_composer
    *
    * \brief
    *    An abstract base for classes that need to handle layout composition
    *    for elements placed in cells inside a list.
    *
    *    The class declares several pure virtual functions for managing and
    *    rendering elements in a dynamic list structure, including
    *    calculating the number of elements, resizing the list, creating or
    *    composing an element at a specific index, and determining the size
    *    and limits of elements along both the main and secondary axes.
    */
   class cell_composer : public std::enable_shared_from_this<cell_composer>
   {
   public:

      struct limits
      {
         float min = 0;
         float max = full_extent;
      };

      virtual std::size_t     size() const = 0;
      virtual void            resize(size_t s) = 0;
      virtual element_ptr     compose(std::size_t index) = 0;
      virtual limits          secondary_axis_limits(basic_context const& ctx) const = 0;
      virtual float           main_axis_size(std::size_t index, basic_context const& ctx) const = 0;
   };

   /**
    * \class static_limits_cell_composer
    *
    * \brief
    *    A cell composer with fixed-size width limits and line-height. The
    *    composed cells have fixed width limits and line-height.
    *
    * \tparam Base
    *    The base cell composer type.
    */
   template <typename Base = cell_composer>
   class static_limits_cell_composer : public Base
   {
   public:

      using base_type = static_limits_cell_composer<Base>;

                              template <typename... Rest>
                              static_limits_cell_composer(
                                 float min_secondary_axis_size, float main_axis_size
                               , Rest&& ...rest
                              );

                              template <typename... Rest>
                              static_limits_cell_composer(
                                 float min_secondary_axis_size
                               , float max_secondary_axis_size
                               , float main_axis_size
                               , Rest&& ...rest
                              );

      cell_composer::limits   secondary_axis_limits(basic_context const& ctx) const override;
      float                   main_axis_size(std::size_t index, basic_context const& ctx) const override;

   private:

      float                   _main_axis_size;
      cell_composer::limits   _secondary_axis_limits;
   };

   ////////////////////////////////////////////////////////////////////////////
   // This cell composer derives the width limits and line-height from the
   // first element in the list (index 0). This cell composer has fixed-sized
   // width limits and line-height.
   ////////////////////////////////////////////////////////////////////////////

   /**
    * \class fixed_derived_limits_cell_composer
    *
    * \brief
    *    A cell composer with fixed-size width limits and line-height derived
    *    from the first cell.
    *
    *    Inherits from a base cell composer and overrides some of its virtual
    *    functions. The width limits and the line-height are derived from the
    *    size of the first cell in the list (index 0) and subsequent cells
    *    also have these fixed dimensions.
    *
    *    Deriving information from the first cell implies that there should
    *    at least be one cell. To overcome this limitation, the composer can
    *    check if the list is empty and generate an prototypical cell in such
    *    cases.
    *
    * \tparam Base
    *    The base cell composer type.
    */
   template <typename Base = cell_composer>
   class fixed_derived_limits_cell_composer : public Base
   {
   public:

      using base_type = fixed_derived_limits_cell_composer<Base>;

                                 template <typename... Rest>
                                 fixed_derived_limits_cell_composer(Rest&& ...rest);

      cell_composer::limits      secondary_axis_limits(basic_context const& ctx) const override;
      float                      main_axis_size(std::size_t index, basic_context const& ctx) const override;

   protected:

      virtual void               get_limits(basic_context const& ctx) const;

      using limits = cell_composer::limits;

      mutable float              _main_axis_size = -1;
      mutable limits             _secondary_axis_limits = {-1, full_extent};
   };

   template<typename Base = cell_composer>
   using vertical_fixed_derived_limits_cell_composer
   [[deprecated("Use vfixed_derived_limits_cell_composer instead.")]]
      = fixed_derived_limits_cell_composer<Base>;

   /**
    * \brief
    *    Vertical fixed derived limits cell composer.
    *
    * \tparam Base
    *    The base cell composer type.
    */
   template<typename Base = cell_composer>
   using vfixed_derived_limits_cell_composer = fixed_derived_limits_cell_composer<Base>;

   /**
    * \class hfixed_derived_limits_cell_composer
    *
    * \brief
    *    Horizontal fixed derived limits cell composer with specifics for
    *    handling columns.
    *
    * \tparam Base
    *    The base cell composer type.
    *
    */
   template<typename Base = cell_composer>
   class hfixed_derived_limits_cell_composer: public fixed_derived_limits_cell_composer<Base>
   {
   public:

      using base_type = hfixed_derived_limits_cell_composer<Base>;

                                template <typename... Rest>
                                hfixed_derived_limits_cell_composer(Rest&& ...rest);
   protected:

      virtual void              get_limits(basic_context const& ctx) const;
   };

   template<typename Base = cell_composer>
   using horizontal_fixed_derived_limits_cell_composer
   [[deprecated("Use hfixed_derived_limits_cell_composer instead.")]]
      = hfixed_derived_limits_cell_composer<Base>;

   /**
    * \class fixed_length_cell_composer
    *
    * \brief
    *    A cell composer that has a fixed length (number of list elements).

    * \tparam Base
    *    The base cell composer type.
    */
   template <typename Base = cell_composer>
   class fixed_length_cell_composer : public Base
   {
   public:

      using base_type = fixed_length_cell_composer<Base>;

                              template <typename... Rest>
                              fixed_length_cell_composer(std::size_t size, Rest&& ...rest)
                               : Base(std::forward<Rest>(rest)...)
                               , _size(size)
                              {}

      std::size_t             size() const override { return _size; }
      void                    resize(size_t s) override { _size = s; }

   private:

      std::size_t             _size;
   };

   ////////////////////////////////////////////////////////////////////////////
   // This cell composer composes the cell element using a provided function.
   ////////////////////////////////////////////////////////////////////////////
   template <typename F, typename Base = cell_composer>
   class function_cell_composer : public Base
   {
   public:

      using base_type = function_cell_composer<Base>;

                              template <typename... Rest>
                              function_cell_composer(F&& compose_, Rest&& ...rest)
                               : Base(std::forward<Rest>(rest)...)
                               , _compose(compose_)
                              {}

      virtual element_ptr     compose(std::size_t index) { return _compose(index); }

   private:

      F                       _compose;
   };

   /**
    * \class function_cell_composer
    *
    * \brief
    *    Create a fixed length vertical basic cell composer given the number
    *    of elements and a compose function.
    *
    * \tparam F
    *    The function type used to compose the cell elements.
    *
    * \tparam Base
    *    The base cell composer type.
    *
    * @return
    *    Shared pointer to a cell composer.
    */
   template <typename F>
   inline auto basic_cell_composer(std::size_t size, F&& compose)
   {
      using ftype = remove_cvref_t<F>;
      using return_type =
         vfixed_derived_limits_cell_composer<
            fixed_length_cell_composer<
               function_cell_composer<ftype>
            >
         >;
      return share(return_type{size, std::forward<ftype>(compose)});
   }

   template<typename F>
   [[deprecated("Use basic_vcell_composer instead.")]]
   inline auto basic_vertical_cell_composer(std::size_t size, F&& compose)
   {
      return basic_cell_composer(size, compose);
   }

   /**
    * \brief
    *    Create a fixed length vertical basic cell composer given the number
    *    of elements and a compose function.
    *
    * \tparam F
    *    The function type used for composing cells.
    *
    * \param size
    *    The number of cells.
    *
    * \param compose
    *    Function used to compose the cells.
    *
    * @return
    *     Shared pointer to a cell composer.
    */
   template <typename F>
   inline auto basic_vcell_composer(std::size_t size, F&& compose)
   {
      return basic_cell_composer(size, compose);
   }

   template<typename F>
   [[deprecated("Use basic_hcell_composer instead.")]]
   inline auto basic_horizontal_cell_composer(std::size_t size, F&& compose)
   {
      using ftype = remove_cvref_t<F>;
      using return_type =
         hfixed_derived_limits_cell_composer<
            fixed_length_cell_composer<
               function_cell_composer<ftype>
            >
         >;
      return share(return_type{size, std::forward<ftype>(compose)});
   }

   /**
    * \brief
    *    Create a fixed length horizontal basic cell composer given the
    *    number of elements and a compose function.
    *
    * \tparam F
    *    The function type used for composing cells.
    *
    * \param size
    *    The number of cells.
    *
    * \param compose
    *    Function used to compose the cells.
    *
    * @return
    *     Shared pointer to a cell composer.
    */
   template<typename F>
   inline auto basic_hcell_composer(std::size_t size, F&& compose)
   {
      using ftype = remove_cvref_t<F>;
      using return_type =
         hfixed_derived_limits_cell_composer<
            fixed_length_cell_composer<
               function_cell_composer<ftype>
            >
         >;
      return share(return_type{size, std::forward<ftype>(compose)});
   }

   /**
    * \brief
    *    Create a fixed length horizontal basic cell composer, with specified
    *    minimum secondary axis, main axis size, number of elements and a
    *    compose function.
    *
    * \tparam F
    *    The function type used for composing cells.
    *
    * \param size
    *    The number of cells.
    *
    * \param compose
    *    Function used to compose the cells.
    *
    * @return
    *     Shared pointer to a cell composer.
    */
   template <typename F>
   inline auto basic_cell_composer(
      float min_secondary_axis_size
    , float main_axis_size
    , std::size_t size
    , F&& compose
   )
   {
      using ftype = remove_cvref_t<F>;
      using return_type =
         static_limits_cell_composer<
            fixed_length_cell_composer<
               function_cell_composer<ftype>
            >
         >;
      return share(
         return_type{
            min_secondary_axis_size
          , main_axis_size
          , size
          , std::forward<ftype>(compose)
         }
      );
   }

   /**
    * \brief
    *    Create a fixed length horizontal basic cell composer, with specified
    *    minimum and maximum secondary axis, main axis size, number of
    *    elements and a compose function.
    *
    * \tparam F
    *    The function type used for composing cells.
    *
    * \param size
    *    The number of cells.
    *
    * \param compose
    *    Function used to compose the cells.
    *
    * @return
    *     Shared pointer to a cell composer.
    */
   template <typename F>
   inline auto basic_cell_composer(
      float min_secondary_axis_size
    , float max_secondary_axis_size
    , float main_axis_size
    , std::size_t size
    , F&& compose
   )
   {
      using ftype = remove_cvref_t<F>;
      using return_type =
         static_limits_cell_composer<
            fixed_length_cell_composer<
               function_cell_composer<ftype>
            >
         >;
      return share(
         return_type{
            min_secondary_axis_size
          , max_secondary_axis_size
          , main_axis_size
          , size
          , std::forward<ftype>(compose)
         }
      );
   }

   /**
    * \class list
    *
    * \brief
    *    Main class for managing a vertically oriented list of elements.
    *
    *    The `list` class manages a collection of cell elements in a vertical
    *    list. This class works with a cell composer to manage layout and
    *    drawing operations. The class provides methods to resize the list,
    *    update list cells, and perform list operations like insertion,
    *    deletion, and movement of items.
    *
    *    The list class provides options for managing the list either
    *    externally or internally. When managing the list externally, only
    *    visible elements are guaranteed to be held in memory. A garbage
    *    collection scheme is implemented to clean up hidden elements,
    *    recreating them only as needed.
    */
   class list : public composite_base
   {
   public:

      using composer_ptr = std::shared_ptr<cell_composer>;
      using indices_type = std::vector<std::size_t>;

                                 list(composer_ptr composer, bool manage_externally = true);
                                 list(list const& rhs);
                                 list(list&& rhs);
      list&                      operator=(list const& rhs);
      list&                      operator=(list&& rhs);

      view_limits                limits(basic_context const& ctx) const override;
      void                       draw(context const& ctx) override;
      void                       layout(context const& ctx) override;

      void                       update();
      void                       clear();
      void                       resize(size_t n);
      bool                       manage_externally() const { return _manage_externally; }
      void                       move(std::size_t pos, indices_type const& indices);
      void                       insert(std::size_t pos, std::size_t num_items);
      void                       erase(indices_type const& indices);

      rect                       bounds_of(context const& ctx, std::size_t ix) const override;

      std::size_t                size() const override;
      element&                   at(std::size_t ix) const override;

      void                       for_each_visible(
                                    context const& ctx
                                  , for_each_callback f
                                  , bool reverse = false
                                 ) const override;

   protected:

      struct cell_info
      {
         double                  pos;
         double                  main_axis_size;
         element_ptr             elem_ptr;
         int                     layout_id = -1;
      };

      // virtual member functions to specialize in hdynamic or vdynamic
      virtual view_limits        make_limits(float main_axis_size, cell_composer::limits secondary_axis_limits ) const;
      virtual float              get_main_axis_start(const rect &r) const;
      virtual float              get_main_axis_end(const rect &r) const;
      virtual void               set_bounds(rect& r, float main_axis_start, cell_info &info) const;
      void                       set_bounds(context& ctx, float main_axis_start, cell_info &info) const;

      using cells_vector = std::vector<cell_info>;
      mutable cells_vector       _cells;

   private:

      void                       sync(basic_context const& ctx) const;
      void                       update(basic_context const& ctx) const;
      void                       move(basic_context const& ctx) const;
      void                       insert(basic_context const& ctx) const;
      void                       erase(basic_context const& ctx) const;

      composer_ptr               _composer;
      bool                       _manage_externally;
      point                      _previous_size;
      std::size_t                _previous_window_start = 0;
      std::size_t                _previous_window_end = 0;

      mutable double             _main_axis_full_size = 0;
      mutable int                _layout_id = 0;

      mutable bool               _update_request:1;
      mutable bool               _move_request:1;
      mutable bool               _insert_request:1;
      mutable bool               _erase_request:1;
      mutable bool               _relinquish_focus_request:1;

      struct request_info
      {
         std::size_t                _move_pos;
         std::vector<std::size_t>   _move_indices;
         std::size_t                _insert_pos;
         std::size_t                _insert_num_items;
         std::vector<std::size_t>   _delete_indices;
      };

      using request_info_ptr = std::unique_ptr<request_info>;
      mutable request_info_ptr      _request_info;
   };

   // The old name is deprecated
   using dynamic_list [[deprecated("Use list instead.")]] = list;

   ////////////////////////////////////////////////////////////////////////////
   // The vertical list class - just an alias
   ////////////////////////////////////////////////////////////////////////////
   using vdynamic_list [[deprecated("Use vlist instead.")]] = list;
   using vlist = list;

   ////////////////////////////////////////////////////////////////////////////
   // The horizontal list class
   ////////////////////////////////////////////////////////////////////////////
   /**
    * \class hlist
    *
    * \brief
    *    Class for managing a horizontally oriented list of cells.
    *
    *    The class inherits from `list` and overrides functions to enable
    *    horizontal cell composition.
    */
   class hlist : public list
   {
   public:
                                 hlist(composer_ptr ptr, bool manage_externally = true)
                                  : list(ptr, manage_externally)
                                 {}
      rect                       bounds_of(context const& ctx, std::size_t ix) const override;

   protected:

      view_limits                make_limits(float main_axis_size, cell_composer::limits secondary_axis_limits) const override;
      void                       set_bounds(rect& r, float main_axis_start, cell_info &info) const override;
      float                      get_main_axis_start(const rect&r) const override;
      float                      get_main_axis_end(const rect &r) const override;
   };

   // The old name is deprecated
   using hdynamic_list [[deprecated("Use hlist instead.")]] = hlist;

   /**
    * \brief
    *    Utility function to move items in a vector `v` from given `indices`
    *    to a new position `pos`.
    *
    *    This utility takes a vector and moves the elements from the provided
    *    indices to a specified position. The moved elements retain their
    *    relative order. If the new position is beyond the range of the
    *    vector, the elements will be moved to the end of the vector. The
    *    indices should be valid indices in vector `v`.
    *
    * \tparam T
    *    The type of elements contained in the vector.
    *
    * \param v
    *    Vector in which elements will be moved.
    *
    * \param pos
    *    Target position in the vector to which the elements will be moved.
    *
    * \param indices
    *    Vector of indexed positions of the elements in vector `v` to be
    *    moved.
    */
   template <typename T>
   inline void move_indices(std::vector<T>& v, std::size_t pos, std::vector<std::size_t> const& indices)
   {
      // Precondition: The indices should be validly pointing to items in vector `v`.

      // Reserve space for moved elements
      std::vector<T> to_move;
      to_move.reserve(indices.size());

      // Create a copy of the elements to be moved and erase the elements
      // from their original positions. Iterating in reverse, after this
      // operation, `to_move` will contain the items in reverse order.
      for (auto i = indices.crbegin(); i != indices.crend(); ++i)
      {
         to_move.push_back(std::move(v[*i]));
         v.erase(v.begin()+*i);
         if (pos > *i)
            --pos;
      }

      // Determine the insert position
      auto pos_i = v.begin() + std::min(pos, v.size());

      // Insert the elements at the new position in reverse order
      v.insert(pos_i, to_move.crbegin(), to_move.crend());
   }

   // Utility to erase items in a vector `v` with given `indices`.

   /**
    * \brief
    *    Utility function to erase items from a vector `v` at the given `indices`.
    *
    *    This utility works by iterating over the indices vector in reverse
    *    order and erasing each corresponding element from the vector `v`.
    *    The indices vector should point to valid items in vector `v`.
    *
    * \tparam T
    *    The type of elements contained in the vector.
    *
    * \param v
    *    Vector in which elements will be erased.
    *
    * \param indices
    *    Vector of indexed positions of the elements in vector `v` to be
    *    erased.
    */
   template <typename T>
   inline void erase_indices(std::vector<T>& v, std::vector<std::size_t> const& indices)
   {
      // Precondition: The indices should be validly pointing to items in vector `v`.
      for (auto i = indices.crbegin(); i != indices.crend(); ++i)
         v.erase(v.begin()+*i);
   }

   ////////////////////////////////////////////////////////////////////////////
   // Inlines
   ////////////////////////////////////////////////////////////////////////////
   namespace inlines {}

   /**
    * \brief
    *    Constructor.
    *
    * \tparam Base
    *    The base class.
    *
    * \tparam Rest
    *    A variadic template parameter for handling an arbitrary number of
    *    additional arguments.
    *
    * \param min_secondary_axis_size
    *    Minimum size along the secondary axis of the cell.
    *
    * \param main_axis_size
    *    Size along the main axis of the cell.
    *
    * \param rest
    *    Parameter pack containing any additional arguments to be forwarded
    *    to the Base class's constructor.
    */
   template <typename Base>
   template <typename... Rest>
   inline static_limits_cell_composer<Base>::static_limits_cell_composer(
      float min_secondary_axis_size
    , float main_axis_size
    , Rest&& ...rest
   )
    : Base(std::forward<Rest>(rest)...)
    , _main_axis_size{main_axis_size}
    , _secondary_axis_limits{min_secondary_axis_size, full_extent}
   {}

   /**
    * \brief
    *    Constructor.
    *
    * \tparam Base
    *    The base class.
    *
    * \tparam Rest
    *    A variadic template parameter for handling an arbitrary number of
    *    additional arguments.
    *
    * \param min_secondary_axis_size
    *    Minimum size along the secondary axis of the cell.
    *
    * \param max_secondary_axis_size
    *    Maximum size along the secondary axis of the cell.
    *
    * \param main_axis_size
    *    Size along the main axis of the cell.
    *
    * \param rest
    *    Parameter pack containing any additional arguments to be forwarded
    *    to the Base class's constructor.
    */
   template <typename Base>
   template <typename... Rest>
   inline static_limits_cell_composer<Base>::static_limits_cell_composer(
      float min_secondary_axis_size
    , float max_secondary_axis_size
    , float main_axis_size
    , Rest&& ...rest
   )
    : Base(std::forward<Rest>(rest)...)
    , _main_axis_size(main_axis_size)
    , _secondary_axis_limits{min_secondary_axis_size, max_secondary_axis_size}
   {}

   template <typename Base>
   inline cell_composer::limits
   static_limits_cell_composer<Base>::secondary_axis_limits(basic_context const& /*ctx*/) const
   {
      return _secondary_axis_limits;
   }

   template <typename Base>
   inline float static_limits_cell_composer<Base>::main_axis_size(std::size_t /*index*/, basic_context const& /*ctx*/) const
   {
      return _main_axis_size;
   }

   template <typename Base>
   template <typename... Rest>
   inline fixed_derived_limits_cell_composer<Base>::fixed_derived_limits_cell_composer(
      Rest&& ...rest
   )
    : Base(std::forward<Rest>(rest)...)
    , _main_axis_size{-1}
    , _secondary_axis_limits{-1, full_extent}
   {}

   /**
    * \brief
    *    Get the secondary axis limits of a static_limits_cell_composer object.
    *
    * \tparam Base
    *    The base class.
    *
    * \param ctx
    *    The basic_context object.
    *
    * @return
    *    The cell's size limits along the secondary axis.
    */
   template <typename Base>
   inline cell_composer::limits
   fixed_derived_limits_cell_composer<Base>::secondary_axis_limits(basic_context const& ctx) const
   {
      if (_secondary_axis_limits.min == -1)
         get_limits(ctx);
      return _secondary_axis_limits;
   }

   /**
    * \brief
    *    Get the main axis size of a static_limits_cell_composer object.
    *
    * \tparam Base
    *    The base class.
    *
    * \param index
    *    The cell index.
    *
    * \param ctx
    *    A reference to the basic_context of the element, which provides
    *    access to the current view and canvas. The context can be used in
    *    calculating size limits of the element.
    *
    * @return
    *    The cell's size along the main axis.
    */
   template <typename Base>
   inline float fixed_derived_limits_cell_composer<Base>::main_axis_size(std::size_t /*index*/, basic_context const& ctx) const
   {
      if (_main_axis_size == -1)
         get_limits(ctx);
      return _main_axis_size;
   }

   // Private member function
   template <typename Base>
   void fixed_derived_limits_cell_composer<Base>::get_limits(basic_context const& ctx) const
   {
      auto lim = const_cast<base_type*>(this)->compose(0)->limits(ctx);
      _secondary_axis_limits.min = lim.min.x;
      _secondary_axis_limits.max = lim.max.x;
      _main_axis_size = lim.min.y;
   }

   /**
    * \brief
    *    Constructor.
    *
    * \tparam Base
    *    The base class.
    *
    * \tparam Rest
    *    A variadic template parameter for handling an arbitrary number of
    *    additional arguments.
    *
    * \param rest
    *    Parameter pack containing any additional arguments to be forwarded
    *    to the Base class's constructor.
    */
   template <typename Base>
   template <typename... Rest>
   inline hfixed_derived_limits_cell_composer<Base>::hfixed_derived_limits_cell_composer(
      Rest&& ...rest
   )
    : fixed_derived_limits_cell_composer<Base> (std::forward<Rest>(rest)...)
   {}

   // Private member function
   template<typename Base>
   void hfixed_derived_limits_cell_composer<Base>::get_limits(basic_context const& ctx)  const
   {
      auto lim = const_cast<base_type*>(this)->compose(0)->limits(ctx);
      this->_secondary_axis_limits.min = lim.min.y;
      this->_secondary_axis_limits.max = lim.max.y;
      this->_main_axis_size = lim.min.x;
   }

   /**
    * \brief
    *    Set the bounds of the list object.
    *
    * \param ctx
    *    A reference to the basic_context of the element, which provides
    *    access to the current view and canvas. The context can be used in
    *    calculating size limits of the element.
    *
    * \param main_axis_pos
    *    The position along the main axis.
    *
    * \param cell
    *    The cell_info object to be used for calculations.
    */
   inline void list::set_bounds(context& ctx, float main_axis_pos, cell_info& cell) const
   {
      set_bounds(ctx.bounds, main_axis_pos, cell);
   }
}

#endif
