/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_ELEMENTS_GUI_LIB_WIDGET_TILE_APRIL_13_2016)
#define CYCFI_ELEMENTS_GUI_LIB_WIDGET_TILE_APRIL_13_2016

#include <elements/element/composite.hpp>
#include <memory>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Vertical Tiles
   ////////////////////////////////////////////////////////////////////////////
   class vtile_element : public composite_base
   {
   public:

      virtual view_limits     limits(basic_context const& ctx) const;
      virtual void            layout(context const& ctx);
      virtual rect            bounds_of(context const& ctx, std::size_t index) const;

   private:

      std::vector<float>      _tiles;
      float                   _left;
      float                   _right;
   };

   using vtile_composite = vector_composite<vtile_element>;

   template <typename... W>
   inline auto vtile(W&&... elements)
   {
      using composite = array_composite<sizeof...(elements), vtile_element>;
      using container = typename composite::container_type;
      composite r{};
      r = container{{ share(std::forward<W>(elements))... }};
      return r;
   }

   ////////////////////////////////////////////////////////////////////////////
   // Horizontal Tiles
   ////////////////////////////////////////////////////////////////////////////
   class htile_element : public composite_base
   {
   public:

      virtual view_limits     limits(basic_context const& ctx) const;
      virtual void            layout(context const& ctx);
      virtual rect            bounds_of(context const& ctx, std::size_t index) const;

   private:

      std::vector<float>      _tiles;
      float                   _top;
      float                   _bottom;
   };

   using htile_composite = vector_composite<htile_element>;

   template <typename... W>
   inline auto htile(W&&... elements)
   {
      using composite = array_composite<sizeof...(elements), htile_element>;
      using container = typename composite::container_type;
      composite r{};
      r = container{{ share(std::forward<W>(elements))... }};
      return r;
   }
}}

#endif
