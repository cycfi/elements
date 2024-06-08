/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_FLOW_SEPTEMBER_23_2016)
#define ELEMENTS_FLOW_SEPTEMBER_23_2016

#include <elements/element/composite.hpp>
#include <elements/element/tile.hpp>
#include <functional>

namespace cycfi::elements
{
   /**
    * \class flowable_container
    *
    * \brief
    *    A container of elements that extends the `storage` class, adding
    *    behavior to handle elements in a flow layout.
    *
    *    `flowable_container` allows elements to flow and be positioned based
    *    on available space (like text in a paragraph). The class includes
    *    functions for breaking elements into rows/lines and recomputing the
    *    layout when needed.
    */
   class flowable_container : public storage
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

   /**
    * \class flow_element
    *
    * \brief
    *    A GUI element that extends `vector_composite<vtile_element>` class
    *    and provides behaviour for flowing elements.
    *
    *    `flow_element`'s instances are constructed with a reference to a
    *    `flowable_container` and use its data to manage their own layouts.
    */
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

   /**
    * \brief
    *    Helper function that creates a `flow_element` instance.
    *
    * \param flowable_
    *    A reference to the `flowable_container` that will be used to manage
    *    the flow layouts.
    *
    * \returns
    *    A `flow_element` instance configured with the given
    *    `flowable_container`.
    */
   inline auto flow(flowable_container& flowable_)
   {
      return flow_element{flowable_};
   }
}

#endif
