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
   {
      client::init(*this);
   }

   photon::canvas view::canvas()
   {
      auto ctx = (CGContextRef) [[NSGraphicsContext currentContext] graphicsPort];
      return photon::canvas{(canvas_impl*)ctx, *this};
   }

   point view::cursor_pos() const
   {
      auto ns_view = get_mac_view(_impl);
      auto global_pos = [NSEvent mouseLocation];
      auto window_pos = [[ns_view window] convertScreenToBase : global_pos];
      auto view_pos = [ns_view convertPoint : window_pos fromView: nil ];
      return { float(view_pos.x), float(view_pos.y) };
   }

   point view::size() const
   {
      auto ns_view = get_mac_view(_impl);
      auto frame = [ns_view frame];
      return { float(frame.size.width), float(frame.size.height) };
   }

   void view::size(point size_)
   {
      auto ns_view = get_mac_view(_impl);
      [[ns_view window] setContentSize:NSMakeSize(size_.x, size_.y)];
   }

   void view::limits(rect limits_) const
   {
      auto ns_view = get_mac_view(_impl);
      [[ns_view window] setContentMinSize: NSSize{ limits_.left, limits_.top }];
      [[ns_view window] setContentMaxSize: NSSize{ limits_.right, limits_.bottom }];
   }

   void view::refresh()
   {
      get_mac_view(_impl).needsDisplay = true;
   }

   void view::refresh(rect area)
   {
      [
         get_mac_view(_impl)
            setNeedsDisplayInRect : CGRectMake(area.left, area.right, area.width(), area.height())
      ];
   }
}
