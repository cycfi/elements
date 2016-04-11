/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_MARGIN_APRIL_11_2016)
#define PHOTON_GUI_LIB_WIDGET_APRIL_1PHOTON_GUI_LIB_WIDGET_MARGIN_APRIL_11_20160_2016

#include <photon/widget/widget.hpp>
#include <memory>

namespace photon
{
	////////////////////////////////////////////////////////////////////////////////////////////////
	// Margins
	////////////////////////////////////////////////////////////////////////////////////////////////
   class margin
   {
   public:

      margin(rect const& margin_, std::shared_ptr<widget> subject)
       : subject(subject)
       , margin_(margin_)
      {}

      ~margin() {}

      virtual rect   limits() const;
      virtual void   draw(layout_info const& l);

   private:

      std::shared_ptr<widget> subject;
      rect margin_;
   };
}

#endif