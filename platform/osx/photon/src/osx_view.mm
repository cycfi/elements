/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#import <Cocoa/Cocoa.h>
#include <photon/view.hpp>
#include "PhotonView.hpp"
#import <cairo-quartz.h>

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
   {
      client::init(*this);
   }

   void view::setup_context()
   {
      auto ns_view = get_mac_view(_impl);
      auto w = [ns_view bounds].size.width;
      auto h = [ns_view bounds].size.height;
      auto context_ref = CGContextRef(NSGraphicsContext.currentContext.graphicsPort);
      _surface = cairo_quartz_surface_create_for_cg_context(context_ref, w, h);
      _context = cairo_create(_surface);
   }

   point view::cursor_pos() const
   {
      auto  ns_view = get_mac_view(_impl);
      auto  frame = [ns_view frame];
      auto  pos = [[ns_view window] mouseLocationOutsideOfEventStream];
      return { float(pos.x), float(frame.size.height - pos.y - 1) };
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
