/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/view.hpp>
#import <Cocoa/Cocoa.h>

namespace photon
{
   photon::canvas view::canvas() const
   {
      CGContextRef ctx = (CGContextRef) [[NSGraphicsContext currentContext] graphicsPort];
      return photon::canvas{(canvas_impl*)ctx};
   }
}
