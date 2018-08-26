/*=============================================================================
   Copyright (c) 2016-2018 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_FLOW_SEPTEMBER_23_2016)
#define PHOTON_GUI_LIB_WIDGET_FLOW_SEPTEMBER_23_2016

#include <photon/element/composite.hpp>
#include <photon/element/tile.hpp>
#include <functional>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////
   // Flow Element
   ////////////////////////////////////////////////////////////////////////////
   class flowable
   {
   public:

      virtual                 ~flowable() {}

      virtual void            break_lines(
                                 std::vector<element_ptr>& rows
                               , basic_context const& ctx
                               , float width
                              ) = 0;
   };

   class flowable_container : public flowable, public container
   {
   public:

      virtual void            break_lines(
                                 std::vector<element_ptr>& rows
                               , basic_context const& ctx
                               , float width
                              );

      virtual float           width_of(size_t index, basic_context const& ctx) const;
      virtual element_ptr     make_row(size_t first, size_t last);
   };

   class flow_element : public vector_composite<vtile_element>
   {
   public:

      using base_type = vector_composite<vtile_element>;

                              flow_element(flowable& flowable_)
                               : _flowable(flowable_)
                               , _laid_out(false)
                              {}

      virtual view_limits     limits(basic_context const& ctx) const;
      virtual void            layout(context const& ctx);

   private:

      flowable&               _flowable;
      bool                    _laid_out;
   };

   inline auto flow(flowable& flowable_)
   {
      return flow_element{ flowable_ };
   }
}

#endif
