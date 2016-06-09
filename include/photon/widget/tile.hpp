/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_TILE_APRIL_13_2016)
#define PHOTON_GUI_LIB_WIDGET_TILE_APRIL_13_2016

#include <photon/widget/widget.hpp>
#include <memory>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Vertical Tiles
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class vtile_widget : public composite_base
   {
   public:

      vtile_widget() {}
      ~vtile_widget() {}

      virtual rect         limits(basic_context const& ctx) const;
      virtual void         layout(context const& ctx);
      virtual rect         bounds_of(context const& ctx, std::size_t index) const;

   private:

      std::vector<float>   _tiles;
      float                _left;
      float                _right;
   };

   using vtile_composite = vector_composite<vtile_widget>;

   template <typename... W>
   inline auto vtile(W&&... elements)
   {
      array_composite<sizeof...(elements), vtile_widget> r{};
      r.elements = { new_(std::forward<W>(elements))... };
      return r;
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Horizontal Tiles
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class htile_widget : public composite_base
   {
   public:

      htile_widget() {}
      ~htile_widget() {}

      virtual rect         limits(basic_context const& ctx) const;
      virtual void         layout(context const& ctx);
      virtual rect         bounds_of(context const& ctx, std::size_t index) const;

   private:

      std::vector<float>   _tiles;
      float                _top;
      float                _bottom;
   };
   
   using htile_composite = vector_composite<htile_widget>;

   template <typename... W>
   inline auto htile(W&&... elements)
   {
      array_composite<sizeof...(elements), htile_widget> r{};
      r.elements = { new_(std::forward<W>(elements))... };
      return r;
   }
}

#endif
