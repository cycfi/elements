/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_MARGIN_APRIL_11_2016)
#define PHOTON_GUI_LIB_WIDGET_MARGIN_APRIL_11_2016

#include <photon/widget/widget.hpp>
#include <memory>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Margins
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class margin_widget : public widget
   {
   public:

      margin_widget(rect const& margin_, std::shared_ptr<widget> subject)
       : subject(subject)
       , margin_(margin_)
      {}

      ~margin_widget() {}

      virtual rect   limits() const;
      virtual void   draw(layout_info const& l);

   private:

      std::shared_ptr<widget> subject;
      rect margin_;
   };

   inline widget_ptr
   margin(rect const& margin_, std::shared_ptr<widget> subject)
   {
      return widget_ptr{ new margin_widget{ margin_, subject } };
   }
}

#endif