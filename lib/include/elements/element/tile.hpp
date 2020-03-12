/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_TILE_APRIL_13_2016)
#define ELEMENTS_TILE_APRIL_13_2016

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

      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;
      void                    layout(context const& ctx) override;
      rect                    bounds_of(context const& ctx, std::size_t index) const override;

   private:

      std::vector<float>      _tiles;
   };

   using vtile_composite = vector_composite<vtile_element>;

   template <typename... E>
   inline auto vtile(E&&... elements)
   {
      using composite = array_composite<sizeof...(elements), vtile_element>;
      using container = typename composite::container_type;
      composite r{};
      r = container{{ share(std::forward<E>(elements))... }};
      return r;
   }

   ////////////////////////////////////////////////////////////////////////////
   // Horizontal Tiles
   ////////////////////////////////////////////////////////////////////////////
   class htile_element : public composite_base
   {
   public:

      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;
      void                    layout(context const& ctx) override;
      rect                    bounds_of(context const& ctx, std::size_t index) const override;

   private:

      std::vector<float>      _tiles;
   };

   using htile_composite = vector_composite<htile_element>;

   template <typename... E>
   inline auto htile(E&&... elements)
   {
      using composite = array_composite<sizeof...(elements), htile_element>;
      using container = typename composite::container_type;
      composite r{};
      r = container{{ share(std::forward<E>(elements))... }};
      return r;
   }
}}

#endif
