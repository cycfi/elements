/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_FLOW_SEPTEMBER_23_2016)
#define ELEMENTS_FLOW_SEPTEMBER_23_2016

#include <elements/element/composite.hpp>
#include <elements/element/tile.hpp>
#include <functional>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Flow Element
   ////////////////////////////////////////////////////////////////////////////
   class flowable_container : public container
   {
   public:

      void                    break_lines(
                                 std::vector<element_ptr>& rows
                               , basic_context const& ctx
                               , float width
                              );

      virtual float           width_of(size_t index, basic_context const& ctx) const;
      virtual element_ptr     make_row(size_t first, size_t last);

      void                    reflow()                { _reflow = true; }
      bool                    needs_reflow() const    { return _reflow; }
      void                    reflow_done()           { _reflow = false; }

   private:

      bool                    _reflow = true;

   };

   using flow_composite = vector_composite<flowable_container>;

   class flow_element : public vector_composite<vtile_element>
   {
   public:

      using base_type = vector_composite<vtile_element>;

                              flow_element(flowable_container& flowable_)
                               : _flowable(flowable_)
                              {}

      view_limits             limits(basic_context const& ctx) const override;
      void                    layout(context const& ctx) override;

   private:

      flowable_container&     _flowable;
   };

   inline auto flow(flowable_container& flowable_)
   {
      return flow_element{ flowable_ };
   }
}}

#endif
