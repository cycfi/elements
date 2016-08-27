/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_SLIDER_APRIL_16_2016)
#define PHOTON_GUI_LIB_WIDGET_SLIDER_APRIL_16_2016

#include <photon/widget/layer.hpp>
#include <photon/support.hpp>
#include <functional>
#include <memory>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Sliders
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class basic_vslider : public array_composite<3, layer_widget>
   {
   public:

      using base_type = array_composite<3, layer_widget>;

                           template <typename Slot, typename Gauge, typename Knob>
                           basic_vslider(Slot&& slot, Gauge&& gauge, Knob&& knob);

      float                state() const;
      void                 state(float state_);
   };

   template <typename Slot, typename Gauge, typename Knob>
   inline basic_vslider::basic_vslider(Slot&& slot, Gauge&& gauge, Knob&& knob)
   {
      elements[0] = new_(std::forward<Slot>(slot));
      elements[1] = new_(std::forward<Gauge>(gauge));
      elements[2] = new_(std::forward<Knob>(valign(0.0, knob)));
   }

   inline float basic_vslider::state() const
   {
      return std::dynamic_pointer_cast<align_widget_base>(elements[2])->align();
   }

   inline void basic_vslider::state(float state_)
   {
      std::dynamic_pointer_cast<align_widget_base>(elements[2])->align(state_);
   }
}

#endif