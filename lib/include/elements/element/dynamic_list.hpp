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
#include<set>
#include<iostream>

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
      virtual void			  resize(size_t s) = 0;
      virtual element_ptr     compose(std::size_t index) = 0;
      virtual limits		  secondary_axis_limits(basic_context const& ctx) const = 0;
      virtual float			  main_axis_size(std::size_t index, basic_context const& ctx) const = 0;
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
                                 float min_secondary_axis_size, float main_axis_size
                               , Rest&& ...rest
                              );

                              template <typename... Rest>
                              static_limits_cell_composer(
                                 float min_secondary_axis_size, float max_secondary_axis_size, float main_axis_size
                               , Rest&& ...rest
                              );
      cell_composer::limits		secondary_axis_limits(basic_context const& ctx) const override;
      float 					main_axis_size(std::size_t index, basic_context const& ctx) const override;

   private:

      float 				  _main_axis_size;
      cell_composer::limits	  _secondary_axis_limits;
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

      cell_composer::limits		secondary_axis_limits(basic_context const& ctx) const override;
      float 					main_axis_size(std::size_t index, basic_context const& ctx) const override;

   protected:

      virtual void              get_limits(basic_context const& ctx) const;

      using limits = cell_composer::limits;

      mutable float 			_main_axis_size = -1;
      mutable limits			_secondary_axis_limits = {-1, full_extent};
   };

   template<typename Base = cell_composer>
   using vertical_fixed_derived_limits_cell_composer = fixed_derived_limits_cell_composer<Base>;

   template<typename Base = cell_composer>
   class horizontal_fixed_derived_limits_cell_composer: public fixed_derived_limits_cell_composer<Base>
   {
   public:
      using base_type = horizontal_fixed_derived_limits_cell_composer<Base>;
                                template <typename... Rest>
                                horizontal_fixed_derived_limits_cell_composer(Rest&& ...rest);
   protected:
      virtual void              get_limits(basic_context const& ctx) const;
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
      void 					  resize(size_t s) override {_size = s;}
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
         vertical_fixed_derived_limits_cell_composer<
            fixed_length_cell_composer<
               function_cell_composer<ftype>
            >
         >;
      return share(return_type{ size, std::forward<ftype>(compose) });
   }

   template<typename F>
   inline auto basic_vertical_cell_composer(std::size_t size, F&& compose)
   {
       return basic_cell_composer(size, compose);
   }

   template<typename F>
   inline auto basic_horizontal_cell_composer(std::size_t size, F&& compose)
   {
      using ftype = remove_cvref_t<F>;
      using return_type =
         horizontal_fixed_derived_limits_cell_composer<
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
      float min_secondary_axis_size, float main_axis_size, std::size_t size, F&& compose
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

   ////////////////////////////////////////////////////////////////////////////
   // basic_cell_composer given the min_width, max_width, line_height, number
   // of elements and a compose function.
   ////////////////////////////////////////////////////////////////////////////
   template <typename F>
   inline auto basic_cell_composer(
      float min_secondary_axis_size, float max_secondary_axis_size, float main_axis_size, std::size_t size, F&& compose
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

   ////////////////////////////////////////////////////////////////////////////
   // The main dynamic_list class -> vertical by default
   ////////////////////////////////////////////////////////////////////////////
   class dynamic_list : public element
   {
   public:

      using composer_ptr = std::shared_ptr<cell_composer>;

                                 dynamic_list(composer_ptr composer)
                                  : _composer(composer)
                                 {}

      virtual view_limits        limits(basic_context const& ctx) const override;
      void                       draw(context const& ctx) override;
      void                       layout(context const& ctx) override;

      void                       update();
      void                       update(basic_context const& ctx) const;

      virtual bool 				 click(const context &ctx, mouse_button btn) override;
      virtual bool 				 text(context const& ctx, text_info info) override;
      virtual bool 				 key(const context &ctx, key_info k) override;
      virtual bool 				 cursor(context const& ctx, point p, cursor_tracking status) override;
      virtual bool 				 scroll(context const& ctx, point dir, point p) override;
      virtual void 		    	 drag(context const& ctx, mouse_button btn) override;

      void 			   			 new_focus(context const& ctx, int index);
      bool 				   		 wants_control() const override;
      bool                       wants_focus() const override;
      void                    	 begin_focus() override;
      void                    	 end_focus() override;
      element const*          	 focus() const override;
      element*                	 focus() override;
      void                    	 focus(std::size_t index);
      virtual void            	 reset();
      void 						 resize(size_t n);

       struct hit_info
       {
          element_ptr            element;
          rect                   bounds   = rect{};
          int                    index    = -1;
       };

      virtual rect 				 bounds_of(context const& ctx, int ix) const;
      virtual bool 			 	 reverse_index() const {return false;}
      virtual hit_info 			 hit_element(context const& ctx, point p, bool control) const;

   protected:
      struct cell_info
      {
         double                  pos;
         double                  main_axis_size;
         element_ptr             elem_ptr;
         int                     layout_id = -1;
      };

      // virtual methods to specialize in hdynamic or vdynamic
      virtual view_limits 		 make_limits(float main_axis_size, cell_composer::limits secondary_axis_limits ) const;
      virtual float 	  		 get_main_axis_start(const rect &r);
      virtual float 	  	     get_main_axis_end(const rect &r);
      virtual void 	  			 make_bounds(context& ctx, float main_axis_start, cell_info &info);

      using cells_vector = std::vector<cell_info>;
      mutable cells_vector        _cells;

   private:

      composer_ptr               _composer;
      point                      _previous_size;
      std::size_t                _previous_window_start = 0;
      std::size_t                _previous_window_end = 0;

      mutable double 			 _main_axis_full_size = 0;
      mutable int                _layout_id = 0;
      mutable bool               _update_request = true;

      int 					   	 _focus = -1;
      int 					     _saved_focus = -1;
      int                        _click_tracking = -1;
      int                        _cursor_tracking = -1;
      std::set<int>           	 _cursor_hovering;
   };

   ////////////////////////////////////////////////////////////////////////////
   // The vertical dynamic_list class - just an alias
   ////////////////////////////////////////////////////////////////////////////
   using vdynamic_list = dynamic_list;

   ////////////////////////////////////////////////////////////////////////////
   // The horizontal dynamic_list class
   ////////////////////////////////////////////////////////////////////////////
   class hdynamic_list : public dynamic_list
   {
   public:
                                 hdynamic_list(composer_ptr ptr) : dynamic_list(ptr) {}
      rect 						 bounds_of(context const& ctx, int ix) const override;

   protected:
      view_limits 				 make_limits(float main_axis_size, cell_composer::limits secondary_axis_limits) const override;
      void 						 make_bounds(context &ctx, float main_axis_start, cell_info &info) override;
      float 					 get_main_axis_start(const rect&r) override;
      float 					 get_main_axis_end(const rect &r) override;

   };

   ////////////////////////////////////////////////////////////////////////////
   // Inlines
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base>
   template <typename... Rest>
   inline static_limits_cell_composer<Base>::static_limits_cell_composer(
      float min_secondary_axis_size
    , float main_axis_size
    , Rest&& ...rest
   )
    : Base(std::forward<Rest>(rest)...)
    , _main_axis_size{ main_axis_size }
    , _secondary_axis_limits{ min_secondary_axis_size, full_extent }
   {}

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
    , _secondary_axis_limits{ min_secondary_axis_size, max_secondary_axis_size }
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

   ////////////////////////////////////////////////////////////////////////////
   template <typename Base>
   template <typename... Rest>
   inline fixed_derived_limits_cell_composer<Base>::fixed_derived_limits_cell_composer(
      Rest&& ...rest
   )
    : Base(std::forward<Rest>(rest)...)
    , _main_axis_size{ -1 }
    , _secondary_axis_limits{ -1, full_extent }
   {}

   template <typename Base>
   inline cell_composer::limits
   fixed_derived_limits_cell_composer<Base>::secondary_axis_limits(basic_context const& ctx) const
   {
      if (_secondary_axis_limits.min == -1)
         get_limits(ctx);
      return _secondary_axis_limits;
   }

   template <typename Base>
   inline float fixed_derived_limits_cell_composer<Base>::main_axis_size(std::size_t /*index*/, basic_context const& ctx) const
   {
      if (_main_axis_size == -1)
         get_limits(ctx);
      return _main_axis_size;
   }

   template <typename Base>
   void fixed_derived_limits_cell_composer<Base>::get_limits(basic_context const& ctx) const
   {
      auto lim = const_cast<base_type*>(this)->compose(0)->limits(ctx);
      _secondary_axis_limits.min = lim.min.x;
      _secondary_axis_limits.max = lim.max.x;
      _main_axis_size = lim.min.y;
   }

   template <typename Base>
   template <typename... Rest>
   inline horizontal_fixed_derived_limits_cell_composer<Base>::horizontal_fixed_derived_limits_cell_composer(
      Rest&& ...rest
   )
    : fixed_derived_limits_cell_composer<Base> (std::forward<Rest>(rest)...)
   {}

   template<typename Base>
   void horizontal_fixed_derived_limits_cell_composer<Base>::get_limits(basic_context const& ctx)  const
   {
      auto lim = const_cast<base_type*>(this)->compose(0)->limits(ctx);
      this->_secondary_axis_limits.min = lim.min.y;
      this->_secondary_axis_limits.max = lim.max.y;
      this->_main_axis_size = lim.min.x;
   }
}}

#endif
