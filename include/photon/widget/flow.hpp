/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_FLOW_SEPTEMBER_23_2016)
#define PHOTON_GUI_LIB_WIDGET_FLOW_SEPTEMBER_23_2016

#include <photon/widget/composite.hpp>
#include <photon/widget/tile.hpp>
#include <functional>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Flow Widget
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class flowable
   {
   public:

      virtual                 ~flowable() {}

      virtual void            break_lines(
                                 std::vector<widget_ptr>& rows
                               , basic_context const& ctx
                               , float width
                              );

      virtual std::size_t     size() const = 0;
      virtual float           width_of(size_t index, basic_context const& ctx) const = 0;
      virtual widget_ptr      make_row(size_t first, size_t last) = 0;
   };

   class flowable_container : public flowable, public container
   {
   public:

      virtual float           width_of(size_t index, basic_context const& ctx) const;
      virtual widget_ptr      make_row(size_t first, size_t last);
   };

   class flow_widget : public vector_composite<vtile_widget>
   {
   public:

      using base_type = vector_composite<vtile_widget>;

                              flow_widget(flowable& flowable_)
                               : _flowable(flowable_)
                               , _laid_out(false)
                              {}

      virtual widget_limits   limits(basic_context const& ctx) const;
      virtual void            layout(context const& ctx);

   private:

      flowable&               _flowable;
      bool                    _laid_out;
   };

   inline auto flow(flowable& flowable_)
   {
      return flow_widget{ flowable_ };
   }
}

#endif
