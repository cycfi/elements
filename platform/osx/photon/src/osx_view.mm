/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#import <Cocoa/Cocoa.h>
#include <photon/view.hpp>
#include "osx_view_state.hpp"
#include "PhotonView.hpp"

namespace photon
{
   namespace
   {
      PhotonView* get_mac_view(view_impl* impl)
      {
         return (__bridge PhotonView*) impl;
      }
   }

   view::view()
    : _impl(nullptr)
    , _state(std::make_shared<view_state>())
   {}

   photon::canvas view::canvas()
   {
      auto ctx = (CGContextRef) [[NSGraphicsContext currentContext] graphicsPort];
      return photon::canvas{(canvas_impl*)ctx, *this};
   }
   
   point view::cursor_pos() const
   {
      auto pos = [NSEvent mouseLocation];
      return { float(pos.x), float(pos.y) };
   }
   
   point view::size() const
   {
      auto ns_view = get_mac_view(_impl);
      auto bounds = [ns_view bounds];
      return { float(bounds.size.width), float(bounds.size.height) };
   }
}
