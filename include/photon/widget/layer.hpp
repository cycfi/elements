/*=======================================================================================
   Copyright (c) 2016-2017 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=======================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_LAYER_APRIL_16_2016)
#define PHOTON_GUI_LIB_WIDGET_LAYER_APRIL_16_2016

#include <photon/widget/composite.hpp>
#include <algorithm>

namespace photon
{
   //////////////////////////////////////////////////////////////////////////////////////
   // Layer
   //////////////////////////////////////////////////////////////////////////////////////
   class layer_widget : public composite_base
   {
   public:

      virtual widget_limits   limits(basic_context const& ctx) const;
      virtual void            layout(context const& ctx);
      virtual hit_info        hit_element(context const& ctx, point p) const;
      virtual rect            bounds_of(context const& ctx, std::size_t index) const;
      virtual bool            focus(focus_request r);

      using composite_base::focus;

   private:

      void                    focus_top();
      rect                    bounds;
   };

   using layer_composite = vector_composite<layer_widget>;

   template <typename... W>
   inline auto layer(W&&... elements)
   {
      using composite = array_composite<sizeof...(elements), layer_widget>;
      composite r{};
      r = composite{{ share(std::forward<W>(elements))... }};
      std::reverse(r.begin(), r.end());
      return r;
   }

   //////////////////////////////////////////////////////////////////////////////////////
   // Deck
   //////////////////////////////////////////////////////////////////////////////////////
   class deck_widget : public layer_widget
   {
   public:
                           deck_widget()
                            : _selected_index(0)
                           {}

      virtual void         draw(context const& ctx);
      virtual void         refresh(context const& ctx, widget& widget);
      virtual hit_info     hit_element(context const& ctx, point p) const;
      virtual bool         focus(focus_request r);

      using composite_base::focus;

      void                 select(std::size_t index);
      std::size_t          selected() const { return _selected_index; }

   private:

      std::size_t          _selected_index;
   };

   using deck_composite = vector_composite<deck_widget>;

   template <typename... W>
   inline auto deck(W&&... elements)
   {
      using composite = array_composite<sizeof...(elements), deck_widget>;
      composite r{};
      r.elements = composite{ share(std::forward<W>(elements))... };
      std::reverse(r.begin(), r.end());
      return r;
   }

   template <typename... W>
   inline auto rdeck(W&&... elements)
   {
      using composite = array_composite<sizeof...(elements), deck_widget>;
      composite r{};
      r.elements = composite{ share(std::forward<W>(elements))... };
      return r;
   }
}

#endif
