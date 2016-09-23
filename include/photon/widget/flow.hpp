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
   class flow_widget : public composite_base
   {
   public:

      virtual widget_limits   limits(basic_context const& ctx) const;
      virtual void            layout(context const& ctx);
      virtual rect            bounds_of(context const& ctx, std::size_t index) const;

   private:

      using htile = range_composite<htile_widget>;
      using vtile = vector_composite<vtile_widget>;

      vtile                   _vtile;
   };
}

#endif
