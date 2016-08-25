/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_OSX_VIEW_STATE_AUGUST_22_2016)
#define PHOTON_GUI_LIB_OSX_VIEW_STATE_AUGUST_22_2016

#include <Quartz/Quartz.h>
#include <photon/support/canvas.hpp>

namespace photon
{
   struct view_state : std::enable_shared_from_this<view_state>
   {
      using view_state_ptr = std::shared_ptr<view_state>;

      enum fill_type
      {
         default_, linear, radial
      };

      view_state()
       : gradient(nullptr)
       , font_attributes(nullptr)
       , text_align(canvas::baseline)
       , saved(nullptr)
       , paint(default_)
      {}

      view_state(view_state_ptr prev)
       : gradient(CGGradientRetain(prev->gradient))
       , font_attributes(CFDictionaryCreateCopy(nullptr, prev->font_attributes))
       , text_align(canvas::baseline)
       , saved(prev)
       , linear_gradient(prev->linear_gradient)
       , radial_gradient(prev->radial_gradient)
       , paint(prev->paint)
      {
      }

      ~view_state()
      {
         if (gradient)
            CGGradientRelease(gradient);
         if (font_attributes)
            CFRelease(font_attributes);
      }



      CGGradientRef     gradient;
      CFDictionaryRef   font_attributes;
      int               text_align;
      view_state_ptr    saved;

      using linear_gradient_type = canvas::linear_gradient;
      using radial_gradient_type = canvas::radial_gradient;

      linear_gradient_type  linear_gradient;
      radial_gradient_type  radial_gradient;
      fill_type             paint;
   };
}

#endif