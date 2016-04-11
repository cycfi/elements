/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_APRIL_10_2016)
#define PHOTON_GUI_LIB_WIDGET_APRIL_10_2016

#include <photon/support.hpp>
#include <photon/rect.hpp>
#include <photon/point.hpp>

namespace photon
{
	////////////////////////////////////////////////////////////////////////////////////////////////
	// Widgets
	////////////////////////////////////////////////////////////////////////////////////////////////
   class widget
   {
   public:

      widget() {}
      ~widget() {}

      widget(widget const&) = delete;
      widget& operator=(widget const&) = delete;

   // image

      virtual size_limits  limits() const;
      virtual widget*      hit_test(layout_info& l, point const& p);
      virtual void         draw(layout_info& l);
      virtual void         layout(rect const& bounds);

   // control

      virtual widget*      click(layout_info& l, point const& p);
      virtual bool         key(layout_info& l, key_info const& k);
      virtual bool         cursor(layout_info& l, point const& p);
   };
}

#endif