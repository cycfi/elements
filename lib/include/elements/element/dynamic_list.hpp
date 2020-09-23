/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_DYNAMIC_MARCH_2_2020)
#define ELEMENTS_DYNAMIC_MARCH_2_2020

#include <elements/element/element.hpp>
#include <memory>
#include <vector>
#include <functional>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // The cell composer abstract class
   ////////////////////////////////////////////////////////////////////////////
   class cell_composer : public std::enable_shared_from_this<cell_composer>
   {
   public:

      struct limits
      {
         float min = 0;
         float max = full_extent;
      };

      virtual std::size_t     size() const = 0;
      virtual element_ptr     compose(std::size_t index) = 0;
      virtual limits          width_limits(basic_context const& ctx) const = 0;
      virtual float           line_height(std::size_t index, basic_context const& ctx) const = 0;
   };

   ////////////////////////////////////////////////////////////////////////////
   // This cell composer has fixed-sized width limits and line-height.
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base = cell_composer>
   class static_limits_cell_composer : public Base
   {
   public:

      using base_type = static_limits_cell_composer<Base>;

                              template <typename... Rest>
                              static_limits_cell_composer(
                                 float min_width, float line_height
                               , Rest&& ...rest
                              );

                              template <typename... Rest>
                              static_limits_cell_composer(
                                 float min_width, float max_width, float line_height
                               , Rest&& ...rest
                              );

      cell_composer::limits   width_limits(basic_context const& ctx) const override;
      float                   line_height(std::size_t index, basic_context const& ctx) const override;

   private:

      float                   _line_height;
       cell_composer::limits  _width_limits;
   };

   ////////////////////////////////////////////////////////////////////////////
   // This cell composer derives the width limits and line-height from the
   // first element in the list (index 0). This cell composer has fixed-sized
   // width limits and line-height.
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base = cell_composer>
   class fixed_derived_limits_cell_composer : public Base
   {
   public:

      using base_type = fixed_derived_limits_cell_composer<Base>;

                              template <typename... Rest>
                              fixed_derived_limits_cell_composer(Rest&& ...rest);

      cell_composer::limits   width_limits(basic_context const& ctx) const override;
      float                   line_height(std::size_t index, basic_context const& ctx) const override;

   private:

      void                    get_limits(basic_context const& ctx) const;

      using limits = cell_composer::limits;

      mutable float           _line_height = -1;
      mutable limits          _width_limits = { -1, full_extent };
   };

   ////////////////////////////////////////////////////////////////////////////
   // This cell composer has fixed-length (number of list elements).
   ////////////////////////////////////////////////////////////////////////////
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

   ////////////////////////////////////////////////////////////////////////////
   // basic_cell_composer given the number of elements and a compose function
   ////////////////////////////////////////////////////////////////////////////
   template <typename F>
   inline auto basic_cell_composer(std::size_t size, F&& compose)
   {
      using ftype = remove_cvref_t<F>;
      using return_type =
         fixed_derived_limits_cell_composer<
            fixed_length_cell_composer<
               function_cell_composer<ftype>
            >
         >;
      return share(return_type{ size, std::forward<ftype>(compose) });
   }

   ////////////////////////////////////////////////////////////////////////////
   // basic_cell_composer given the min_width, line_height, number of
   // elements and a compose function.
   ////////////////////////////////////////////////////////////////////////////
   template <typename F>
   inline auto basic_cell_composer(
      float min_width, float line_height, std::size_t size, F&& compose
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
            min_width
          , line_height
          , size
          , std::forward<ftype>(compose)
         }
      );
   }

   ////////////////////////////////////////////////////////////////////////////
   // basic_cell_composer given the min_width, max_width, line_height, number
   // of elements and a compose function.
   ////////////////////////////////////////////////////////////////////////////
   template <typename F>
   inline auto basic_cell_composer(
      float min_width, float max_width, float line_height, std::size_t size, F&& compose
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
            min_width
          , max_width
          , line_height
          , size
          , std::forward<ftype>(compose)
         }
      );
   }

   ////////////////////////////////////////////////////////////////////////////
   // The main dynamic_list class
   ////////////////////////////////////////////////////////////////////////////
   class dynamic_list : public element
   {
   public:

      using composer_ptr = std::shared_ptr<cell_composer>;

                                 dynamic_list(composer_ptr composer)
                                  : _composer(composer)
                                 {}

      view_limits                limits(basic_context const& ctx) const override;
      void                       draw(context const& ctx) override;
      void                       layout(context const& ctx) override;

      void                       update();
      void                       update(basic_context const& ctx) const;

   private:

      struct row_info
      {
         double                  pos;
         double                  height;
         element_ptr             elem_ptr;
         int                     layout_id = -1;
      };

      using rows_vector = std::vector<row_info>;

      composer_ptr               _composer;
      point                      _previous_size;
      std::size_t                _previous_window_start = 0;
      std::size_t                _previous_window_end = 0;

      mutable rows_vector        _rows;
      mutable double             _height = 0;
      mutable int                _layout_id = 0;
      mutable bool               _update_request = true;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Inlines
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base>
   template <typename... Rest>
   inline static_limits_cell_composer<Base>::static_limits_cell_composer(
      float min_width
    , float line_height
    , Rest&& ...rest
   )
    : Base(std::forward<Rest>(rest)...)
    , _line_height{ line_height }
    , _width_limits{ min_width, full_extent }
   {}

   template <typename Base>
   template <typename... Rest>
   inline static_limits_cell_composer<Base>::static_limits_cell_composer(
      float min_width
    , float max_width
    , float line_height
    , Rest&& ...rest
   )
    : Base(std::forward<Rest>(rest)...)
    , _line_height(line_height)
    , _width_limits{ min_width, max_width }
   {}

   template <typename Base>
   inline cell_composer::limits
   static_limits_cell_composer<Base>::width_limits(basic_context const& /*ctx*/) const
   {
      return _width_limits;
   }

   template <typename Base>
   inline float static_limits_cell_composer<Base>::line_height(std::size_t /*index*/, basic_context const& /*ctx*/) const
   {
      return _line_height;
   }

   ////////////////////////////////////////////////////////////////////////////
   template <typename Base>
   template <typename... Rest>
   inline fixed_derived_limits_cell_composer<Base>::fixed_derived_limits_cell_composer(
      Rest&& ...rest
   )
    : Base(std::forward<Rest>(rest)...)
    , _line_height{ -1 }
    , _width_limits{ -1, full_extent }
   {}

   template <typename Base>
   inline cell_composer::limits
   fixed_derived_limits_cell_composer<Base>::width_limits(basic_context const& ctx) const
   {
      if (_width_limits.min == -1)
         get_limits(ctx);
      return _width_limits;
   }

   template <typename Base>
   inline float fixed_derived_limits_cell_composer<Base>::line_height(std::size_t /*index*/, basic_context const& ctx) const
   {
      if (_line_height == -1)
         get_limits(ctx);
      return _line_height;
   }

   template <typename Base>
   void fixed_derived_limits_cell_composer<Base>::get_limits(basic_context const& ctx) const
   {
      auto lim = const_cast<base_type*>(this)->compose(0)->limits(ctx);
      _width_limits.min = lim.min.x;
      _width_limits.max = lim.max.x;
      _line_height = lim.min.y;
   }
}}

#endif
