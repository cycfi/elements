/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_ALIGN_APRIL_14_2016)
#define PHOTON_GUI_LIB_WIDGET_ALIGN_APRIL_14_2016

#include <photon/widget/widget.hpp>
#include <memory>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Alignment widgets
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class halign_widget : public proxy
   {
   public:

      halign_widget(double align, std::shared_ptr<widget> subject)
       : proxy(subject)
       , _align(align)
      {}

      ~halign_widget() {}

      virtual rect   limits() const;
      virtual void   subject_bounds(rect& b);

   private:

      double         _align;
   };

   inline widget_ptr halign(double align, std::shared_ptr<widget> subject)
   {
      return widget_ptr{ new halign_widget{ align, subject } };
   }
}

#endif