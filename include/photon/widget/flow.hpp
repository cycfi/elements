/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_FLOW_SEPTEMBER_23_2016)
#define PHOTON_GUI_LIB_WIDGET_FLOW_SEPTEMBER_23_2016

#include <photon/widget/composite.hpp>
#include <photon/widget/tile.hpp>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Flow Widget
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class flow_widget : public vector_composite<vtile_widget>
   {
   public:

      using base_type = vector_composite<vtile_widget>;

                              flow_widget(container& container_)
                               : _container(container_)
                               , _laid_out(false)
                              {}

      virtual widget_limits   limits(basic_context const& ctx) const;
      virtual void            layout(context const& ctx);

   private:

      container&              _container;
      bool                    _laid_out;
   };

   inline auto flow(container& container_)
   {
      return flow_widget{ container_ };
   }
}

#endif
