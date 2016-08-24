/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/view.hpp>
#import <Cocoa/Cocoa.h>
#include "osx_view_state.hpp"

namespace photon
{
   view::view()
    : _impl(nullptr)
    , _state(std::make_shared<view_state>())
   {}

   photon::canvas view::canvas()
   {
      CGContextRef ctx = (CGContextRef) [[NSGraphicsContext currentContext] graphicsPort];
      return photon::canvas{(canvas_impl*)ctx, *this};
   }
}
