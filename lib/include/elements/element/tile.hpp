/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_TILE_APRIL_13_2016)
#define ELEMENTS_TILE_APRIL_13_2016

#include <elements/view.hpp>
#include <elements/element/composite.hpp>
#include <elements/element/margin.hpp>
#include <memory>

namespace cycfi::elements
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
      extent                  _prev;
   };

   using vtile_composite = vector_composite<vtile_element>;

   template <concepts::Element... E>
   inline auto vtile(E&&... elements)
   {
      using composite = array_composite<sizeof...(elements), vtile_element>;
      using container = typename composite::container_type;
      composite r{};
      r = container{{share(std::forward<E>(elements))...}};
      return r;
   }

   template <concepts::Element FE, concepts::Element... E>
   inline auto vtile_spaced(float space, FE&& first, E&&... rest)
   {
      using composite = array_composite<1+(sizeof...(rest)), vtile_element>;
      composite r{};

      std::size_t i = 0;
      r[i] = share(std::forward<FE>(first));
      auto fill_interleaved = [&, i = 1](auto&& e) mutable
      {
         r[i++] = share(margin_top(space, std::forward<decltype(e)>(e)));
      };

      (fill_interleaved(rest), ...);
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
      extent                  _prev;
   };

   using htile_composite = vector_composite<htile_element>;

   template <concepts::Element... E>
   inline auto htile(E&&... elements)
   {
      using composite = array_composite<sizeof...(elements), htile_element>;
      using container = typename composite::container_type;
      composite r{};
      r = container{{share(std::forward<E>(elements))...}};
      return r;
   }

   template <concepts::Element FE, concepts::Element... E>
   inline auto htile_spaced(float space, FE&& first, E&&... rest)
   {
      using composite = array_composite<1+(sizeof...(rest)), htile_element>;
      composite r{};

      std::size_t i = 0;
      r[i] = share(std::forward<FE>(first));
      auto fill_interleaved = [&, i = 1](auto&& e) mutable
      {
         r[i++] = share(margin_left(space, std::forward<decltype(e)>(e)));
      };

      (fill_interleaved(rest), ...);
      return r;
   }
}

#endif
