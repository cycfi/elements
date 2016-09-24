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
   class flow_widget : public vector_composite<vtile_widget>
   {
   public:

      using base_type = vector_composite<vtile_widget>;

      using break_function =
               std::function<
                  void(
                     container& container_
                   , std::vector<widget_ptr>& rows
                   , basic_context const& ctx
                   , float width
                  )
               >;
                              flow_widget(
                                 container& container_
                               , break_function brf = default_break_function()
                              )
                               : _container(container_)
                               , _laid_out(false)
                               , _break_f(brf)
                              {}

      virtual widget_limits   limits(basic_context const& ctx) const;
      virtual void            layout(context const& ctx);

   private:

      static break_function   default_break_function();

      container&              _container;
      bool                    _laid_out;
      break_function          _break_f;
   };

   inline auto flow(container& container_)
   {
      return flow_widget{ container_ };
   }

   inline auto flow(container& container_, flow_widget::break_function brf)
   {
      return flow_widget{ container_, brf };
   }
}

#endif
