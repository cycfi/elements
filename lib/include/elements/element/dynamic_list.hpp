/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_DYNAMIC_MARCH_2_2020)
#define ELEMENTS_DYNAMIC_MARCH_2_2020

#include <elements/element/element.hpp>
#include <memory>
#include <vector>

namespace cycfi { namespace elements
{
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
   template <typename Base = cell_composer>
   class fixed_cell_composer : public Base
   {
   public:

      using base_type = fixed_cell_composer<Base>;

                              template <typename... Rest>
                              fixed_cell_composer(
                                 float line_height, float min_width
                               , Rest&& ...rest
                              );

                              template <typename... Rest>
                              fixed_cell_composer(
                                 float line_height, float min_width
                               , float max_width
                               , Rest&& ...rest
                              );

      cell_composer::limits   width_limits(basic_context const& ctx) const override;
      float                   line_height(std::size_t index, basic_context const& ctx) const override;

   private:

      float                   _line_height;
       cell_composer::limits  _width_limits;
   };

   ////////////////////////////////////////////////////////////////////////////
   template <typename Base = cell_composer>
   class fixed_size_composer : public Base
   {
   public:

      using base_type = fixed_size_composer<Base>;

                              template <typename... Rest>
                              fixed_size_composer(std::size_t size, Rest&& ...rest)
                               : Base(std::forward<Rest>(rest)...)
                               , _size(size)
                              {}

      std::size_t             size() const override { return _size; }

   private:

      std::size_t             _size;
   };

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
   template <typename Base>
   template <typename... Rest>
   inline fixed_cell_composer<Base>::fixed_cell_composer(
      float line_height
      , float min_width
      , Rest&& ...rest
   )
      : Base(std::forward<Rest>(rest)...)
      , _line_height{ line_height }
      , _width_limits{ min_width, full_extent }
   {}

   template <typename Base>
   template <typename... Rest>
   inline fixed_cell_composer<Base>::fixed_cell_composer(
      float line_height
      , float min_width
      , float max_width
      , Rest&& ...rest
   )
      : Base(std::forward<Rest>(rest)...)
      , _line_height(line_height)
      , _width_limits{ min_width, max_width }
   {}

   template <typename Base>
   inline cell_composer::limits
   fixed_cell_composer<Base>::width_limits(basic_context const& /*ctx*/) const
   {
      return _width_limits;
   }

   template <typename Base>
   inline float fixed_cell_composer<Base>::line_height(std::size_t /*index*/, basic_context const& /*ctx*/) const
   {
      return _line_height;
   }
}}

#endif
